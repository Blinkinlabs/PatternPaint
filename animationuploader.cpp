#include "animationuploader.h"
#include "PatternPlayer_Sketch.h"

#include <qDebug>

// Amount of time, in ms, to wait after resetting the device before looking for it
#define RESET_WAIT_INTERVAL 2000

// Longest time, in ms, that we will wait for a signal before we cancel this process.
#define MAX_TIMER_INTERVAL 3000

// How long to wait between receiving notification that the programmer has been
// reset, and notifying the caller that we are finished
#define PROGRAMMER_RESET_DELAY 1000

AnimationUploader::AnimationUploader(QObject *parent) :
    QObject(parent)
{
    processTimer = new QTimer(this);

    state = State_Ready;

    connect(&programmer,SIGNAL(error(QString)),
            this,SLOT(handleProgrammerError(QString)));
    connect(&programmer,SIGNAL(commandFinished(QString,QByteArray)),
            this,SLOT(handleProgrammerCommandFinished(QString,QByteArray)));
}

void AnimationUploader::handleProgrammerError(QString error) {
    qCritical() << error;
    // TODO: not sure if we should do this, or let the programmer handle it?
    programmer.closeSerial();

    emit(finished(false));
}

void AnimationUploader::handleProgrammerCommandFinished(QString command, QByteArray returnData) {
    // TODO: Update our progress somehow? But how to tell how far we've gotten?
    qDebug() << "Command finished:" << command;
    updateProgress(progress + 1);

    // we know reset is the last command, so the BlinkyTape should be ready soon.
    // Schedule a timer to emit the message shortly.
    if(command == "reset") {
        QTimer::singleShot(PROGRAMMER_RESET_DELAY, this,SLOT(handleResetTimer()));
    }
}

void AnimationUploader::handleResetTimer()
{
    emit(finished(true));
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

    char buff[100];
    snprintf(buff, 100, "Sketch size: %iB, animation size: %iB, metadata size: %iB",
             PATTERNPLAYER_LENGTH,
             animation.length(),
             metadata.length());
    qDebug() << buff;

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
    processTimer->singleShot(RESET_WAIT_INTERVAL,this,SLOT(doWork()));
}


void AnimationUploader::doWork() {
    // TODO: This flow is really ungainly

    qDebug() << "In doWork state=" << state;

    // Continue the current state
    switch(state) {
    case State_WaitForBootloaderPort:
        {
            // RESET_WAIT_INTERVAL ms ago, we reset the BlinkyTape. It should now be in bootloader mode.
            // Compare the the blinkyTapes that we have now to the previous ones. If there
            // is a new one, then try to attach to it. Otherwise, stop.

//            // Wait until we see a serial port appear again.
//            QList<QSerialPortInfo> postResetTapes;
//            bool portAdded = false;

//            postResetTapes = BlinkyTape::findBlinkyTapes();
//            if (postResetTapes.length() == preResetTapes.length() + 1) {
//                portAdded = true;
//                qCritical() << "Port added, hooray!";
//            }
//            if(!portAdded) {
//                qCritical() << "Timeout waiting for port to appear...";
//                return;
//            }

//            // Remove all of the tapes that are in the mid-reset list from the post-reset list
//            qDebug() << "Post-reset tapes: " << postResetTapes.length();
//            for(int i = 0; i < preResetTapes.length(); i++) {
//                for(int j = 0; j < postResetTapes.length(); j++) {
//                    if(preResetTapes.at(i).portName() == postResetTapes.at(j).portName()) {
//                        postResetTapes.removeAt(j);
//                        break;
//                    }
//                }
//            }

//            // Now, we should only have one new port.
//            if(postResetTapes.length() != 1) {
//                qCritical() << "Too many or two few ports, something went wrong??";
//                return; // TODO: Fail.
//            }

            QList<QSerialPortInfo> postResetTapes;
            postResetTapes = BlinkyTape::findBlinkyTapeBootloaders();

            // Now, we should only have one new port.
            if(postResetTapes.length() == 0) {
                qCritical() << "Didn't detect a bootloader, something went wrong!";
                return; // TODO: Fail.
            }
            if(postResetTapes.length() > 1) {
                qWarning() << "Too many bootloaders, something is amiss.";
            }

            qDebug() << "Bootloader waiting on: " << postResetTapes.at(0).portName();

            // Try to create a new programmer by connecting to the port
            if(!programmer.openSerial(postResetTapes.at(0))) {
                qCritical() << "could not connect to programmer!";
                return;
            }
            else {
                qDebug() << "Connected to programmer!";
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
