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

void Program::LaczenieDoSQL(QSqlDatabase *baza)
{
    //connecting to mysql

    baza->setDatabaseName("Gabinet"); // ustawiam nazwę biblioteki, pod którą chcę się podpiąć

    baza->setHostName("127.0.0.1"); // nazwa serwera

    baza->setPassword("password"); // hasło

    baza->setPort(3306); // port połączenia z bazą danych

    baza->setUserName("root"); // nazwa użytkownika
}

Program::Program(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Program)
{
    ui->setupUi(this);
}

QString nazwaUzytkownika;

Program::Program(QString nazwaUzytkownikaLog) :
    ui(new Ui::Program)
{
    ui->setupUi(this);

    //tworzeni instancji uzytkownika

    Uzytkownik uzytkownik;

    //sciaganie z bazy informacji o przekazanym rekordzie
    //przekazana nazwa uzytkownika

    nazwaUzytkownika = nazwaUzytkownikaLog;

    //**********************
    //******DEBUG_LOG*******
    //qDebug()<<"nazwa uzytkownika przekazana: "<< nazwaUzytkownika;
    //**********************

    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

    PobieranieDanych(nazwaUzytkownika, &uzytkownik);

    baza.close();

    //**********************
    //******DEBUG_LOG*******
    qDebug()<< "id: " << uzytkownik.id << "imie: " << uzytkownik.imie << "nazwisko: " << uzytkownik.nazwisko << "haslo: " << uzytkownik.haslo <<"pracownik?: " << uzytkownik.jestPracownikiem;
    //**********************

}

Program::~Program()
{
    delete ui;
}

//Pobranie informacji o zalogowanym uzytkowniku
void Program::PobieranieDanych(QString nazwaUzytkownikaLog, struct Uzytkownik *uzytkownik)
{

    QSqlQueryModel zapytanie;

    zapytanie.setQuery("SELECT uzytkownik_id, imie, nazwisko, haslo, pracownik FROM uzytkownik WHERE uzytkownik_nazwa = '" + nazwaUzytkownikaLog + "';");

    uzytkownik->id = zapytanie.record(0).value("uzytkownik_id").toInt();

    uzytkownik->imie = zapytanie.record(0).value("imie").toString();

    uzytkownik->nazwisko = zapytanie.record(0).value("nazwisko").toString();

    uzytkownik->haslo = zapytanie.record(0).value("haslo").toString();

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
//~Pobranie informacji o zalogowanym uzytkowniku


//Sprawdzenie wypelnienia pol formularza haslo
void Program::btnZmien_pokaz()
{
    if(this->ui->txtHStare->text() != "" && this->ui->txtHNowe->text() != "" && this->ui->txtHPowtorzone->text() == this->ui->txtHNowe->text())
    {
        //**********************
        //******DEBUG_LOG*******
        //qDebug()<< "zezwalamy na wcisniecie przycisku zmien haslo";
        //**********************
        this->ui->btnHZmien->setEnabled(1);
    }
    else
    {
        //**********************
        //******DEBUG_LOG*******
        //qDebug()<< "nie zezwalamy na wcisniecie przycisku zmien haslo";
        //**********************
        this->ui->btnHZmien->setEnabled(0);
    }
}

void Program::on_txtHStare_textChanged(const QString &arg1)
{
    btnZmien_pokaz();
}


void Program::on_txtHNowe_textChanged(const QString &arg1)
{
    btnZmien_pokaz();
}


void Program::on_txtHPowtorzone_textChanged(const QString &arg1)
{
    btnZmien_pokaz();
}
//~Sprawdzenie wypelnienia pol formularza haslo

//Gdy klikamy przycisk zmien: zmiana Hasla
void Program::on_btnHZmien_clicked()
{
    bool czyTakieSame;

    //QSqlDatabase baza;

    //baza = QSqlDatabase::addDatabase("QMYSQL");

    //LaczenieDoSQL(&baza);

   // baza.open();


    QSqlQueryModel zapytanie;

    //jak wpisze to samo wszedzie to nie dziala

    zapytanie.setQuery("SELECT haslo FROM uzytkownik WHERE uzytkownik_nazwa = '" + nazwaUzytkownika + "';");

    QString haslo = zapytanie.record(0).value("haslo").toString();

    if(this->ui->txtHStare->text() == haslo)
    {
        zapytanie.setQuery("UPDATE uzytkownik SET haslo = '"+ this->ui->txtHNowe->text() +"' WHERE uzytkownik_nazwa = '" + nazwaUzytkownika + "' AND haslo = '"+ haslo +"'");
    }


   /* if(wykonaloSie == true)
    {
        QMessageBox msgBox;

        msgBox.setWindowTitle("WARNING");

        msgBox.setInformativeText("Zmieniono haslo");

        msgBox.setStandardButtons(QMessageBox::Ok);

        msgBox.setDefaultButton(QMessageBox::Ok);

        int ret = msgBox.exec();
    }
    else
    {
        QMessageBox msgBox;

        msgBox.setWindowTitle("WARNING");

        msgBox.setInformativeText("Bledna wprowadzenie starego hasla badz powtorzenia");

        msgBox.setStandardButtons(QMessageBox::Retry);

        msgBox.setDefaultButton(QMessageBox::Retry);

        int ret = msgBox.exec();

    }*/
}


/*
 * zapytanie.prepare("INSERT INTO Uzytkownicy(Id, Imie, Nazwisko, Login) ""VALUES(:id, :imie, :nazwisko, :login)");
 * zapytanie.bindValue(:id,10);
 * zapytanie.bindValue(:imie,"Jan");
 * zapytanie.bindValue(:nazwisko,"Kowalski");
 * zapytanie.bindValue(:login,"janek");
 * zapytanie.exec();}
 * */




