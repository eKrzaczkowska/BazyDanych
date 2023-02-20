#include "program.h"
#include "ui_program.h"

//odznaczyc jezeli chcemy logi
//#define LOG


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

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug()<<"nazwa uzytkownika przekazana: "<< nazwaUzytkownika;
    #endif
    //**********************

    PobieranieDanych(nazwaUzytkownika, &uzytkownik);

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug()<< "id: " << uzytkownik.id << "imie: " << uzytkownik.imie << "nazwisko: " << uzytkownik.nazwisko << "haslo: " << uzytkownik.haslo <<"pracownik?: " << uzytkownik.jestPracownikiem;
    #endif
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

    QSqlDatabase::removeDatabase(baza.connectionName());
}
//~Pobranie informacji o zalogowanym uzytkowniku


//Sprawdzenie wypelnienia pol formularza haslo
void Program::btnZmien_pokaz()
{
    if(this->ui->txtHStare->text() != "" && this->ui->txtHNowe->text() != "" && this->ui->txtHPowtorzone->text() == this->ui->txtHNowe->text())
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<< "zezwalamy na wcisniecie przycisku zmien haslo";
        #endif
        //**********************

        this->ui->btnHZmien->setEnabled(1);
    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<< "nie zezwalamy na wcisniecie przycisku zmien haslo";
        #endif
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

            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug()<< "zmieniono haslo";
            #endif
            //**********************

            msgBox.setInformativeText("ZMIENIONO HASLO");

            msgBox.setStandardButtons(QMessageBox::Ok);

            msgBox.setDefaultButton(QMessageBox::Ok);

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug()<< "haslo nie zmieniono";
            #endif
            //**********************

            msgBox.setInformativeText("Bledna wprowadzenie starego hasla badz powtorzenia");

            msgBox.setStandardButtons(QMessageBox::Retry);

            msgBox.setDefaultButton(QMessageBox::Retry);

        }

        int ret = msgBox.exec();

        baza.close();

    } catch (const std::exception &e)
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
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

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        QMessageBox msgBox;

        msgBox.setWindowTitle("ERROR");

        msgBox.setInformativeText("Blad polaczenia z baza danych");

        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);

        msgBox.setDefaultButton(QMessageBox::Retry);

        int ret = msgBox.exec();
    }

    baza.close();


}

//zachowanie na klikniecia wiersza w tabeli
void Program::on_dgUzytkownicy_clicked(const QModelIndex &index)
{
    this->ui->btnPmodyfikuj->setEnabled(1);

    this->ui->btnPusun->setEnabled(1);

    //dobranie się do numeru id kliknietego wiersza i do aktualnie nacisnietej komorki
    //QString currentCell = this->ui->dgUzytkownicy->currentIndex().data(Qt::DisplayRole).toString();
    id_rekordu = this->ui->dgUzytkownicy->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell;
    qDebug() << "komorka o numerze id: " <<  id_rekordu;
    #endif
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
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Jest  pracownikiem";
        #endif
        //**********************

        this->ui->cbP->setChecked(true);

        QSqlDatabase baza;

        baza = QSqlDatabase::addDatabase("QMYSQL");

        LaczenieDoSQL(&baza);

        baza.open();

        QSqlQueryModel zapytanie;

        zapytanie.setQuery("SELECT pon_od, pon_do, wt_od, wt_do, sr_od, sr_do, cz_od, cz_do, pt_od, pt_do FROM godziny WHERE uzytkownik_id = '" + QString::number(id_rekordu) + "';"); //SELECT * to oznacza ze wszystko

        this->ui->txtponOd->setText(zapytanie.record(0).value("pon_od").toString());
        this->ui->txtponDo->setText(zapytanie.record(0).value("pon_do").toString());
        this->ui->txtwtOd->setText(zapytanie.record(0).value("wt_od").toString());
        this->ui->txtwtDo->setText(zapytanie.record(0).value("wt_do").toString());
        this->ui->txtsrOd->setText(zapytanie.record(0).value("sr_od").toString());
        this->ui->txtsrDo->setText(zapytanie.record(0).value("sr_do").toString());
        this->ui->txtczwOd->setText(zapytanie.record(0).value("cz_od").toString());
        this->ui->txtczwDo->setText(zapytanie.record(0).value("cz_do").toString());
        this->ui->txtptOd->setText(zapytanie.record(0).value("pt_od").toString());
        this->ui->txtptDo->setText(zapytanie.record(0).value("pt_do").toString());

        baza.close();

        QSqlDatabase::removeDatabase(baza.connectionName());
    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Nie jest pracownikiem";
        #endif
        //**********************

        this->ui->cbP->setChecked(false);
        this->ui->txtponOd->clear();
        this->ui->txtponDo->clear();
        this->ui->txtwtOd->clear();
        this->ui->txtwtDo->clear();
        this->ui->txtsrOd->clear();
        this->ui->txtsrDo->clear();
        this->ui->txtczwOd->clear();
        this->ui->txtczwDo->clear();
        this->ui->txtptOd->clear();
        this->ui->txtptDo->clear();

    }

    showRecords();


}

