#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    portsListUpdate();
    showOpenPortsInfo();

    mgps = new mGPS{this};
    setWindowTitle("ublox parser");
    connectionIndicator = false;
    gpgllw = new gpgllwin{this};
    gpgllw ->setWindowTitle("gpgll");

    manageConnections();

}

/////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    delete ui;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::manageConnections()
{
    connect(ui->refresh,SIGNAL(clicked()),this,SLOT(portsListUpdate()));
    connect(ui->record,SIGNAL(toggled(bool)),mgps,SLOT(recordToFile(bool)));

    connect(mgps,SIGNAL(receivedALine(QString)),ui->textEdit,SLOT(append(QString)));
    connect(mgps,SIGNAL(gpsPortDisconnected()),this,SLOT(GPSDisconnectedByItsOwnSignal()));
    connect(mgps,SIGNAL(validDevice()),this,SLOT(validDeviceHandler()));
    connect(mgps,SIGNAL(gpsPortDisconnected()),this,SLOT(play3()));
    connect(mgps,SIGNAL(writingOnFileStarted()),this,SLOT(recording()));
    connect(mgps,SIGNAL(writingOnFileStopped()),this,SLOT(recordingStopped()));


    connect(mgps,SIGNAL(statusUpdate(QChar)),gpgllw,SLOT(setStatus(QChar)));
    connect(mgps,SIGNAL(posMUpdate(QChar)),gpgllw,SLOT(setposMode(QChar)));
    connect(mgps,SIGNAL(latitudeDegreesCompleteUpdate(double)),gpgllw,SLOT(setLatitude(double)));
    connect(mgps,SIGNAL(longitudeDegreesCompleteUpdate(double)),gpgllw,SLOT(setLongitude(double)));
    connect(mgps,SIGNAL(UTCHUpdate(int)),gpgllw,SLOT(setUTCH(int)));
    connect(mgps,SIGNAL(UTCMUpdate(int)),gpgllw,SLOT(setUTCM(int)));
    connect(mgps,SIGNAL(UTCSUpdate(float)),gpgllw,SLOT(setUTCS(float)));
    connect(mgps,SIGNAL(NSIndicatorUpdate(QChar)),gpgllw,SLOT(setNSIndicator(QChar)));
    connect(mgps,SIGNAL(EWIndicatorUpdate(QChar)),gpgllw,SLOT(setEWIndicator(QChar)));
    connect(mgps,SIGNAL(statusUpdate(QChar)),gpgllw,SLOT(setColorBasedOnStatus(QChar)));
    connect(mgps,SIGNAL(gpsPortDisconnected()),gpgllw,SLOT(disconnected()));


}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::portsListUpdate()
{
    if(!connectionIndicator)
    {
    ui->comboBox->clear();
    for(auto o:QSerialPortInfo::availablePorts())
       ui->comboBox->addItem(o.portName());
    std::cout<<"ports updated!"<<std::endl;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::showOpenPortsInfo()
{
    for(auto o:QSerialPortInfo::availablePorts())
    {
        std::cout<<"Name: "<<(o.portName()).toStdString()<<" -- "<<"Manufacturer: "<<  (o.manufacturer()).toStdString()<<" -- "<<"Pro ID:"<< o.productIdentifier()<<" -- "<<"Desc: "<<o.description().toStdString()<<" -- "<<"SNum:"<<o.serialNumber().toStdString()<<" -- "<<"VendorID:"<<o.vendorIdentifier()<<std::endl;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::GPSDisconnectedByItsOwnSignal()
{
    ui->comboBox->setStyleSheet("background-color:#dddddd");
    connectionIndicator = false;

}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::validDeviceHandler()
{
    connectionIndicator = true;
    play4();
    ui->comboBox->setStyleSheet("background-color:#44dd44");
    ui->statusBar->showMessage("valid device!", 4000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_connect_clicked()
{
    play2();
    if(ui->comboBox->currentText()=="")
    {
        std::cout<<"nothing to select.\n";
        return;
    }
    else
    {
    if(mgps->setSerialPort(ui->comboBox->currentText()))
    {
        ui->comboBox->setStyleSheet("background-color:yellow");
    }
    else {
        return;
    }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_disconnect_clicked()
{
    connectionIndicator = false;
    mgps->closePort();
    ui->comboBox->setStyleSheet("background-color:#dddddd");
    portsListUpdate();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::on_gpgll_clicked()
{
    gpgllw -> show();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::play1()
{
    QSound::play(":/be.wav");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::play2()
{
    QSound::play(":/bu3.wav");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::play3()
{
    QSound::play(":/bu5.wav");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::play4()
{
    QSound::play(":/bu9.wav");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::recording()
{
    ui -> record -> setStyleSheet("background-color:#dd7777");
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::recordingStopped()
{
    ui -> record -> setStyleSheet("background-color:#dddddd");

}



