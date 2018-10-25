#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QInputDialog"


#define PATH_DEFAULT "/home/fred/Dropbox/Taf/Cassiopee/mashreq/files/"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    bool ok;
        QString text = QInputDialog::getText(this,"Authentification" ,"Enter password", QLineEdit::Normal,
                                             "", &ok);


        if(text!="1234cass")
            exit(0);

    ui->setupUi(this);

    QStringList params =QApplication::arguments();

    if(params.size()>1)
        PATH = params[1];
    else
        PATH=PATH_DEFAULT;


    connect(ui->save,SIGNAL(clicked(bool)),this,SLOT(writeVouchersCount()));
    connect(ui->reload,SIGNAL(clicked(bool)),this,SLOT(getVouchersCount()));
    connect(ui->bErase,SIGNAL(clicked(bool)),this,SLOT(eraseData()));
    connect(ui->bExport,SIGNAL(clicked(bool)),this,SLOT(exportData()));

    initDb();
    getVouchersCount();

    ui->eraseDateFrom->setDateTime(QDateTime::currentDateTime());
    ui->eraseDateTo->setDateTime(QDateTime::currentDateTime());
    ui->exportDateFrom->setDateTime(QDateTime::currentDateTime());
    ui->exportDateTo->setDateTime(QDateTime::currentDateTime());



}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::createTable()
{

    QSqlQuery query("CREATE TABLE players (id INTEGER PRIMARY KEY AUTOINCREMENT, firstName TEXT,lastName TEXT, phone TEXT, email TEXT, date TIMESTAMP)");

    bool success = query.exec();

    if(!success)
    {
        qDebug() << query.lastError();
    }

}

void MainWindow::initDb()
{

    const QString DRIVER("QSQLITE");
    QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);
    db.setHostName("hostname");
    db.setDatabaseName(PATH+"mashreqdb");
    db.setUserName("mashrequser");
    db.setPassword("mashreqpassword");

    if(!db.open())
        qWarning() << "ERROR: " << db.lastError();

    qDebug()<<db.tables();

    //qDebug()<<db.record("players");
    //dropTable();
    //createTable();
    createTable();
}


void MainWindow::getVouchersCount()
{


    //voucher100Total,voucher150Total,voucher200Total;
    //   uint voucher100Given,voucher150Given,voucher200Given;

    QFile file(PATH+"vouchers.cfg");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"no voucher file!";
        exit(0);

    }
    else
    {

        QTextStream in(&file);
        QStringList  lines;

        while(!in.atEnd()) {
            lines.append(in.readLine());
        }

        if(lines.size()<6)
        {
            qDebug()<<"voucher file error!";
            exit(0);
        }

        voucher100Total = lines[0].toInt();
        voucher150Total = lines[1].toInt();
        voucher200Total = lines[2].toInt();
        voucher100Given = lines[3].toInt();
        voucher150Given = lines[4].toInt();
        voucher200Given = lines[5].toInt();

        uint voucher100left = voucher100Total-voucher100Given;
        uint voucher150left = voucher150Total-voucher150Given;
        uint voucher200left = voucher200Total-voucher200Given;
        uint vouchersleft = voucher100left+voucher150left+voucher200left;
        qDebug()<<"vouchers left:"<<vouchersleft<<"("<<voucher100left<<voucher150left<<voucher200left<<")";
         file.close();
         if(vouchersleft<=0)
         {
             qDebug()<<"resetting vouchers count!";
             voucher100Given=voucher150Given=voucher200Given=0;
             writeVouchersCount();

         }


         ui->given100->setValue(voucher100Given);
         ui->given150->setValue(voucher150Given);
         ui->given200->setValue(voucher200Given);

         ui->total100->setValue(voucher100Total);
         ui->total150->setValue(voucher150Total);
         ui->total200->setValue(voucher200Total);
    }






}

void MainWindow::writeVouchersCount()
{
    voucher100Given = ui->given100->value();
    voucher150Given = ui->given150->value();
    voucher200Given = ui->given200->value();
    voucher100Total = ui->total100->value();
    voucher150Total = ui->total150->value();
    voucher200Total = ui->total200->value();


    QFile file(PATH+"vouchers.cfg");

    if(!file.open(QIODevice::WriteOnly)) {
        qDebug()<<"cannot write  to voucher file!";
        exit(0);
    }
    else
    {
        QTextStream in(&file);
        in << QString::number(voucher100Total) << endl;
        in << QString::number(voucher150Total) << endl;
        in << QString::number(voucher200Total) << endl;
        in << QString::number(voucher100Given) << endl;
        in << QString::number(voucher150Given) << endl;
        in << QString::number(voucher200Given) << endl;
    }
    file.close();
}



void MainWindow::eraseData()
{
    int ret = QMessageBox::critical(this,"Erase data","Are you sure you wan to erase this data?\nThis operation CANNOT be undone.",QMessageBox::Cancel,QMessageBox::Ok);

    switch (ret) {
    case QMessageBox::Ok:
        doEraseData(ui->eraseDateFrom->dateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->eraseDateTo->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
        break;
    case QMessageBox::Cancel:
        qDebug()<<"abort";
        break;
    default:
        // should never be reached
        break;
    }
}


void MainWindow::doEraseData(QString date1, QString date2)
{
    qDebug()<<"erase data between "<<date1<<"and"<<date2;
    QSqlQuery query;
    query.prepare("DELETE FROM players WHERE date between :DATE1 and :DATE2");
    query.bindValue(":DATE1", QVariant(date1));
    query.bindValue(":DATE2", QVariant(date2));
    query.exec();
    getPlayers();

}



void MainWindow::getPlayers()
{
    qDebug()<<"get";
    QSqlQuery query;
    query.prepare("SELECT id,name,email,score1,score2,score3,scoreTotal,date FROM players");
    query.exec();

    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString email = query.value(2).toString();
        int score1 = query.value(3).toInt();
        int score2 = query.value(4).toInt();
        int score3 = query.value(5).toInt();
        int scoreTotal = query.value(6).toInt();
        QDateTime date = query.value(7).toDateTime();
        qDebug()<<id<<name<<email<<score1<<score2<<score3<<scoreTotal<<date;
    }
}

void MainWindow::exportData()
{
    QString date1 = ui->exportDateFrom->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString date2 = ui->exportDateTo->dateTime().toString("yyyy-MM-dd hh:mm:ss");

    QSqlQuery query;





    qDebug()<<"Export data between: "<<date1<<"and"<<date2;


    query.prepare("SELECT id,firstName,lastName,phone,email,date FROM players WHERE date between :DATE1 and :DATE2");
    query.bindValue(":DATE1", QVariant(date1));
    query.bindValue(":DATE2", QVariant(date2));
    query.exec();



    QString filename=PATH+"data.csv";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );


        stream <<"user id"<<","<<"first name"<<","<<"last name"<<","<<"phone number"<<","<<"email address"<<","<<"registration date"<<endl;
        while (query.next()) {
            int id = query.value(0).toInt();
            QString firstname = query.value(1).toString();
            QString lastname =  query.value(2).toString();
            QString phone = query.value(3).toString();
            QString email = query.value(4).toString();
            QDateTime date = query.value(5).toDateTime();

            qDebug()<<id<<firstname<<lastname<<phone<<email<<date.toString("yyyy-MM-dd hh:mm:ss");
            stream <<id<<","<<firstname<<","<<lastname<<","<<phone<<","<<email<<","<<date.toString("yyyy-MM-dd hh:mm:ss")<<endl;
        }
    }
}


