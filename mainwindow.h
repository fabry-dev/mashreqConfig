#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qfile.h"
#include "qdebug.h"
#include <QtSql>
#include <QSqlQuery>
#include "qmessagebox.h"


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
    uint voucher100Total,voucher150Total,voucher200Total,voucher100Given,voucher150Given,voucher200Given;
    QString PATH;
    void createTable();
    void initDb();
    void doEraseData(QString date1, QString date2);
    void getPlayers();

private slots:
    void getVouchersCount();
    void writeVouchersCount();
    void eraseData();
    void exportData();


};

#endif // MAINWINDOW_H
