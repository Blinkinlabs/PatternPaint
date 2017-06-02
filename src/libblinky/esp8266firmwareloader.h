#ifndef ESP8266FIRMWARELOADER_H
#define ESP8266FIRMWARELOADER_H

#include "firmwareloader.h"
#include "serialcommandqueue.h"

#include <QObject>

class Esp8266FirmwareLoader : public FirmwareLoader
{
    Q_OBJECT

public:
    Esp8266FirmwareLoader(QObject *parent = 0);

    bool updateFirmware(BlinkyController &controller);
    bool restoreFirmware(qint64 timeout);
    QString getErrorString() const;

public slots:
    void cancel();

private slots:
    void doWork();  /// Handle the next section of work, whatever it is

    void handleError(QString error);

    void handleCommandFinished(QString command, QByteArray returnData);

    void handleLastCommandFinished();

private:
    /// Update any listeners with the latest progress
    void setProgress(int newProgress);

    QString errorString;

    QSerialPortInfo serialPortInfo;
    QSerialPort serialPort;

    SerialCommandQueue commandQueue;

    enum State {
        State_startReset,
        State_assertRTS,
        State_assertDTR,
        State_releaseBootPins,
        State_connectCommandQueue,
        State_startBootloaderSync,
        State_waitForBootloaderSync,
        State_doFlashDownload,
        State_Done                 ///< And we're done!
    };

    State state;
    int resetLoopsRemaining;
    int syncTriesRemaining;

    int progress;
    int maxProgress;

    QList<QByteArray> flashData;
};

#endif // ESP8266FIRMWARELOADER_H
