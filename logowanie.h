#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

//pliki z graficznym interfejsem
#include "./ui_loginwindow.h"
#include "./ui_program.h"

//reszta bibliotek
#include <string>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QMessageBox>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class logInWindow; }
QT_END_NAMESPACE

class logInWindow : public QMainWindow
{
    Q_OBJECT

public:
    logInWindow(QWidget *parent = nullptr);
    ~logInWindow();

private slots:
    void on_btnZaloguj_clicked();

private:
    Ui::logInWindow *ui;
};
#endif // LOGINWINDOW_H
