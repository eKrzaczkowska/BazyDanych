#include "logowanie.h"
#include "./ui_loginwindow.h"

#include "program.h"
#include "./ui_program.h"


#include <QDebug>

logInWindow::logInWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::logInWindow) 
{
    ui->setupUi(this);
}

logInWindow::~logInWindow()
{
    delete ui;
}



void logInWindow::on_btnZaloguj_clicked()
{
    this->hide();

    Program * mainWindow = new Program();

    mainWindow->show();

    this->close();
}

