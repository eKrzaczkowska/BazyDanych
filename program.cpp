#include "program.h"
#include "ui_program.h"

//reszta bibliotek
#include <string>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQueryModel>

#include <QMessageBox>

Program::Program(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Program)
{
    ui->setupUi(this);
}

Program::Program(QString nazwaUzytkownikaLog) :
    ui(new Ui::Program)
{
    ui->setupUi(this);

    //tworzeni instancji uzytkownika

    Uzytkownik uzytkownik;

    //connecting to mysql
    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    baza.setDatabaseName("Gabinet"); // ustawiam nazwę biblioteki, pod którą chcę się podpiąć

    baza.setHostName("127.0.0.1"); // nazwa serwera

    baza.setPassword("password"); // hasło

    baza.setPort(3306); // port połączenia z bazą danych

    baza.setUserName("root"); // nazwa użytkownika


    //sciaganie z bazy informacji o przekazanym rekordzie
    //przekazana nazwa uzytkownika

    QString nazwaUzytkownika = nazwaUzytkownikaLog;

    //**********************
    //******DEBUG_LOG*******
    qDebug()<<"nazwa uzytkownika przekazana: "<< nazwaUzytkownika;
    //**********************

    baza.open();

    PobieranieDanych(nazwaUzytkownika, &uzytkownik);

    //**********************
    //******DEBUG_LOG*******
    //qDebug()<< "id: " << uzytkownik.id << "imie: " << uzytkownik.imie << "nazwisko: " << uzytkownik.nazwisko << "pracownik?: " << uzytkownik.jestPracownikiem;
    //**********************

}

Program::~Program()
{
    delete ui;
}

void Program::PobieranieDanych(QString nazwaUzytkownikaLog, struct Uzytkownik *uzytkownik)
{

    QSqlQueryModel zapytanie;

    zapytanie.setQuery("SELECT uzytkownik_id, imie, nazwisko, pracownik FROM uzytkownik WHERE uzytkownik_nazwa = '" + nazwaUzytkownikaLog + "';");

    uzytkownik->id = zapytanie.record(0).value("uzytkownik_id").toInt();

    uzytkownik->imie = zapytanie.record(0).value("imie").toString();

    uzytkownik->nazwisko = zapytanie.record(0).value("nazwisko").toString();

    int pracownik = zapytanie.record(0).value("pracownik").toInt();

    if(pracownik == 0)
    {
        uzytkownik->jestPracownikiem = false;
    }
    else
    {
        uzytkownik->jestPracownikiem = true;
    }
}




/*
 * zapytanie.prepare("INSERT INTO Uzytkownicy(Id, Imie, Nazwisko, Login) ""VALUES(:id, :imie, :nazwisko, :login)");
 * zapytanie.bindValue(:id,10);
 * zapytanie.bindValue(:imie,"Jan");
 * zapytanie.bindValue(:nazwisko,"Kowalski");
 * zapytanie.bindValue(:login,"janek");
 * zapytanie.exec();}
 * */
