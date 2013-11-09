#include "animationuploader.h"
#include "PatternPlayer_Sketch.h"

#include <qDebug>


#define MAX_TIMER_INTERVAL 5000  // Longest time, in ms, that we will wait for a signal before we cancel this process.

AnimationUploader::AnimationUploader(QObject *parent) :
    QObject(parent)
{
    processTimer = new QTimer(this);

    watchdogTimer = new QTimer(this);
    connect(watchdogTimer, SIGNAL(timeout()), this, SLOT(watchdogTimer_timeout()));

//    connect(&serial, SIGNAL(readyRead()), this, SLOT(doWork()));

    state = State_Ready;
}

void AnimationUploader::updateProgress(int newProgress) {
    progress = newProgress;
    emit(progressChanged(progress));
}

void AnimationUploader::startUpload(BlinkyTape& tape, QByteArray animation, int frameRate) {
    updateProgress(0);

    // We can't reset if we weren't already connected...
    if(!tape.isConnected()) {
        return;
    }

/// Create the compressed image and check if it will fit into the device memory
    // TODO: Only write the sketch portion if we absolutely have to, to lower the risk
    // that we trash the firmware if there is a problem later

    // First, convert the sketch binary data into a qbytearray
    sketch = QByteArray(PatternPlayerSketch,PATTERNPLAYER_LENGTH);

    // Next, append the image data to it
    // TODO: Compress the animation
    sketch += animation;

    // Finally, write the metadata about the animation to the end of flash
    metadata = QByteArray(FLASH_MEMORY_PAGE_SIZE, 0xFF); // TODO: Connect this to the block size
    metadata[metadata.length()-6] = (PATTERNPLAYER_LENGTH >> 8) & 0xFF;
    metadata[metadata.length()-5] = (PATTERNPLAYER_LENGTH     ) & 0xFF;
    metadata[metadata.length()-4] = ((animation.length()/3/60) >> 8) & 0xFF;
    metadata[metadata.length()-3] = ((animation.length()/3/60)     ) & 0xFF;
    metadata[metadata.length()-2] = (1000/frameRate >> 8) & 0xFF;
    metadata[metadata.length()-1] = (1000/frameRate     ) & 0xFF;

    std::cout << std::hex;
    std::cout <<  "Sketch size (bytes): 0x" << PATTERNPLAYER_LENGTH << std::endl;
    std::cout << "Animation size (bytes): 0x" << animation.length() << std::endl;
    std::cout << "Image size (bytes): 0x" << sketch.length() << std::endl;
    std::cout << "Metadata size (bytes): 0x" << metadata.length() << std::endl;

    std::cout << "Animation address: 0x"
              << (int)metadata.at(122)
              << (int)metadata.at(123) << std::endl;
    std::cout << "Animation length: 0x"
              << (int)metadata.at(124)
              << (int)metadata.at(125) << std::endl;
    std::cout << "Animation Delay: 0x"
              << (int)metadata.at(126)
              << (int)metadata.at(127) << std::endl;

    // The entire sketch must fit into the available memory, minus a single page
    // at the end of flash for the configuration header
    // TODO: Could save ~100 bytes if we let the sketch spill into the unused portion
    // of the header.
    if(sketch.length() + metadata.length() > FLASH_MEMORY_AVAILABLE) {
        qCritical() << "sketch can't fit into memory!";
        // Emit fail message?
        return;
    }

    /// Attempt to reset the strip using the 1200 baud rate method, and identify the newly connected bootloader
    // TODO: Record the sketch and metadata somewhere so we can actually program them later!


    // First, record the currently available BlinkyTapes
    preResetTapes = BlinkyTape::findBlinkyTapes();

    // Next, remove the currently active port from the list of available BlinkyTapes
    QSerialPortInfo currentPort;
    if(!tape.getPortInfo(currentPort)) {
        return;
    }

    for(int i = 0; i < preResetTapes.length(); i++) {
        if(preResetTapes.at(i).portName() == currentPort.portName()) {
            preResetTapes.removeAt(i);
            break;
        }
    }

    // Next, tell the tape to reset.
    tape.reset();

    // Now, we want to wait 1 second and then start scanning to see if the port reappears
    // TODO: Is one second correct?
    // NOTE: Waiting to be sure that the port disappears doesn't seem to be reliable;
    // we don't necessicarily get a notification about that quick enough.
    state = State_WaitForBootloaderPort;
    processTimer->singleShot(2000,this,SLOT(doWork()));
}


void AnimationUploader::doWork() {
    // TODO: This flow is really ungainly

    qDebug() << "In doWork state=" << state;

    watchdogTimer->start(MAX_TIMER_INTERVAL); // Kick the watchdog.

    // Continue the current state
    switch(state) {
    case State_WaitForBootloaderPort:
        {
            // 1000 ms ago, we reset the BlinkyTape. It should now be in bootloader mode.
            // Compare the the blinkyTapes that we have now to the previous ones. If there
            // is a new one, then try to attach to it. Otherwise, stop.

            // Wait until we see a serial port appear again.
            QList<QSerialPortInfo> postResetTapes;
            bool portAdded = false;

            postResetTapes = BlinkyTape::findBlinkyTapes();
            if (postResetTapes.length() == preResetTapes.length() + 1) {
                portAdded = true;
                qCritical() << "Port added, hooray!";
            }
            if(!portAdded) {
                qCritical() << "Timeout waiting for port to appear...";
                return;
            }

            // Remove all of the tapes that are in the mid-reset list from the post-reset list
            qDebug() << "Post-reset tapes: " << postResetTapes.length();
            for(int i = 0; i < preResetTapes.length(); i++) {
                for(int j = 0; j < postResetTapes.length(); j++) {
                    if(preResetTapes.at(i).portName() == postResetTapes.at(j).portName()) {
                        postResetTapes.removeAt(j);
                        break;
                    }
                }
            }

            // Now, we should only have one new port.
            if(postResetTapes.length() != 1) {
                qCritical() << "Too many or two few ports, something went wrong??";
                return; // TODO: Fail.
            }

            qDebug() << "Bootloader waiting on: " << postResetTapes.at(0).portName();

            // Try to create a new programmer by connecting to the port
            if(!programmer.connectSerial(postResetTapes.at(0))) {
                qCritical() << "could not connect to programmer!";
                return;
            }

            // Send Check Device Signature command
            programmer.checkDeviceSignature();

            // TODO: Break this into pieces so we can respond individually, or just send it all
            // at once and fail gloriously?
            programmer.writeFlash(sketch, 0);
            programmer.writeFlash(metadata, FLASH_MEMORY_AVAILABLE - FLASH_MEMORY_PAGE_SIZE);
            programmer.reset();

            // TODO: Read back
            state = State_WaitForDeviceSignature;
        }
        break;
    case State_WaitForDeviceSignature:
        // TODO

        break;

    default:
        break;
    }

}


void AnimationUploader::watchdogTimer_timeout() {
    watchdogTimer->stop();
    qCritical() << "Watchdog tripped, we're done.";
    // TODO: Clear everything out.
}