//dodanie uzytkownikow do bazy na razie jest domyslne haslo 123 (zmienic dodać okienko?)
void Program::on_btnPdodaj_clicked()
{
    QString imie = this->ui->txtPImie->text();

    QString nazwisko = this->ui->txtPNazwisko->text();

    QString login = this->ui->txtPLogin->text();

    bool pracownik = this->ui->cbP->checkState();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> imie: " << imie << "nazwisko: " << nazwisko << "nazwa: " << login << "czy jest pracownikiem?: " << pracownik  ;
    #endif
    //**********************

    {

        QSqlDatabase baza;

        baza = QSqlDatabase::addDatabase("QMYSQL");

        LaczenieDoSQL(&baza);

        baza.open();

        if(baza.transaction())
        {
            QSqlQuery query(baza);

            query.prepare("INSERT INTO `Gabinet`.`uzytkownik` (`uzytkownik_nazwa`, `imie`, `nazwisko`, `pracownik`, `haslo`) "
                          "VALUES (:uzytkownik_nazwa, :imie, :nazwisko, :pracownik, :haslo);");
            query.bindValue( ":uzytkownik_nazwa", login);
            query.bindValue( ":imie", imie );
            query.bindValue( ":nazwisko", nazwisko );
            query.bindValue( ":pracownik", pracownik );
            query.bindValue( ":haslo", login ); //domyslne haslo zawsze jak login

            if( !query.exec() )
            {
                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag";
                #endif
                //**********************

                //qFatal( "Failed to add tag" );
            }

            if(pracownik == true){

                query.prepare("INSERT INTO `Gabinet`.`godziny` (`uzytkownik_id`, `pon_od`, `pon_do`, `wt_od`, `wt_do`, `sr_od`, `sr_do`, `cz_od`, `cz_do`, `pt_od`, `pt_do`) "
                              "VALUES (last_insert_id(), :pon_od, :pon_do, :wt_od, :wt_do, :sr_od, :sr_do, :cz_od, :cz_do, :pt_od, :pt_do);");
                query.bindValue( ":pon_od",  this->ui->txtponOd->text() );
                query.bindValue( ":pon_do", this->ui->txtponDo->text() );
                query.bindValue( ":wt_od", this->ui->txtwtOd->text() );
                query.bindValue( ":wt_do", this->ui->txtwtDo->text() );
                query.bindValue( ":sr_od", this->ui->txtsrOd->text() );
                query.bindValue( ":sr_do", this->ui->txtsrDo->text() );
                query.bindValue( ":cz_od", this->ui->txtczwOd->text() );
                query.bindValue( ":cz_do", this->ui->txtczwDo->text() );
                query.bindValue( ":pt_od", this->ui->txtptOd->text() );
                query.bindValue( ":pt_do", this->ui->txtptDo->text() );

                if( !query.exec() )
                {
                    //******DEBUG_LOG*******
                    #ifdef LOG
                    qDebug() << "Failed to add tag";
                    #endif
                    //**********************

                    //qFatal( "Failed to add tag" );
                }
            }

           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction";
           #endif
           //**********************

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode";
            #endif
            //**********************
        }

        baza.close();
    }

    showRecords();

}

//DODAC USUWANIE I MODYFIKOWANIE REKORDOW:


//modyfikowacja danych uzytkownika

