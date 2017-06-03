#ifndef FIRMWARELOADER_H
#define FIRMWARELOADER_H

#include <QObject>
#include <QList>

class BlinkyController;

/// This is an re-entreant version of an pattern uploader.
/// Each task in the upload process is broken into a single state, and the state
/// machine is driven forward by receiving events from the connected serial port,
/// or in the case of a timeout, from the timeout timer.
///
/// While the upload process is underway, it will send periodic progress updates
/// via the progressUpdate() signal.
class FirmwareLoader : public QObject
{
    Q_OBJECT

public:
    FirmwareLoader(QObject *parent = 0) :
        QObject(parent)
    {
    }

    /// Update/restore the firmware on the specificed device
    /// @param tape BlinkyTape to upload to (must already be connected)
    virtual bool updateFirmware(BlinkyController &controller) = 0;

    /// Scan for a device bootloader, then attempt to restore the firmware to it.
    /// @param timeout Number of milliseconds to wait for a bootloader, or -1 to wait
    /// indefinetly
    virtual bool restoreFirmware(qint64 timeout) = 0;

    /// Get a string describing the last error, if any.
    virtual QString getErrorString() const = 0;

signals:
    /// Sends an update about the upload progress, from 0 to 100
    void progressChanged(int progress);

    /// Send an update for the text
    void setText(QString textLabel);

    /// Sends a signal at end of upload to report the result.
    void finished(bool result);

public slots:
    /// Request that the upload process be stopped
    virtual void cancel() = 0;
};

#endif // FIRMWARELOADER_H
