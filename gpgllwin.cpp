#include "gpgllwin.h"
#include "ui_gpgllwin.h"

gpgllwin::gpgllwin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gpgllwin)
{
    ui->setupUi(this);

}

gpgllwin::~gpgllwin()
{
    delete ui;
}


void gpgllwin::setStatus(QChar ss)
{
    ui->stts->setText(ss);
}


void gpgllwin::setposMode(QChar spm)
{
    ui->posm->setText(spm);
}

void gpgllwin::setNSIndicator(QChar snsi)
{
    ui->nsi->setText(snsi);
}

void gpgllwin::setEWIndicator(QChar sewi)
{
    ui->ewi->setText(sewi);
}

void gpgllwin::setLatitude(double sla)
{
    ui->lat->setText(QString::number(sla,'f',10));
}

void gpgllwin::setLongitude(double slo)
{
    ui->lon->setText(QString::number(slo,'f',10));
}

void gpgllwin::setUTCH(int utch)
{
    ui->utch->setText(QString::number(utch));
}

void gpgllwin::setUTCM(int utcm)
{
    ui->utcm->setText(QString::number(utcm));
}

void gpgllwin::setUTCS(float utcs)
{
    ui->utcs->setText(QString::number(utcs));
}

void gpgllwin::setColorBasedOnStatus(QChar sc)
{
    if(sc == 'a' || sc == 'A')
    {
        ui->stts->setStyleSheet("background-color: #66bb66");
    }
    if(sc == 'v' || sc == 'V')
    {
        ui->stts->setStyleSheet("background-color: #eeee00");
    }
}

void gpgllwin::disconnected()
{
    ui->stts->setStyleSheet("background-color: #eeeeee");
}
