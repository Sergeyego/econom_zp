#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "modelprod.h"
#include "db/dbtablemodel.h"

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

private slots:
    void updProd();
    void updNorm(QModelIndex ind);
    void copy();
    void paste();
};

#endif // MAINWIDGET_H
