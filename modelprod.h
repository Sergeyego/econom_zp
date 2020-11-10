#ifndef MODELPROD_H
#define MODELPROD_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include <QDebug>

class ModelProd : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelProd(QObject *parent);
    void refresh(QDate beg, QDate end);
    QVariant data(const QModelIndex &item, int role) const;
private:
    QStringList exList;
public slots:
    void updState();
};

#endif // MODELPROD_H
