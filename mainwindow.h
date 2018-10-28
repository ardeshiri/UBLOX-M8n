#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QList>
#include "mgps.h"
#include "gpgllwin.h"
#include <QSound>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void manageConnections();
    mGPS* mgps;
    gpgllwin* gpgllw;
    bool connectionIndicator;

public slots:
void portsListUpdate();
void showOpenPortsInfo();
void play1();
void play2();
void play3();
void play4();


private slots:

void GPSDisconnectedByItsOwnSignal();
void validDeviceHandler();
void on_connect_clicked();
void on_disconnect_clicked();
void on_gpgll_clicked();
void recording();
void recordingStopped();
};

#endif // MAINWINDOW_H
