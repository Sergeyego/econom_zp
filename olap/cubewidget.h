#ifndef CUBEWIDGET_H
#define CUBEWIDGET_H

#include <QWidget>
#include "olapmodel.h"
#include <QCalendarWidget>
#include "axiswidget.h"

namespace Ui {
class CubeWidget;
}

class CubeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CubeWidget(QString head, QStringList axes, QString qu, int dec, QWidget *parent = 0);
    explicit CubeWidget(int id_cube, QWidget *parent = 0);
    ~CubeWidget();
    void setRange(QDate beg, QDate end, bool block=true);
    void setSum(double s);
    
private:
    void inital(QString head, QStringList axes, QString qu, int dec);
    Ui::CubeWidget *ui;
    OlapModel *olapmodel;
    QString query;
    AxisWidget *axisX;
    AxisWidget *axisY;
    int decimal;
    double sum;

private slots:
    void updQuery();
    void saveXls();

};

#endif // CUBEWIDGET_H
