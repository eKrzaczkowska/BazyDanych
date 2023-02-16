#include "program.h"
#include "ui_program.h"


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
int id_rekordu;

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

    PobieranieDanych(nazwaUzytkownika, &uzytkownik);

    //**********************
    //******DEBUG_LOG*******
    //qDebug()<< "id: " << uzytkownik.id << "imie: " << uzytkownik.imie << "nazwisko: " << uzytkownik.nazwisko << "haslo: " << uzytkownik.haslo <<"pracownik?: " << uzytkownik.jestPracownikiem;
    //**********************

}

Program::~Program()
{
    delete ui;
}

//Pobranie informacji o zalogowanym uzytkowniku
void Program::PobieranieDanych(QString nazwaUzytkownikaLog, struct Uzytkownik *uzytkownik)
{
    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

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


    baza.close();
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

    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();


    QSqlQueryModel zapytanie;

    zapytanie.setQuery("SELECT haslo FROM uzytkownik WHERE uzytkownik_nazwa = '" + nazwaUzytkownika + "';");

    QString haslo = zapytanie.record(0).value("haslo").toString();

    try {

        QMessageBox msgBox;

        msgBox.setWindowTitle("WARNING");

        if(this->ui->txtHStare->text() == haslo)
        {
            zapytanie.setQuery("UPDATE uzytkownik SET haslo = '"+ this->ui->txtHNowe->text() +"' WHERE uzytkownik_nazwa = '" + nazwaUzytkownika + "' AND haslo = '"+ haslo +"'");
            //**********************
            //******DEBUG_LOG*******
            qDebug()<< "zmieniono haslo";
            //**********************

            msgBox.setInformativeText("ZMIENIONO HASLO");

            msgBox.setStandardButtons(QMessageBox::Ok);

            msgBox.setDefaultButton(QMessageBox::Ok);

        }
        else
        {
            //**********************
            //******DEBUG_LOG*******
            qDebug()<< "haslo nie zmieniono";
            //**********************

            msgBox.setInformativeText("Bledna wprowadzenie starego hasla badz powtorzenia");

            msgBox.setStandardButtons(QMessageBox::Retry);

            msgBox.setDefaultButton(QMessageBox::Retry);

        }

        int ret = msgBox.exec();

        baza.close();

    } catch (const std::exception &e)
    {

        //**********************
        //******DEBUG_LOG*******
        //qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        //qDebug() << "ERROR load: " << baza.lastError().text();
        //**********************

        QMessageBox msgBox;

        msgBox.setWindowTitle("ERROR");

        msgBox.setInformativeText("Blad polaczenia z baza danych");

        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);

        msgBox.setDefaultButton(QMessageBox::Retry);

        int ret = msgBox.exec();
    }

}

//Pobieranie rekordów z SQL do tablicy wynikow
void Program::on_btnPSzukaj_clicked()
{


    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uzytkownik_id, imie, nazwisko, uzytkownik_nazwa AS login, pracownik FROM uzytkownik WHERE CONCAT(imie, ' ', nazwisko, uzytkownik_nazwa) LIKE '%"+ this->ui->txtPSzukaj->text() +"%' ORDER BY nazwisko;");

    try {

        this->ui->dgUzytkownicy->setModel(queryModel);

        this->ui->dgUzytkownicy->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgUzytkownicy->hideColumn(0); //chowanie kolumny z id

        baza.close();

    } catch (const std::exception &e)
    {

        //**********************
        //******DEBUG_LOG*******
        //qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        //qDebug() << "ERROR load: " << baza.lastError().text();
        //**********************

        QMessageBox msgBox;

        msgBox.setWindowTitle("ERROR");

        msgBox.setInformativeText("Blad polaczenia z baza danych");

        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);

        msgBox.setDefaultButton(QMessageBox::Retry);

        int ret = msgBox.exec();
    }

}

//zachowanie na klikniecia wiersza w tabeli
void Program::on_dgUzytkownicy_clicked(const QModelIndex &index)
{

    //dobranie się do numeru id kliknietego wiersza i do aktualnie nacisnietej komorki
    //QString currentCell = this->ui->dgUzytkownicy->currentIndex().data(Qt::DisplayRole).toString();
    id_rekordu = this->ui->dgUzytkownicy->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //**********************
    //******DEBUG_LOG*******
    //qDebug() << "nacisnieta komorka: " << currentCell;
    //qDebug() << "komorka o numerze id: " <<  id_rekordu;
    //**********************

    QString tabName = this->ui->dgUzytkownicy->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();
    QString tabSubName = this->ui->dgUzytkownicy->selectionModel()->selectedIndexes().at(2).data(Qt::DisplayRole).toString();
    QString tabLog = this->ui->dgUzytkownicy->selectionModel()->selectedIndexes().at(3).data(Qt::DisplayRole).toString();
    int ifWorker = this->ui->dgUzytkownicy->selectionModel()->selectedIndexes().at(4).data(Qt::DisplayRole).toInt();

    this->ui->txtPImie->setText(tabName);
    this->ui->txtPNazwisko->setText(tabSubName);
    this->ui->txtPLogin->setText(tabLog);
    if(ifWorker == 1)
    {
        //**********************
        //******DEBUG_LOG*******
        //qDebug() << "Jest  pracownikiem";
        //**********************
        this->ui->cbP->setChecked(true);
    }
    else
    {
        //**********************
        //******DEBUG_LOG*******
        //qDebug() << "Nie jest pracownikiem";
        //**********************
        this->ui->cbP->setChecked(false);
    }

   // baza.close();
}



/*
 * zapytanie.prepare("INSERT INTO Uzytkownicy(Id, Imie, Nazwisko, Login) ""VALUES(:id, :imie, :nazwisko, :login)");
 * zapytanie.bindValue(:id,10);
 * zapytanie.bindValue(:imie,"Jan");
 * zapytanie.bindValue(:nazwisko,"Kowalski");
 * zapytanie.bindValue(:login,"janek");
 * zapytanie.exec();}
 * */







