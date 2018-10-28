#include "mgps.h"

mGPS::mGPS(QObject *parent,QString SPName) : QObject(parent)
{

    GPSSerialPort = new QSerialPort{this};
    wDogTimer = new QTimer{this};
    GPSRecordFile = new QFile{this};

    setSerialPort(SPName);
    wDogTimer -> setInterval(3000);
    attemptCtr = 0;
    invalidLineCtr = 0;
    validDeviceIndicator = false;

    manageConnections();

}

/////////////////////////////////////////////////////////////////////////////////////////////////

mGPS::~mGPS()
{
    closePort();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void mGPS::manageConnections()
{
    connect(GPSSerialPort,SIGNAL(errorOccurred(QSerialPort::SerialPortError)),this,SLOT(errorOccurred(QSerialPort::SerialPortError)));
    //connect(this,SIGNAL(receivedALine(QString)),this,SLOT(writeLineOnConsole(QString)));
    connect(GPSSerialPort,SIGNAL(readyRead()),this,SLOT(readGPSRawData()));
    connect(this,SIGNAL(receivedALine(QString)),wDogTimer,SLOT(start()));
    connect(wDogTimer,SIGNAL(timeout()),this,SLOT(wDogTimerOverflowHandler()));
    connect(this,SIGNAL(receivedALine(QString)),this,SLOT(GPSDataParser(QString)));

}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool mGPS::setSerialPort(QString SPName)
{
    if(SPName == "")
    {
       std::cout<<"GPS port not set!"<<std::endl;
       emit gpsPortConnected(false);
       return false;
    }

    QSerialPortInfo SPInfo(SPName);

    if(SPInfo.isBusy())
       {
        std::cout<<"Given port is busy!"<<std::endl;
        emit gpsPortConnected(false);
        return false;
       }

    else
    {
       GPSCurrentPortName = SPName;
       GPSSerialPort->setPort(SPInfo);
       std::cout<<"GPS port is set!"<<std::endl;

       GPSSerialPort->setBaudRate(QSerialPort::Baud9600);
       GPSSerialPort->setDataBits(QSerialPort::Data8);
       GPSSerialPort->setParity(QSerialPort::NoParity);
       GPSSerialPort->setStopBits(QSerialPort::OneStop);
       GPSSerialPort->setFlowControl(QSerialPort::NoFlowControl);

       if(GPSSerialPort->open(QIODevice::ReadWrite))
       {
           std::cout<<"GPS port is open!"<<std::endl;
           emit gpsPortConnected(true);
           emit gpsPortOpened(true);
           wDogTimer->start();
           return true;
       }
       else
       {
           std::cout<<"Given port could not be opened!"<<std::endl;
           emit gpsPortConnected(false);
           emit gpsPortOpened(false);
           return false;
       }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void mGPS::errorOccurred(QSerialPort::SerialPortError er)
{
    std::cout<<"error number -- "<<er<<" -- occurred!"<<std::endl;
    if(er==9)
    {
      closePort();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool mGPS::closePort()
{
    wDogTimer ->stop();

    if(GPSRecordFile->isOpen())
        GPSRecordFile->close();

    if(GPSSerialPort->isOpen())
    {
    GPSSerialPort->clear();
    GPSSerialPort->close();
    std::cout<<"GPS port closed!"<<std::endl;
    emit gpsPortDisconnected();
    validDeviceIndicator = false;

    return true;
    }
    else
    {
    std::cout<<"No open port to closed!"<<std::endl;
    return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void mGPS::readGPSRawData()
{

    while (GPSSerialPort->bytesAvailable())
  {
    GPSSerialPort-> getChar(&GPSOch);

    if(GPSOch == '\r')
        continue;

    if(GPSOs.length()>100)
    {
        emit invalidLine(GPSOs);
        GPSOs = "";
        attemptCtr = 0;     //got something, no more atempt to reset the port
        break;
    }


    if(GPSOch != '\n')
     {
        GPSOs+=GPSOch;
     }
    else
     {
        GPSOs+=GPSOch;
     if(GPSOs.at(0)=='$')
        {

         if(validDeviceIndicator == false)
         {
            emit validDevice();
            validDeviceIndicator = true;
         }

         emit receivedALine(GPSOs);
         GPSOs = "";
         invalidLineCtr = 0;   //got a sane line, no more effort to check for invalid input
         attemptCtr = 0;   //got a sane line, no need to reset the port
        }
     else
        {
         emit invalidLine(GPSOs);
         GPSOs = "";
         attemptCtr = 0;   //got something, no need to reset the port
        }
     }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void mGPS::writeLineOnConsole(QString qs)
{
    std::cout<<qs.toStdString();
}



void mGPS::wDogTimerOverflowHandler()
{
    ++attemptCtr;
    std::cout<<"No answer from device in 3 Seconds!\nTrying to restart.";

    if(attemptCtr<4)
    {
        std::cout<<"Attempt number: "<<attemptCtr<<"\n";
        closePort();
        setSerialPort(GPSCurrentPortName);
        emit portReset();
    }

    else
    {
        std::cout<<"Attemps reached the limit(3), port closed."<<"\n";
        closePort();
        GPSCurrentPortName = "";
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////

void  mGPS::invalidLineHandler(QString qs)
{
    ++invalidLineCtr;
    if(invalidLineCtr<5)
    {
        std::cout<<invalidLineCtr<<" Invalid inputs.\n";
    }
    else
    {
        std::cout<<invalidLineCtr<<" Invalid inputs, reached max invalid inputs, going to close port.\n";
        emit invalidDevice(qs);
        closePort();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////

void mGPS::GPSDataParser(QString qs)
{
    QStringList qsl = qs.split(',');
/*
    if(qsl.at(0)=="$GPDTM")
        emit gpdtmmsg(qsl);

    if(qsl.at(0)=="$GPGBQ")
        emit gpgbqmsg(qsl);

    if(qsl.at(0)=="$GPGBS")
        emit gpgbsmsg(qsl);

    if(qsl.at(0)=="$GPGGA")
        emit gpggamsg(qsl);
*/
    if(qsl.at(0)=="$GPGLL")
    {
        emit gpgllmsg();

        if(!(qsl.at(1)).isEmpty())
        {
            emit latitudeDegreeUpdate((qsl.at(1)).left(2).toInt());
        }

        if(!(qsl.at(1)).isEmpty())
        {
            emit latitudeMinuteUpdate((qsl.at(1)).right(8).toDouble());
        }

        if(!(qsl.at(1)).isEmpty())
        {
            emit latitudeDegreesCompleteUpdate(((qsl.at(1)).right(8).toDouble()/60)+(qsl.at(1)).left(2).toInt());
        }


        if(!(qsl.at(2)).isEmpty())
        {
            emit NSIndicatorUpdate((qsl.at(2)).at(0));
        }

        if(!(qsl.at(3)).isEmpty())
        {
            emit longitudeDegreeUpdate((qsl.at(3)).left(3).toInt());
        }

        if(!(qsl.at(3)).isEmpty())
        {
            emit longitudeMinuteUpdate((qsl.at(3)).right(8).toDouble());
        }

        if(!(qsl.at(3)).isEmpty())
        {
            emit longitudeDegreesCompleteUpdate(((qsl.at(3)).right(8).toDouble()/60)+(qsl.at(3)).left(3).toInt());
        }

        if(!(qsl.at(4)).isEmpty())
        {
            emit EWIndicatorUpdate((qsl.at(4)).at(0));
        }

        if(!(qsl.at(5)).isEmpty())
        {
            emit UTCHUpdate(((qsl.at(5)).left(2)).toInt());
            emit UTCMUpdate(((qsl.at(5)).mid(2,2)).toInt());
            emit UTCSUpdate(((qsl.at(5)).right(5)).toFloat());
        }


        if(!(qsl.at(6)).isEmpty())
        {
            emit statusUpdate((qsl.at(6)).at(0));
        }

        if(!(qsl.at(7)).isEmpty())
        {
            emit posMUpdate((qsl.at(7)).at(0));
        }
    }

/*
    if(qsl.at(0)=="$GPGLQ")
        emit gpglqmsg(qsl);

    if(qsl.at(0)=="$GPGNQ")
        emit gpgnqmsg(qsl);

    if(qsl.at(0)=="$GPGNS")
        emit gpgnsmsg(qsl);

    if(qsl.at(0)=="$GPGPQ")
        emit gpgpqmsg(qsl);

    if(qsl.at(0)=="$GPGRS")
        emit gpgrsmsg(qsl);

    if(qsl.at(0)=="$GPGSA")
        emit gpgsamsg(qsl);

    if(qsl.at(0)=="$GPGST")
        emit gpgstmsg(qsl);

    if(qsl.at(0)=="$GPGSV")
        emit gpgsvmsg(qsl);

    if(qsl.at(0)=="$GPRMC")
        emit gprmcmsg(qsl);

    if(qsl.at(0)=="$GPTXT")
        emit gptxtmsg(qsl);

    if(qsl.at(0)=="$GPVLW")
        emit gpvlwmsg(qsl);

    if(qsl.at(0)=="$GPVTG")
        emit gpvtgmsg(qsl);

    if(qsl.at(0)=="$GPZDA")
        emit gpzdamsg(qsl);

    if(qsl.at(0)=="$PUBX")
        emit pubxmsg(qsl);
*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void mGPS::recordToFile(bool rtf)
{
    if(rtf==true)
{
    QString filePath{""};
    QString fileName{};

    fileName = "Record_";
    fileName += QString::number(QDateTime::currentDateTime().time().hour());
    fileName += "_";
    fileName += QString::number(QDateTime::currentDateTime().time().minute());
    fileName += "_";
    fileName += QString::number(QDateTime::currentDateTime().time().second());
    fileName += ".txt";

    filePath = QDir::currentPath();
    filePath += "/Records/";
    filePath += QDateTime::currentDateTime().date().toString();

    GPSRecordsDirection.setPath(filePath);

    if(!GPSRecordsDirection.exists())
        GPSRecordsDirection.mkpath(filePath);

    GPSRecordFile->setFileName(GPSRecordsDirection.absoluteFilePath(fileName));

    if(!GPSRecordFile->open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        std::cout<<"Could not open file: "<<GPSRecordFile->fileName().toStdString()<<" to write on. "<<std::endl;
        return;
    }
    else
    {
        std::cout<<"File: "<<GPSRecordFile->fileName().toStdString()<<" opened. recording... "<<std::endl;
        fileStream.setDevice(GPSRecordFile);
        emit writingOnFileStarted();
        connect(this,SIGNAL(receivedALine(QString)),this,SLOT(writeOnFile(QString)));
        return;
    }
}

    else
    {
        std::cout<<"File: "<<GPSRecordFile->fileName().toStdString()<<" closed. "<<std::endl;
        fileStream<<"Recording stopped ----- "<<QDateTime::currentDateTime().currentDateTime().toString()<<"\n";
        disconnect(this,SIGNAL(receivedALine(QString)),this,SLOT(writeOnFile(QString)));
        emit writingOnFileStopped();
        GPSRecordFile->close();
        fileStream.flush();
        fileStream.reset();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void mGPS::writeOnFile(QString wf)
{
    fileStream<<wf;
}


