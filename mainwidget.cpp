#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());
    ui->lineEditZp->setValidator(new QDoubleValidator(0,1000000000,2,this));
    ui->lineEditZpPF->setValidator(new QDoubleValidator(0,1000000000,2,this));
    ui->lineEditZp->setText("500000");
    ui->lineEditZpPF->setText("200000");

    relTypeJob = new DbRelation(QString("select n.lid, n.fnam from wire_rab_nams as n "
                                        "inner join wire_tarifs('"+QDate::currentDate().toString("yyyy-MM-dd")+"') as t on t.lid=n.lid "
                                        "inner join wire_rab_liter as l on l.id = n.id "
                                        "where l.id_ed = 1 "
                                        "order by n.fnam"),0,1,this);
    modelProd = new ModelProd(this);
    ui->tableViewProd->setModel(modelProd);

    modelNorm = new DbTableModel("econom_zp_norm",this);
    modelNorm->addColumn("id","id");
    modelNorm->addColumn("id_provol","id_provol");
    modelNorm->addColumn("id_diam","id_diam");
    modelNorm->addColumn("id_spool","id_pack");
    modelNorm->addColumn("lid",QString::fromUtf8("Вид работ"),relTypeJob);
    modelNorm->setSort("id");

    ui->tableViewNorm->setModel(modelNorm);
    ui->tableViewNorm->setColumnHidden(0,true);
    ui->tableViewNorm->setColumnHidden(1,true);
    ui->tableViewNorm->setColumnHidden(2,true);
    ui->tableViewNorm->setColumnHidden(3,true);
    ui->tableViewNorm->setColumnWidth(4,400);

    connect(ui->dateEditBeg,SIGNAL(dateChanged(QDate)),this,SLOT(updProd()));
    connect(ui->dateEditEnd,SIGNAL(dateChanged(QDate)),this,SLOT(updProd()));
    connect(ui->tableViewProd->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updNorm(QModelIndex)));
    connect(modelNorm,SIGNAL(sigUpd()),modelProd,SLOT(updState()));
    connect(ui->pushButtonCopy,SIGNAL(clicked(bool)),this,SLOT(copy()));
    connect(ui->pushButtonPaste,SIGNAL(clicked(bool)),this,SLOT(paste()));
    connect(ui->pushButtonNorm,SIGNAL(clicked(bool)),this,SLOT(repNorm()));
    connect(ui->pushButtonFact,SIGNAL(clicked(bool)),this,SLOT(repFact()));
    connect(ui->pushButtonNormPF,SIGNAL(clicked(bool)),this,SLOT(repNormPF()));
    connect(ui->pushButtonFactPF,SIGNAL(clicked(bool)),this,SLOT(repFactPF()));

    updProd();
}

MainWidget::~MainWidget()
{
    delete ui;
}

bool MainWidget::ready()
{
    bool ok=modelProd->ready();
    if (!ok){
        QMessageBox::information(this,QString::fromUtf8("Предупреждение"),QString::fromUtf8("Не для всех марок указаны виды работ!"),QMessageBox::Ok);
    }
    return ok;
}

void MainWidget::updProd()
{
    relTypeJob->refreshModel();
    modelProd->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date());
    ui->tableViewProd->setColumnHidden(0,true);
    ui->tableViewProd->setColumnHidden(1,true);
    ui->tableViewProd->setColumnHidden(2,true);
    ui->tableViewProd->setColumnWidth(3,150);
    ui->tableViewProd->setColumnWidth(4,80);
    ui->tableViewProd->setColumnWidth(5,110);
    if (ui->tableViewProd->model()->rowCount()){
        ui->tableViewProd->selectRow(0);
    }
}

void MainWidget::updNorm(QModelIndex ind)
{
    int id_provol=ui->tableViewProd->model()->data(ui->tableViewProd->model()->index(ind.row(),0),Qt::EditRole).toInt();
    int id_diam=ui->tableViewProd->model()->data(ui->tableViewProd->model()->index(ind.row(),1),Qt::EditRole).toInt();
    int id_pack=ui->tableViewProd->model()->data(ui->tableViewProd->model()->index(ind.row(),2),Qt::EditRole).toInt();
    modelNorm->setDefaultValue(1,id_provol);
    modelNorm->setDefaultValue(2,id_diam);
    modelNorm->setDefaultValue(3,id_pack);
    modelNorm->setFilter(QString("id_provol = %1 and id_diam = %2 and id_spool = %3").arg(id_provol).arg(id_diam).arg(id_pack));
    modelNorm->select();
}

void MainWidget::copy()
{
    buf.clear();
    for (int i=0; i<ui->tableViewNorm->model()->rowCount(); i++){
        QModelIndex ind=ui->tableViewNorm->model()->index(i,4);
        QVariant val=ui->tableViewNorm->model()->data(ind,Qt::EditRole);
        if (!val.isNull()){
            buf.push_back(val.toLongLong());
        }
    }
    ui->pushButtonPaste->setEnabled(buf.size());
}

void MainWidget::paste()
{
    if (buf.size()){
        int id_provol=ui->tableViewProd->model()->data(ui->tableViewProd->model()->index(ui->tableViewProd->currentIndex().row(),0),Qt::EditRole).toInt();
        int id_diam=ui->tableViewProd->model()->data(ui->tableViewProd->model()->index(ui->tableViewProd->currentIndex().row(),1),Qt::EditRole).toInt();
        int id_pack=ui->tableViewProd->model()->data(ui->tableViewProd->model()->index(ui->tableViewProd->currentIndex().row(),2),Qt::EditRole).toInt();
        QString lastError;
        foreach (qlonglong lid, buf) {
            QSqlQuery query;
            query.prepare("insert into econom_zp_norm (id_provol, id_diam, id_spool, lid) values (:id_provol, :id_diam, :id_spool, :lid)");
            query.bindValue(":id_provol",id_provol);
            query.bindValue(":id_diam",id_diam);
            query.bindValue(":id_spool",id_pack);
            query.bindValue(":lid",lid);
            if (!query.exec()){
                lastError=query.lastError().text();
            }
        }
        if (!lastError.isEmpty()){
            QMessageBox::critical(this,tr("Error"),lastError,QMessageBox::Cancel);
        }
        modelProd->updState();
        modelNorm->select();
    }
}

void MainWidget::repNorm()
{
    if (ready()){
        CubeWidget *w = new CubeWidget(31);
        w->setRange(ui->dateEditBeg->date(),ui->dateEditEnd->date(),false);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    }
}

void MainWidget::repFact()
{
    if (ready()){
        CubeWidget *w = new CubeWidget(31);
        w->setWindowTitle("Фактический расчет заработной платы на производство проволоки, руб");
        QString sum=ui->lineEditZp->text();
        sum=sum.replace(",",".");
        w->setSum(sum.toDouble());
        w->setRange(ui->dateEditBeg->date(),ui->dateEditEnd->date());
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    }
}

void MainWidget::repNormPF()
{
    if (ready()){
        CubeWidget *w = new CubeWidget(32);
        w->setRange(ui->dateEditBeg->date(),ui->dateEditEnd->date(),false);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    }
}

void MainWidget::repFactPF()
{
    if (ready()){
        CubeWidget *w = new CubeWidget(32);
        w->setWindowTitle("Фактический расчет заработной платы на производство проволоки полуфабриката, руб");
        QString sum=ui->lineEditZpPF->text();
        sum=sum.replace(",",".");
        w->setSum(sum.toDouble());
        w->setRange(ui->dateEditBeg->date(),ui->dateEditEnd->date());
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    }
}
