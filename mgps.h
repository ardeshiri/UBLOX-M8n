#ifndef MGPS_H
#define MGPS_H

#include <QObject>
#include <QSerialPort>
#include <iostream>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

class mGPS : public QObject
{
    Q_OBJECT

public:
    explicit mGPS(QObject *parent = nullptr,QString SPName = "");
    ~mGPS();
    bool setSerialPort(QString SPName);
    bool closePort();


private:
    QSerialPort* GPSSerialPort;
    QTimer* wDogTimer;
    QFile* GPSRecordFile;
    QTextStream fileStream;
    QDir GPSRecordsDirection;
    QString GPSOs{};
    char GPSOch{};
    int attemptCtr;
    int invalidLineCtr;
    QString GPSCurrentPortName{};
    bool validDeviceIndicator;

    void manageConnections();



signals:
     void validPort(bool);
     void gpsPortOpened(bool);
     void gpsPortConnected(bool);
     void gpsPortDisconnected();
     void receivedALine(QString qs);
     void validDevice();
     void invalidLine(QString qs);
     void invalidDevice(QString qs);
     void portReset();
     void writingOnFileStarted();
     void writingOnFileStopped();

     void latitudeMinuteUpdate(double lttm);
     void latitudeDegreesCompleteUpdate(double ldc);
     void latitudeDegreeUpdate(int lttd);
     void NSIndicatorUpdate(QChar nsi);
     void longitudeDegreesCompleteUpdate(double ldc);
     void longitudeMinuteUpdate(double lntm);
     void longitudeDegreeUpdate(int lntd);
     void EWIndicatorUpdate(QChar nsi);
     void UTCSUpdate(float utcs);
     void UTCMUpdate(int utcm);
     void UTCHUpdate(int utch);
     void statusUpdate(QChar sts);
     void posMUpdate(QChar posm);
     void gpgllmsg();


private slots:
    void errorOccurred(QSerialPort::SerialPortError);
    void writeLineOnConsole(QString qs);
    void wDogTimerOverflowHandler();
    void invalidLineHandler(QString qs);
    void GPSDataParser(QString qs);
    void writeOnFile(QString wf);


public slots:
    void readGPSRawData();
    void recordToFile(bool rtf);

};

#endif // MGPS_H
