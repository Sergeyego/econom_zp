#include "modelprod.h"

ModelProd::ModelProd(QObject *parent) : QSqlQueryModel(parent)
{

}

void ModelProd::refresh(QDate beg, QDate end)
{
    QSqlQuery query;
    query.prepare("select distinct m.id_provol, m.id_diam, p.id_pack, pr.nam, d.sdim, k.short "
                  "from wire_in_cex_data w "
                  "inner join wire_in_cex_type t on w.id_type=t.id and t.koef=1 "
                  "inner join wire_parti p on w.id_wparti=p.id "
                  "inner join wire_parti_m m on p.id_m=m.id "
                  "inner join provol pr on pr.id=m.id_provol "
                  "inner join diam d on d.id=m.id_diam "
                  "inner join wire_pack_kind as k on k.id=p.id_pack "
                  "where w.dat between :d1 and :d2 "
                  "order by pr.nam, d.sdim, k.short");
    query.bindValue(":d1",beg);
    query.bindValue(":d2",end);
    if (query.exec()){
        setQuery(query);
        setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Марка"));
        setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Диаметр"));
        setHeaderData(5,Qt::Horizontal,QString::fromUtf8("Носитель"));
        updState();
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
}

QVariant ModelProd::data(const QModelIndex &item, int role) const
{
    if (role==Qt::BackgroundColorRole){
        QString s=this->data(this->index(item.row(),0),Qt::EditRole).toString();
        s+=":";
        s+=this->data(this->index(item.row(),1),Qt::EditRole).toString();
        s+=":";
        s+=this->data(this->index(item.row(),2),Qt::EditRole).toString();
        return exList.contains(s) ? QVariant(QColor(255,255,255)) : QVariant(QColor(255,170,170));
    }
    return QSqlQueryModel::data(item,role);
}

void ModelProd::updState()
{
    QSqlQuery query;
    query.prepare("select distinct id_provol||':'||id_diam||':'|| id_spool as nam from econom_zp_norm order by nam");
    if (query.exec()){
        exList.clear();
        while(query.next()){
            exList.push_back(query.value(0).toString());
        }
        emit dataChanged(this->index(0,0),this->index(rowCount()-1,columnCount()-1));
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Cancel);
    }
}
