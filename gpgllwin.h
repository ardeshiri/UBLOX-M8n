#ifndef GPGLLWIN_H
#define GPGLLWIN_H

#include <QDialog>

namespace Ui {
class gpgllwin;
}

class gpgllwin : public QDialog
{
    Q_OBJECT

public:
    explicit gpgllwin(QWidget *parent = 0);
    ~gpgllwin();


private:
    Ui::gpgllwin *ui;


public slots:
    void setStatus(QChar ss);
    void setposMode(QChar spm);
    void setNSIndicator(QChar snsi);
    void setEWIndicator(QChar sewi);
    void setLatitude(double sla);
    void setLongitude(double slo);
    void setUTCH(int utch);
    void setUTCM(int utcm);
    void setUTCS(float utcs);
    void setColorBasedOnStatus(QChar sc);
    void disconnected();


};

#endif // GPGLLWIN_H
