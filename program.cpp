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

//--------------------------------------------------UZYTKOWNICY---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------

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
    UzytkownicySzukaj();
}

void Program::UzytkownicySzukaj()
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
            else
            {
                QMessageBox msgBox;

                msgBox.setWindowTitle("WARNING");

                msgBox.setInformativeText("dodano uzytkownika");

                msgBox.setStandardButtons(QMessageBox::Ok);

                //msgBox.setDefaultButton(QMessageBox::Retry);
                int ret = msgBox.exec();
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
                else
                {
                    QMessageBox msgBox;

                    msgBox.setWindowTitle("WARNING");

                    msgBox.setInformativeText("dodano sekcje godizny");

                    msgBox.setStandardButtons(QMessageBox::Ok);

                    //msgBox.setDefaultButton(QMessageBox::Retry);
                    int ret = msgBox.exec();
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
            else
            {
                QMessageBox msgBox;

                msgBox.setWindowTitle("WARNING");

                msgBox.setInformativeText("zmodyfikowane dane uytkownika");

                msgBox.setStandardButtons(QMessageBox::Ok);

                //msgBox.setDefaultButton(QMessageBox::Retry);
                int ret = msgBox.exec();
            }

            QSqlQuery zapytanie;

            zapytanie.prepare("SELECT * FROM godziny WHERE uzytkownik_id = '" + QString::number(id_rekordu) + "';"); //SELECT * to oznacza ze wszystko
            zapytanie.exec();

            if((zapytanie.size() != 0) && (pracownik== true) )
            {
                qDebug() << "jest rekord";

                QString ponP =  this->ui->txtponOd->text();
                QString ponK = this->ui->txtponDo->text();
                QString wtP = this->ui->txtwtOd->text();
                QString wtK = this->ui->txtwtDo->text();
                QString srP = this->ui->txtsrOd->text();
                QString srK = this->ui->txtsrDo->text();
                QString czP = this->ui->txtczwOd->text();
                QString czK = this->ui->txtczwDo->text();
                QString ptP = this->ui->txtptOd->text();
                QString ptK = this->ui->txtptDo->text();

                query.prepare("UPDATE `Gabinet`.`godziny` SET `pon_od` = '"+ ponP +"', `pon_do` = '"+ ponK +"', `wt_od` = '"+ wtP +"', `wt_do` = '"+ wtK +"', `sr_od` = '"+ srP +"', `sr_do` = '"+ srK +"'"
                              ", `cz_od` = '"+ czP +"', `cz_do` = '"+ czK +"', `pt_od` = '"+ ptP +"', `pt_do` = '"+ ptK +"' WHERE uzytkownik_id = '" + QString::number(id_rekordu) + "';");

                if( !query.exec() )
                {
                    //******DEBUG_LOG*******
                    #ifdef LOG
                    qDebug() << "Failed to add tag";
                    #endif
                    //**********************

                    //qFatal( "Failed to add tag" );
                }
                else
                {
                    QMessageBox msgBox;

                    msgBox.setWindowTitle("WARNING");

                    msgBox.setInformativeText("zmodyfikowane dane uytkownika w sekcji godziny");

                    msgBox.setStandardButtons(QMessageBox::Ok);

                    //msgBox.setDefaultButton(QMessageBox::Retry);
                    int ret = msgBox.exec();
                }
            }
            else if(pracownik== true)
            {
                query.prepare("INSERT INTO `Gabinet`.`godziny` (`uzytkownik_id`, `pon_od`, `pon_do`, `wt_od`, `wt_do`, `sr_od`, `sr_do`, `cz_od`, `cz_do`, `pt_od`, `pt_do`) "
                              "VALUES ("+ QString::number(id_rekordu)  +", :pon_od, :pon_do, :wt_od, :wt_do, :sr_od, :sr_do, :cz_od, :cz_do, :pt_od, :pt_do);");
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
                else
                {
                    QMessageBox msgBox;

                    msgBox.setWindowTitle("WARNING");

                    msgBox.setInformativeText("dodano sekcje godizny do uzytkownika");

                    msgBox.setStandardButtons(QMessageBox::Ok);

                    //msgBox.setDefaultButton(QMessageBox::Retry);
                    int ret = msgBox.exec();
                }
                qDebug() << " nie ma rekordu";
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

            QMessageBox msgBox;

            msgBox.setWindowTitle("WARNING");

            msgBox.setInformativeText("usunieto uzytkownika");

            msgBox.setStandardButtons(QMessageBox::Ok);

            //msgBox.setDefaultButton(QMessageBox::Retry);
            int ret = msgBox.exec();
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

    this->ui->txtUcena->clear();

    this->ui->txtUnazwa->clear();

    this->ui->txtUczas->clear();

    this->ui->txtUopis->clear();
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

//QVector<QString> czasStart = {this->ui->pon, "txtwtOd", "txtsrOd", "txtczwOd", "txtptOd"};


//czy trzeba do trzech po :
void Program::on_btn7_15_clicked()
{
   czasPracy("07:00:00", "15:00:00");
}

void Program::on_btn8_16_clicked()
{
    czasPracy("08:00:00", "16:00:00");
}


void Program::on_btn9_17_clicked()
{
    czasPracy("09:00:00", "17:00:00");
}


void Program::on_btn10_18_clicked()
{
    czasPracy("10:00:00", "18:00:00");
}

void Program::czasPracy(QString poczatek, QString koniec)
{
    QVector<QLineEdit*> czasStart {this->ui->txtponOd, this->ui->txtwtOd, this->ui->txtsrOd, this->ui->txtczwOd, this->ui->txtptOd};

    QVector<QLineEdit*> czasKoniec {this->ui->txtponDo, this->ui->txtwtDo, this->ui->txtsrDo, this->ui->txtczwDo, this->ui->txtptDo};


   for(int i = 0; i < czasStart.size(); i++)
    {
        czasStart[i]->setText(poczatek);
    }

   for(int i = 0; i < czasKoniec.size(); i++)
    {
        czasKoniec[i]->setText(koniec);
    }


}

//---------------------------------------------------------------------USLUGI----------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------


void Program::on_btnUSzukaj_clicked()
{
    showUslugi();
}

void Program::showUslugi()
{
    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT * FROM uslugi WHERE nazwa LIKE '%"+ this->ui->txtUzabieg->text() +"%' ORDER BY nazwa;");

    try {

        this->ui->dgZabiegi->setModel(queryModel);

        this->ui->dgZabiegi->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgZabiegi->hideColumn(0); //chowanie kolumny z id

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

void Program::on_btnUDodaj_clicked()
{
    QString nazwa = this->ui->txtUnazwa->text();

    QString cena = this->ui->txtUcena->text().replace(",","."); //przecinek nie jest uznawany przez typ DECIMAL(6,2) -> 6 cyfr z czego 2 po przecinku

    QString czas = this->ui->txtUczas->text();

    QString opis= this->ui->txtUopis->toPlainText();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> nazwa: " << nazwa << "cena: " << cena << "czas: " << czas << "opis: " << opis  ;
    #endif
    //**********************

    {

        QSqlDatabase baza;

        baza = QSqlDatabase::addDatabase("QMYSQL");

        LaczenieDoSQL(&baza);

        baza.open();

        if(baza.transaction())
        {
            if(nazwa.length()>=3 && cena.length()>=1 && czas.length() >=5)
            {
                QSqlQuery query(baza);

                query.prepare("INSERT INTO `Gabinet`.`uslugi` (`nazwa`, `cena`, `czas`, `opis`) "
                              "VALUES (:nazwa, :cena, :czas, :opis);");
                query.bindValue( ":nazwa", nazwa);
                query.bindValue( ":cena", cena );
                query.bindValue( ":czas", czas );
                query.bindValue( ":opis", opis );

                if( !query.exec() )
                {
                    //******DEBUG_LOG*******
                    #ifdef LOG
                    qDebug() << "Failed to add tag";
                    #endif
                    //**********************

                    //qFatal( "Failed to add tag" );
                }
                else
                {
                    QMessageBox msgBox;

                    msgBox.setWindowTitle("WARNING");

                    msgBox.setInformativeText("dodano uzytkownika");

                    msgBox.setStandardButtons(QMessageBox::Ok);

                    //msgBox.setDefaultButton(QMessageBox::Retry);
                    int ret = msgBox.exec();
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

    showUslugi();
}


void Program::on_btnUUsun_clicked()
{
   // QString login = this->ui->txtPLogin->text();

    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

    if(baza.transaction())
    {

        QMessageBox msgBox;

        QMessageBox::StandardButton reply;

        reply = msgBox.question(this, "WARRRNING", "Czy na pewno chcesz usunac ta usluge", QMessageBox::Yes | QMessageBox::No);

        QSqlQuery query(baza);

        if(reply == QMessageBox::Yes)
        {
            query.prepare("DELETE FROM `Gabinet`.`uslugi` WHERE uslugi_id = '" +  QString::number(id_rekordu) + "';");
            query.exec();
            clearRecord();

            QMessageBox msgBox;

            msgBox.setWindowTitle("WARNING");

            msgBox.setInformativeText("usunieto usluge");

            msgBox.setStandardButtons(QMessageBox::Ok);

            //msgBox.setDefaultButton(QMessageBox::Retry);
            int ret = msgBox.exec();
        }
        else
        {
            QMessageBox msgBox;

            msgBox.setWindowTitle("ERROR");

            msgBox.setInformativeText("Blad usuniecia uslugi");

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

    showUslugi();
}


void Program::on_btnUModyfikuj_clicked()
{
    QString nazwa = this->ui->txtUnazwa->text();

    QString cena = this->ui->txtUcena->text().replace(",","."); //przecinek nie jest uznawany przez typ DECIMAL(6,2) -> 6 cyfr z czego 2 po przecinku

    QString czas = this->ui->txtUczas->text();

    QString opis= this->ui->txtUopis->toPlainText();
    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> nazwa: " << nazwa << "cena: " << cena << "czas: " << czas << "opis: " << opis  ;
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

            query.prepare("UPDATE `Gabinet`.`uslugi` SET `nazwa` = '"+ nazwa +"', `cena` = '"+ cena +"', `czas` = '"+ czas +"', `opis` = "
                          "'"+ opis +"' WHERE (`uslugi_id` = '"+ QString::number(id_rekordu) +"');");

            if( !query.exec() )
            {

                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag";
                #endif
                //**********************

                //qFatal( "Failed to add tag" );
            }
            else
            {
                QMessageBox msgBox;

                msgBox.setWindowTitle("WARNING");

                msgBox.setInformativeText("zmodyfikowane dane uslugi");

                msgBox.setStandardButtons(QMessageBox::Ok);

                //msgBox.setDefaultButton(QMessageBox::Retry);
                int ret = msgBox.exec();
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

    showUslugi();
}


void Program::on_dgZabiegi_clicked(const QModelIndex &index)
{
    this->ui->btnUModyfikuj->setEnabled(1);

    this->ui->btnUUsun->setEnabled(1);

    //dobranie się do numeru id kliknietego wiersza i do aktualnie nacisnietej komorki
    //QString currentCell = this->ui->dgUzytkownicy->currentIndex().data(Qt::DisplayRole).toString();
    id_rekordu = this->ui->dgZabiegi->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell;
    qDebug() << "komorka o numerze id: " <<  id_rekordu;
    #endif
    //**********************

    QString tabNazwa = this->ui->dgZabiegi->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();

    QString tabCena = this->ui->dgZabiegi->selectionModel()->selectedIndexes().at(2).data(Qt::DisplayRole).toString();

    QString tabCzas = this->ui->dgZabiegi->selectionModel()->selectedIndexes().at(3).data(Qt::DisplayRole).toString();

    QString tabOpis = this->ui->dgZabiegi->selectionModel()->selectedIndexes().at(4).data(Qt::DisplayRole).toString();

    this->ui->txtUnazwa->setText(tabNazwa);

    this->ui->txtUcena->setText(tabCena);

    this->ui->txtUczas->setText(tabCzas);

    this->ui->txtUopis->setPlainText(tabOpis);

    showUslugi();

}

//usuniecie w usluach dziala i modyfikacja gdy juz nie ma uslug trzeba na to sie  uodpornic i optymalizacja kodu jezeli powtarzajacy sie kod np. wyszukiwanie i wrzucanie do siatki

void Program::on_btnPUSzukaj_clicked()
{
    pokaszPracownikow();
}

void Program::pokaszPracownikow()
{
    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uzytkownik_id, imie, nazwisko, uzytkownik_nazwa AS login, pracownik FROM uzytkownik WHERE CONCAT(imie, ' ', nazwisko, uzytkownik_nazwa) LIKE '%"+ this->ui->txtPSzukaj->text() +"%' AND pracownik = '1' ORDER BY nazwisko;");

    try {

        this->ui->dgPUPracownicy->setModel(queryModel);

        this->ui->dgPUPracownicy->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgPUPracownicy->hideColumn(0); //chowanie kolumny z id

        this->ui->dgPUPracownicy->

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



void Program::on_dgPUPracownicy_clicked(const QModelIndex &index)
{
    //wyszukiwanie uslugi ktore wykonuje pracownik
    //dobranie się do numeru id kliknietego wiersza i do aktualnie nacisnietej komorki
    //QString currentCell = this->ui->dgUzytkownicy->currentIndex().data(Qt::DisplayRole).toString();
    id_rekordu = this->ui->dgPUPracownicy->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell;
    qDebug() << "komorka o numerze id: " <<  id_rekordu;
    #endif
    //**********************

    QString tabNazwa = this->ui->dgPUPracownicy->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();

    QString tabNazwisko = this->ui->dgPUPracownicy->selectionModel()->selectedIndexes().at(2).data(Qt::DisplayRole).toString();

    this->ui->txtPUImie->setText(tabNazwa);

    this->ui->txtPUNazwisko->setText(tabNazwisko);

    //Wypelnianie uslug pracownika

    PokazUslugi();

    //pokaszPracownikow();

}

void Program::PokazUslugi()
{
    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    LaczenieDoSQL(&baza);

    baza.open();

    QSqlTableModel *model = new QSqlTableModel;

    model->setTable("uslugi");
    model->select();

    try {

        this->ui->dgPUDodajUsluge->setModel(model);

        this->ui->dgPUDodajUsluge->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgPUDodajUsluge->hideColumn(0); //chowanie kolumny z id

        this->ui->dgPUDodajUsluge->show();

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
/* queryModel = new QSqlQueryModel(this);

 queryModel->setQuery("SELECT uslugi.uslugi_id, uslugi.nazwa, uslugi.cena, uslugi.czas FROM uslugi, uzytkownik_usluga WHERE "
                      "uslugi.uslugi_id = uzytkownik_usluga.uslugi_id AND uzytkownik_usluga.uzytkownik_id = "+ QString::number(id_rekordu) +"ORDER BY nazwa;");

 try {

     this->ui->dgPUUslugi->setModel(queryModel);

     this->ui->dgPUUslugi->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

     //this->ui->dgPUUslugi->hideColumn(0); //chowanie kolumny z id

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

 //Wypelnianie wszytskich uslug*/

