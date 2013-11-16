#include "systeminformation.h"
#include "ui_systeminformation.h"

#include "blinkytape.h"

#include <QSerialPortInfo>
#include <QSysInfo>
#include <QLibraryInfo>

SystemInformation::SystemInformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemInformation)
{
    ui->setupUi(this);
    QString report;

    QString osName;

    report.append("Pattern Paint (Unknown version)\r");
    report.append("  Build Date: ");
    report.append(__DATE__);
    report.append(" ");
    report.append(__TIME__);
    report.append("\r");

#if defined(Q_OS_WIN)
    switch(QSysInfo::windowsVersion()) {
    case QSysInfo::WV_2000:
        osName = "Windows 2000";
        break;
    case QSysInfo::WV_2003:
        osName = "Windows 2003";
        break;
    case QSysInfo::WV_VISTA:
        osName = "Windows Vista";
        break;
    case QSysInfo::WV_WINDOWS7:
        osName = "Windows 7";
        break;
    case QSysInfo::WV_WINDOWS8:
        osName = "Windows 8";
        break;
    default
        osName = "Windows (Unknown Version)";
        break;
    }
#elif defined(Q_OS_MAC)
    switch(QSysInfo::macVersion()) {
    case QSysInfo::MV_SNOWLEOPARD:
        osName = "OS X 10.6 (Snow Leopard)";
        break;
    case QSysInfo::MV_LION:
        osName = "OS X 10.7 (Lion)";
        break;
    case QSysInfo::MV_MOUNTAINLION:
        osName = "OS X 10.8 (Mountain Lion)";
        break;
    case QSysInfo::MV_MAVERICKS:
        osName = "OS X 10.9 (Mavericks)";
        break;
    default:
        osName = "OS X (Unknown version)";
        break;
    }
#else
// TODO: Linux
    osName = "Unknown";
#endif
    report.append("Operating system: " + osName + "\r");

    report.append("QT information:\r");
    report.append("  buildDate: " + QLibraryInfo::buildDate().toString() + "\r");
    report.append("  licensee: " + QLibraryInfo::licensee() + "\r");
    report.append("  path: " + QLibraryInfo::location(QLibraryInfo::LibrariesPath) + "\r");

    report.append("Detected Serial Ports: \r");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        report.append("  " + info.portName() + "\r");
        report.append("    Manufacturer: " + info.manufacturer() + "\r");
        report.append("    Description: " + info.description() + "\r");
        report.append("    VID: 0x" + QString::number(info.vendorIdentifier(),16) + "\r");
        report.append("    PID: 0x" + QString::number(info.productIdentifier(),16) + "\r");
    }

    report.append("Detected BlinkyTapes: \r");
    foreach (const QSerialPortInfo &info, BlinkyTape::findBlinkyTapes()) {
        report.append("  " + info.portName() + "\r");
        report.append("    Manufacturer: " + info.manufacturer() + "\r");
        report.append("    Description: " + info.description() + "\r");
        report.append("    VID: 0x" + QString::number(info.vendorIdentifier(),16) + "\r");
        report.append("    PID: 0x" + QString::number(info.productIdentifier(),16) + "\r");
    }

    report.append("Detected BlinkyTape Bootloaders: \r");
    foreach (const QSerialPortInfo &info, BlinkyTape::findBlinkyTapeBootloaders()) {
        report.append("  " + info.portName() + "\r");
        report.append("    Manufacturer: " + info.manufacturer() + "\r");
        report.append("    Description: " + info.description() + "\r");
        report.append("    VID: 0x" + QString::number(info.vendorIdentifier(),16) + "\r");
        report.append("    PID: 0x" + QString::number(info.productIdentifier(),16) + "\r");
    }

    ui->infoBrowser->setText(report);
}

SystemInformation::~SystemInformation()
{
    delete ui;
}