void Program::on_btnPmodyfikuj_clicked()
{
    QString imie = this->ui->txtPImie->text();

    QString nazwisko = this->ui->txtPNazwisko->text();

    QString login = this->ui->txtPLogin->text();

    bool pracownik = this->ui->cbP->checkState();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> imie: " << imie << "nazwisko: " << nazwisko << "nazwa: " << login << "czy jest pracownikiem?: " << pracownik  ;
    #endif
    //**********************

    {

        QSqlDatabase baza;

        baza = QSqlDatabase::addDatabase("QMYSQL");

        LaczenieDoSQL(&baza);

        baza.open();

        if(baza.transaction())
        {
            QSqlQuery query(baza);

            query.prepare("UPDATE `Gabinet`.`uzytkownik` SET `imie` = '"+ imie +"', `nazwisko` = '"+ nazwisko +"', `uzytkownik_nazwa` = '"+ login +"', `pracownik` = "
                          ""+ QString::number(pracownik) +" WHERE uzytkownik_id = '" +  QString::number(id_rekordu) + "';");

            if( !query.exec() )
            {

                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag";
                #endif
                //**********************

                //qFatal( "Failed to add tag" );
            }


           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction";
           #endif
           //**********************

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode";
            #endif
            //**********************
        }

        baza.close();
    }

    showRecords();
}

//usuniecie uzytkownika z zabezpieczeniem przed usuwaniem admina
void Program::on_btnPusun_clicked()
{
    {
        QString login = this->ui->txtPLogin->text();

        QSqlDatabase baza;

        baza = QSqlDatabase::addDatabase("QMYSQL");

        LaczenieDoSQL(&baza);

        baza.open();

        if(baza.transaction())
        {

            QMessageBox msgBox;

            QMessageBox::StandardButton reply;

            reply = msgBox.question(this, "WARRRNING", "Czy na pewno chcesz usunac tego uzytkownika", QMessageBox::Yes | QMessageBox::No);

            QSqlQuery query(baza);

            if(!(login == "admin") && reply == QMessageBox::Yes)
            {
            query.prepare("DELETE FROM `Gabinet`.`godziny` WHERE uzytkownik_id = '" +  QString::number(id_rekordu) + "';");
            query.exec();
            query.prepare("DELETE FROM `Gabinet`.`uzytkownik` WHERE uzytkownik_id = '" +  QString::number(id_rekordu) + "';");
            query.exec();
            clearRecord();
            }
            else
            {
                QMessageBox msgBox;

                msgBox.setWindowTitle("ERROR");

                msgBox.setInformativeText("Blad usuniecia uzytkownika");

                msgBox.setStandardButtons(QMessageBox::Retry);

                //msgBox.setDefaultButton(QMessageBox::Retry);
                int ret = msgBox.exec();
            }

            if( !query.exec() )
            {
                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag";
                #endif
                //**********************

                //qFatal( "Failed to add tag" );
            }

           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction";
           #endif
           //**********************

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode";
            #endif
            //**********************
        }

        baza.close();
    }

    showRecords();
}

void Program::showRecords()
{

    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uzytkownik_id, imie, nazwisko, uzytkownik_nazwa AS login, pracownik FROM uzytkownik ORDER BY nazwisko;");

    try {

        this->ui->dgUzytkownicy->setModel(queryModel);

        this->ui->dgUzytkownicy->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgUzytkownicy->hideColumn(0); //chowanie kolumny z id

        baza.close();

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        QMessageBox msgBox;

        msgBox.setWindowTitle("ERROR");

        msgBox.setInformativeText("Blad polaczenia z baza danych");

        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);

        msgBox.setDefaultButton(QMessageBox::Retry);

        int ret = msgBox.exec();
    }

    baza.close();
}

void Program::clearRecord()
{
    this->ui->txtPImie->clear();

    this->ui->txtPNazwisko->clear();

    this->ui->txtPLogin->clear();

    this->ui->cbP->setChecked(false);

    this->ui->txtponOd->clear();

    this->ui->txtponDo->clear();

    this->ui->txtwtOd->clear();

    this->ui->txtwtDo->clear();

    this->ui->txtsrOd->clear();

    this->ui->txtsrDo->clear();

    this->ui->txtczwOd->clear();

    this->ui->txtczwDo->clear();

    this->ui->txtptOd->clear();

    this->ui->txtptDo->clear();
}


void Program::on_cbP_stateChanged(int arg1)
{;
    Qt::CheckState result = this->ui->cbP->checkState();
    if(result == Qt::Checked)
    {
        this->ui->boxGodiznyPracy->setEnabled(1);
        this->ui->boxGodiznyPracy->setVisible(1);
    }
    else
    {
        this->ui->boxGodiznyPracy->setEnabled(0);
        this->ui->boxGodiznyPracy->setVisible(0);
    }
}

