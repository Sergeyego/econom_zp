#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "modelprod.h"
#include "db/dbtablemodel.h"
#include "olap/cubewidget.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    ModelProd *modelProd;
    DbTableModel *modelNorm;
    DbRelation *relTypeJob;
    QVector<qlonglong> buf;
    bool ready();

private slots:
    void updProd();
    void updNorm(QModelIndex ind);
    void copy();
    void paste();
    void repNorm();
    void repFact();
    void repNormPF();
    void repFactPF();
};

#endif // MAINWIDGET_H
