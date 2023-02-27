#include "program.h"
#include "ui_program.h"

//odkomentowac jezeli chcemy logi
//#define LOG

//zmienne globalne

QString nazwaUzytkownika;

int id_rekordu;

int id_uslugiDoDodania;

int id_uslugiDoUsuniecia;

//---------------------------------------GLOWNE OKNO----------------------------------------------------

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

    //LACZENIE DO SQL

    QSqlDatabase baza = QSqlDatabase::addDatabase("QMYSQL");

    baza.setDatabaseName("Gabinet"); // ustawiam nazwę biblioteki, pod którą chcę się podpiąć

    baza.setHostName("127.0.0.1"); // nazwa serwera

    baza.setPassword("password"); // hasło

    baza.setPort(3306); // port połączenia z bazą danych

    baza.setUserName("root"); // nazwa użytkownika

    baza.open();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: main  :: program.cpp";
    }
    #endif
    //**********************

    //przekazana nazwa uzytkownika

    nazwaUzytkownika = nazwaUzytkownikaLog;

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug()<<"nazwa uzytkownika przekazana: "<< nazwaUzytkownika << " :: program.cpp";
    #endif
    //**********************

    //ustawianie co moga zwierac pola tekstowe w okienku klient

    this->ui->txtKnumer->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9][0-9]{4}")));
    this->ui->txtKtelefon->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9][0-9]{8}")));
    this->ui->txtKImie->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Z][a-z]*")));
    this->ui->txtKNazwisko->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Z][a-z]*")));
    this->ui->txtKmiejscowosc->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z]*")));
    this->ui->txtKpoczta->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{2}-[0-9]{3}")));
    this->ui->txtKulica->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z]*")));




            //QRegExpValidator(QRegExp("[0-9]*"), &le));


}

Program::~Program()
{
    QSqlDatabase baza = QSqlDatabase::database();

    if(baza.open())
    {

        baza.close();
    }

    QSqlDatabase::removeDatabase(baza.connectionName());

    //delete [] buttons;

    delete ui;
}

//--------------------------------------------------UZYTKOWNICY---------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------

//Sprawdzenie wypelnienia pol formularza haslo -> gdy podane 3 wartości i hasło powtorzone takie samo to zezwalamy na wcisniecie przycisku
void Program::btnZmien_pokaz()
{

    if(this->ui->txtHStare->text() != "" && this->ui->txtHNowe->text() != "" && this->ui->txtHPowtorzone->text() == this->ui->txtHNowe->text())
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<< "zezwalamy na wcisniecie przycisku zmien haslo :: program.cpp";
        #endif
        //**********************

        this->ui->btnHZmien->setEnabled(1);
    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<< "nie zezwalamy na wcisniecie przycisku zmien haslo :: program.cpp";
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

    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    QSqlQueryModel zapytanie;

    zapytanie.setQuery("SELECT haslo FROM uzytkownik WHERE uzytkownik_nazwa = '" + nazwaUzytkownika + "';");

    QString haslo = zapytanie.record(0).value("haslo").toString();

    try {

        if(this->ui->txtHStare->text() == haslo)
        {
            zapytanie.setQuery("UPDATE uzytkownik SET haslo = '"+ this->ui->txtHNowe->text() +"' "
                               "WHERE uzytkownik_nazwa = '" + nazwaUzytkownika + "' AND haslo = '"+ haslo +"'");

            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug()<< "zmieniono haslo :: program.cpp";
            #endif
            //**********************

            msgOK("WARNING", "ZMIENIONO HASŁO");

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug()<< "haslo nie zmieniono :: program.cpp";
            #endif
            //**********************

            msgRetry("WARNING", "BLEDNE WPROWADZENIE STAREGO HASLA ALBO NIEPOPRAWNE POWTORZENIE");

        }


    } catch (const std::exception &e)
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }

}

//Pobieranie rekordów z SQL do tablicy wynikow -> uzytkownicy
void Program::on_btnPSzukaj_clicked()
{
    UzytkownicySzukaj();
}

void Program::UzytkownicySzukaj()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uzytkownik_id, imie, nazwisko, uzytkownik_nazwa AS login, pracownik "
                         "FROM uzytkownik WHERE CONCAT(imie, ' ', nazwisko, uzytkownik_nazwa) "
                         "LIKE '%"+ this->ui->txtPSzukaj->text() +"%' ORDER BY nazwisko;");

    try {

        this->ui->dgUzytkownicy->setModel(queryModel);

        this->ui->dgUzytkownicy->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgUzytkownicy->hideColumn(0); //chowanie kolumny z id

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }

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
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id: " <<  id_rekordu << " :: program.cpp";
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
        qDebug() << "Jest  pracownikiem :: program.cpp";
        #endif
        //**********************

        this->ui->cbP->setChecked(true);

        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************

        QSqlQueryModel zapytanie;

        zapytanie.setQuery("SELECT pon_od, pon_do, wt_od, wt_do, sr_od, sr_do, cz_od, cz_do, pt_od, pt_do FROM godziny "
                           "WHERE uzytkownik_id = '" + QString::number(id_rekordu) + "';"); //SELECT * to oznacza ze wszystko

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

    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Nie jest pracownikiem :: program.cpp";
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

}

//dodanie uzytkownikow do bazy
void Program::on_btnPdodaj_clicked()
{
    QString imie = this->ui->txtPImie->text();

    QString nazwisko = this->ui->txtPNazwisko->text();

    QString login = this->ui->txtPLogin->text();

    bool pracownik = this->ui->cbP->checkState();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> imie: " << imie << "nazwisko: " << nazwisko << "nazwa: " << login << "czy jest pracownikiem?: " << pracownik  << " :: program.cpp";
    #endif
    //**********************

    {

        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************

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
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

                msgRetry("ERROR", "NIE DODANO UZYTKOWNIKA");

            }
            else
            {
                msgOK("WARNING","DODANO UZYTKOWNIKA");
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
                    qDebug() << "Failed to add tag :: program.cpp";
                    #endif
                    //**********************

                    msgRetry("ERROR", "NIE DODANO SEKCJI GODZINY DO UZYTKOWNIKA");
                }
                else
                {
                    msgOK("WARNING","DODANO SEKCJE GODZINY DO UZYTKOWNIKA");
                }
            }

           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit :: program.cpp";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction :: program.cpp";
           #endif
           //**********************

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode :: program.cpp";
            #endif
            //**********************
        }

    }

    showRecords();

}
//~DODANIE UZYTKOWNIKA OKIENKO UZYTKOWNICY


//modyfikowacja danych uzytkownika
void Program::on_btnPmodyfikuj_clicked()
{
    QString imie = this->ui->txtPImie->text();

    QString nazwisko = this->ui->txtPNazwisko->text();

    QString login = this->ui->txtPLogin->text();

    bool pracownik = this->ui->cbP->checkState();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> imie: " << imie << "nazwisko: " << nazwisko << "nazwa: " << login << "czy jest pracownikiem?: " << pracownik << " :: program.cpp";
    #endif
    //**********************

    {
        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************


        if(baza.transaction())
        {


            QSqlQuery query(baza);

            query.prepare("UPDATE `Gabinet`.`uzytkownik` SET `imie` = '"+ imie +"', `nazwisko` = '"+ nazwisko +"', `uzytkownik_nazwa` = '"+ login +"', `pracownik` = "
                          ""+ QString::number(pracownik) +" WHERE uzytkownik_id = '" +  QString::number(id_rekordu) + "';");

            if( !query.exec() )
            {

                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

                msgRetry("ERROR", "NIE ZMODYFIKOWANO DANYCH UZYTKOWNIKA");
            }
            else
            {
                msgOK("WARNING", "ZMODYFIKOWANO DANE UZYTKOWNIKA");
            }

            QSqlQuery zapytanie;

            zapytanie.prepare("SELECT * FROM godziny WHERE uzytkownik_id = '" + QString::number(id_rekordu) + "';");

            zapytanie.exec();

            if((zapytanie.size() != 0) && (pracownik== true) ) //SPRAWDZANIE CZY JEST TABELA GODZINY DLA DANEGO ID
            {
                //******DEBUG_LOG*******
                #ifdef LOG
                    qDebug() << "istnieja godziny pracy dla tego rekordu :: program.cpp";
                #endif
                //**********************

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

                    msgRetry("ERROR", "NIE UDALO SIE ZMODYFIKOWAC GODZIN PRACY UZYTKOWNIKA");
                }
                else
                {
                    msgOK("WARNING", "ZMODYFIKOWANO GODIZNY PRACY UZYTKOWNIKA");
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
                    qDebug() << "Failed to add tag :: program.cpp";
                    #endif
                    //**********************

                    msgRetry("ERROR", "NIE UDALO SIE DODAC GODZIN PRACY DO UZYTKOWNIKA");
                }
                else
                {
                    msgOK("WARNING", "DODAO GODIZNY PRACY DO UZYTKOWNIKA");
                }

                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "nie istnieja godziny pracy dla tego rekordu dodano je :: program.cpp";
                #endif
                //**********************
            }

           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit :: program.cpp";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction :: program.cpp";
           #endif
           //**********************

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode :: program.cpp";
            #endif
            //**********************
        }

    }

    showRecords();
}
//~MODYFIKACJA DANYCH UZYTKOWNIKA OKIENKO UZYTKOWNIKA

//usuniecie uzytkownika z zabezpieczeniem przed usuwaniem admina
void Program::on_btnPusun_clicked()
{

        QString login = this->ui->txtPLogin->text();

        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************

        if(baza.transaction())
        {

            QMessageBox msgBox;

            QMessageBox::StandardButton reply;

            reply = msgBox.question(this, "WARRRNING", "CZY NA PEWNO CHCESZ USUNAC UZYTKOWNIKA", QMessageBox::Yes | QMessageBox::No);

            QSqlQuery query(baza);

            if(!(login == "admin") && reply == QMessageBox::Yes)
            {
                query.prepare("DELETE FROM `Gabinet`.`godziny` WHERE uzytkownik_id = '" +  QString::number(id_rekordu) + "';");

                query.exec();

                query.prepare("DELETE FROM `Gabinet`.`uzytkownik` WHERE uzytkownik_id = '" +  QString::number(id_rekordu) + "';");

                if( !query.exec() )
                {
                    //******DEBUG_LOG*******
                    #ifdef LOG
                    qDebug() << "Failed to add tag :: program.cpp";
                    #endif
                    //**********************

                   msgRetry("ERROR", "USUWNAIE UZYTKOWNIKA SIE NIE POWIODLO BLAD LACZENIA Z BAZA");
                }
                else
                {
                    msgOK("WARNING", "USUNIETO UZYTKOWNIKA");

                    clearRecord();
                }

               if(!baza.commit())
               {
                   //******DEBUG_LOG*******
                   #ifdef LOG
                   qDebug()<<"Failed to commit :: program.cpp";
                   #endif
                   //**********************

                   baza.rollback();
               }

               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug() << "Inserted using Qt Transaction :: program.cpp";
               #endif
               //**********************

            }
            else
            {
                msgRetry("ERROR", "USUWANIE UZYTKOWNIKA NIE POWIODLO SIE");
            }
        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode :: program.cpp";
            #endif
            //**********************
        }

    showRecords();
}
//~USUNIECIE UZYTKOWNIKA OKIENKO UZYTKOWNIKA

void Program::showRecords()
{

    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uzytkownik_id, imie, nazwisko, uzytkownik_nazwa AS login, pracownik FROM uzytkownik ORDER BY nazwisko;");

    try {

        this->ui->dgUzytkownicy->setModel(queryModel);

        this->ui->dgUzytkownicy->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgUzytkownicy->hideColumn(0); //chowanie kolumny z id

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }
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

    this->ui->txtKImie->clear();

    this->ui->txtKNazwisko->clear();

    this->ui->txtKemail->clear();

    this->ui->txtKmiejscowosc->clear();

    this->ui->txtKnumer->clear();

    this->ui->txtKpoczta->clear();

    this->ui->txtKulica->clear();

    this->ui->txtKtelefon->clear();

}

//ZMIANA STANU PRZYCIKU CZY JEST PRACOWNIKIEM
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

//czy trzeba do trzech po PRZECINKU SPRAWDZIC??!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//USTAWIANIE GODZIN PRACY -> PRZYCIKI GODZIN
void Program::on_btn7_15_clicked()
{
    czasPracy("07:00", "15:00");
}

void Program::on_btn8_16_clicked()
{
    czasPracy("08:00", "16:00");
}


void Program::on_btn9_17_clicked()
{
    czasPracy("09:00", "17:00");
}


void Program::on_btn10_18_clicked()
{
    czasPracy("10:00", "18:00");
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
//~USTAWIANIE CZASU GODZIN PRACY

//---------------------------------------------------------------------USLUGI----------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------


void Program::on_btnUSzukaj_clicked()
{
    showUslugi();
}

void Program::showUslugi()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT * FROM uslugi WHERE nazwa LIKE '%"+ this->ui->txtUzabieg->text() +"%' ORDER BY nazwa;");

    try {

        this->ui->dgZabiegi->setModel(queryModel);

        this->ui->dgZabiegi->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgZabiegi->hideColumn(0); //chowanie kolumny z id

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }

}

//DODANIE USLUGI
void Program::on_btnUDodaj_clicked()
{
    QString nazwa = this->ui->txtUnazwa->text();

    QString cena = this->ui->txtUcena->text().replace(",","."); //przecinek nie jest uznawany przez typ DECIMAL(6,2) -> 6 cyfr z czego 2 po przecinku

    QString czas = this->ui->txtUczas->text();

    QString opis= this->ui->txtUopis->toPlainText();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> nazwa: " << nazwa << "cena: " << cena << "czas: " << czas << "opis: " << opis  << " :: program.cpp";
    #endif
    //**********************

    {

        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************

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
                    qDebug() << "Failed to add tag :: program.cpp";
                    #endif
                    //**********************

                    msgRetry("ERROR", "NIE UDALO SIE DODAC USLUGI");
                }
                else
                {
                    msgOK("WARNING", "DODANO USLUGE");
                }

               if(!baza.commit())
               {
                   //******DEBUG_LOG*******
                   #ifdef LOG
                   qDebug()<<"Failed to commit :: program.cpp";
                   #endif
                   //**********************

                   baza.rollback();
               }

               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug() << "Inserted using Qt Transaction :: program.cpp";
               #endif
               //**********************
            }
        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode :: program.cpp";
            #endif
            //**********************
        }
    }

    showUslugi();
}
//~DODANIE USLUGI

//USUWANIE USLUGI
void Program::on_btnUUsun_clicked()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    if(baza.transaction())
    {

        QMessageBox msgBox;

        QMessageBox::StandardButton reply;

        reply = msgBox.question(this, "WARRRNING", "CZY NA PEWNO CHCESZ USUNAC USLUGE", QMessageBox::Yes | QMessageBox::No);

        QSqlQuery query(baza);

        if(reply == QMessageBox::Yes)
        {
            query.prepare("DELETE FROM `Gabinet`.`uslugi` WHERE uslugi_id = '" +  QString::number(id_rekordu) + "';");

            if( !query.exec() )
            {
                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

                msgRetry("ERROR", "BLAD USUWANIA USLUGI");
            }
            else
            {
                clearRecord();

                msgOK("WARNING", "USUNIETO USLUGE");
            }

        }
        else
        {

            msgRetry("ERROR", "BLAD USUWANIA USLUGI");

        }

       if(!baza.commit())
       {
           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug()<<"Failed to commit :: program.cpp";
           #endif
           //**********************

           baza.rollback();
       }

       //******DEBUG_LOG*******
       #ifdef LOG
       qDebug() << "Inserted using Qt Transaction :: program.cpp";
       #endif
       //**********************

    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Failed to start transaction mode :: program.cpp";
        #endif
        //**********************
    }

    showUslugi();
}
//~USUWANIE USLUGI

//MODYFIKUJ USLUGI
void Program::on_btnUModyfikuj_clicked()
{
    QString nazwa = this->ui->txtUnazwa->text();

    QString cena = this->ui->txtUcena->text().replace(",","."); //przecinek nie jest uznawany przez typ DECIMAL(6,2) -> 6 cyfr z czego 2 po przecinku

    QString czas = this->ui->txtUczas->text();

    QString opis= this->ui->txtUopis->toPlainText();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> nazwa: " << nazwa << "cena: " << cena << "czas: " << czas << "opis: " << opis  << " :: program.cpp";
    #endif
    //**********************

    {

        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************

        if(baza.transaction())
        {


            QSqlQuery query(baza);

            query.prepare("UPDATE `Gabinet`.`uslugi` SET `nazwa` = '"+ nazwa +"', `cena` = '"+ cena +"', `czas` = '"+ czas +"', `opis` = "
                          "'"+ opis +"' WHERE (`uslugi_id` = '"+ QString::number(id_rekordu) +"');");

            if( !query.exec() )
            {

                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

                msgRetry("ERROR", "NIE UDALO SIE ZMODYFIKOWAC DANYCH USLUGI");
            }
            else
            {
                msgOK("WARNING", "ZMODYFIKOWANE DANE USLUGI");
            }

           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit :: program.cpp";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction :: program.cpp";
           #endif
           //**********************

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode :: program.cpp";
            #endif
            //**********************
        }

    }

    showUslugi();
}
//~MODYFIKUJ USLUGI

//NACISNIECI PRZYCISKU W USLUGACH
void Program::on_dgZabiegi_clicked(const QModelIndex &index)
{
    this->ui->btnUModyfikuj->setEnabled(1);

    this->ui->btnUUsun->setEnabled(1);

    //dobranie się do numeru id kliknietego wiersza i do aktualnie nacisnietej komorki
    //QString currentCell = this->ui->dgUzytkownicy->currentIndex().data(Qt::DisplayRole).toString();
    id_rekordu = this->ui->dgZabiegi->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id: " <<  id_rekordu << " :: program.cpp";
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

}
//~NACISNIECIE PRZYCISKU W USLUGACH

//------------------------------------------------PRACOWNIK - USLUGA -------------------------------------------------------

//wcisniecie przycisku szukaj w okienku dodawania uslug do pracownika
void Program::on_btnPUSzukaj_clicked()
{
    pokaszPracownikow();
}

void Program::pokaszPracownikow()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uzytkownik_id, imie, nazwisko, uzytkownik_nazwa AS login, pracownik FROM uzytkownik "
                         "WHERE CONCAT(imie, ' ', nazwisko, uzytkownik_nazwa) LIKE '%"+ this->ui->txtPUSzukaj->text() +"%' AND pracownik = '1' ORDER BY nazwisko;");

    try {

        this->ui->dgPUPracownicy->setModel(queryModel);

        this->ui->dgPUPracownicy->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgPUPracownicy->hideColumn(0); //chowanie kolumny z id


    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }

}

//wcisniety rekord pracownika w okienku dodawania uslug do uzytkownika
void Program::on_dgPUPracownicy_clicked(const QModelIndex &index)
{

    id_rekordu = this->ui->dgPUPracownicy->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id: " <<  id_rekordu << " :: program.cpp";
    #endif
    //**********************

    QString tabNazwa = this->ui->dgPUPracownicy->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();

    QString tabNazwisko = this->ui->dgPUPracownicy->selectionModel()->selectedIndexes().at(2).data(Qt::DisplayRole).toString();

    this->ui->txtPUImie->setText(tabNazwa);

    this->ui->txtPUNazwisko->setText(tabNazwisko);

    PokazUslugi();

    PokazUslugiPracownika();

}

void Program::PokazUslugi()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    /*
    //TUTAJ PRZYKLAD JAKO TABELA
    QSqlTableModel *model = new QSqlTableModel;

    model->setTable("uslugi");

    model->select();
    */

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT * FROM uslugi ORDER BY nazwa;");

    try {

        this->ui->dgPUDodajUsluge->setModel(queryModel);

        this->ui->dgPUDodajUsluge->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgPUDodajUsluge->hideColumn(0); //chowanie kolumny z id

        this->ui->dgPUDodajUsluge->show();

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }
}

void Program::PokazUslugiPracownika()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uslugi.uslugi_id, uslugi.nazwa, uslugi.cena, uslugi.czas FROM uslugi, uzytkownik_usluga WHERE uslugi.uslugi_id = "
                         "uzytkownik_usluga.uslugi_id AND uzytkownik_usluga.uzytkownik_id = '"+ QString::number(id_rekordu) +"' ORDER BY nazwa;");
    try {

        this->ui->dgPUUslugi->setModel(queryModel);

        this->ui->dgPUUslugi->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgPUUslugi->hideColumn(0); //chowanie kolumny z id

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }
}

//klikniecie uslugi w wyborze uslug -> dodanie jej do pracownika
void Program::on_dgPUDodajUsluge_clicked(const QModelIndex &index)
{
    bool czyIstnieje = false;

    id_uslugiDoDodania = this->ui->dgPUDodajUsluge->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();
    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id_uslugi: " <<  id_uslugiDoDodania << " :: program.cpp";
    #endif
    //**********************

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    QSqlDatabase baza = QSqlDatabase::database();

    QSqlQuery zapytanie;

    zapytanie.exec("SELECT uzytkownik_usluga.uslugi_id FROM uzytkownik_usluga WHERE uzytkownik_usluga.uzytkownik_id = '"+ QString::number(id_rekordu) +"';");

    //zapytanie.exec("SELECT uzytkownik_nazwa, haslo FROM uzytkownik");

    while(zapytanie.next())
    {
        if(QString::number(id_uslugiDoDodania) == zapytanie.value(0).toString())
        {
            czyIstnieje = true;

            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "takie same id :: program.cpp";
            #endif
            //**********************
        }

    }

    if(czyIstnieje == false)
    {
        QMessageBox msgBox;

        QMessageBox::StandardButton reply;

        reply = msgBox.question(this, "WARRNING", "CZY NA PEWNO CHCESZ DODAC WYBRANA USLUGE DO PRACOWNIKA", QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::Yes)
        {
            DodajUsluge();
        }
        else
        {
            msgOK("ERROR", "NIE DODANO USLUGI");
        }
    }
    else
    {
        msgOK("ERROR", "TAKA USLUGA JUZ ISTNIEJE DLA WYBRANEGO PRACOWNIKA");
    }

    PokazUslugiPracownika();

}

void Program::DodajUsluge()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    QSqlQuery query(baza);

    if(baza.transaction())
    {

        query.prepare("INSERT INTO `Gabinet`.`uzytkownik_usluga` (`uzytkownik_id`, `uslugi_id`) "
                               "VALUES (:uzytkownik_id, :uslugi_id);");

        query.bindValue( ":uzytkownik_id", id_rekordu);

        query.bindValue( ":uslugi_id", id_uslugiDoDodania);

        if( !query.exec() )
        {

            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to add tag :: program.cpp";
            #endif
            //**********************

            msgRetry("ERROR", "NIE UDALO SIE DODAC USLUGI");
        }
        else
        {
            msgOK("WARNING", "DODANO USLUGE");

        }

        if(!baza.commit())
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug()<<"Failed to commit :: program.cpp";
            #endif
            //**********************

            baza.rollback();
        }

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Inserted using Qt Transaction :: program.cpp";
        #endif
        //**********************
    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Failed to start transaction mode :: program.cpp";
        #endif
        //**********************
    }
}

//usuniecie uslugi
void Program::on_dgPUUslugi_clicked(const QModelIndex &index)
{
    id_uslugiDoUsuniecia = this->ui->dgPUUslugi->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();
    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id_uslugi: " <<  id_uslugiDoDodania << " :: program.cpp";
    #endif
    //**********************

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************


    QMessageBox msgBox;

    QMessageBox::StandardButton reply;

    reply = msgBox.question(this, "WARRNING", "CZY NA PEWNO CHCESZ USUNAC WYBRANA USLUGE DO PRACOWNIKA", QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        usunUsluge();
    }
    else
    {
        msgOK("ERROR", "NIE USUNIETO USLUGI");
    }

    PokazUslugiPracownika();
}

void Program::usunUsluge()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    QSqlQuery query(baza);

    if(baza.transaction())
    {
        query.prepare("DELETE FROM `Gabinet`.`uzytkownik_usluga` WHERE uslugi_id = '" +  QString::number(id_uslugiDoUsuniecia) + "';");

        if( !query.exec() )
        {

            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to add tag :: program.cpp";
            #endif
            //**********************

            msgRetry("ERROR", "NIE UDALO SIE DODAC USLUGI");
        }
        else
        {
            msgOK("WARNING", "USUNIETO USLUGE");

        }

        if(!baza.commit())
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug()<<"Failed to commit :: program.cpp";
            #endif
            //**********************

            baza.rollback();
        }

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Inserted using Qt Transaction :: program.cpp";
        #endif
        //**********************
    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Failed to start transaction mode :: program.cpp";
        #endif
        //**********************
    }
}

//----------------------------------------------------KLIENCI------------------------------------------------------

void Program::on_btnKSzukaj_clicked()
{
    szukajKlient();
}


void Program::on_btnKDodaj_clicked()
{
    QString imie = this->ui->txtKImie->text();

    QString nazwisko = this->ui->txtKNazwisko->text();

    QString email = this->ui->txtKemail->text();

    QString telefon = this->ui->txtKtelefon->text();

    QString ulica = this->ui->txtKulica->text();

    QString Miejscowosc = this->ui->txtKmiejscowosc->text();

    QString poczta = this->ui->txtKpoczta->text();
    //numer mieszkania
    //modyfikacja

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    QSqlDatabase baza = QSqlDatabase::database();

    if(baza.transaction())
    {
        if(true)
        {
            QSqlQuery query(baza);

            query.prepare("INSERT INTO `Gabinet`.`klient` (`imie`, `nazwisko`, `email`, `telefon`, `ulica`, `Miejscowosc`, `poczta`) "
                          "VALUES (:imie, :nazwisko, :email, :telefon, :ulica, :Miejscowosc, :poczta);");

            query.bindValue( ":imie", imie);

            query.bindValue( ":nazwisko", nazwisko);

            query.bindValue( ":email", email);

            query.bindValue( ":telefon", telefon);

            query.bindValue( ":ulica", ulica);

            query.bindValue( ":Miejscowosc", Miejscowosc);

            query.bindValue( ":poczta", poczta);

            if( !query.exec() )
            {
                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

                msgRetry("ERROR", "NIE UDALO SIE DODAC KLIENTA");
            }
            else
            {
                msgOK("WARNING", "DODANO KLIENTA");
            }

           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit :: program.cpp";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction :: program.cpp";
           #endif
           //**********************
        }
    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Failed to start transaction mode :: program.cpp";
        #endif
        //**********************
    }

    szukajKlient();
}


void Program::on_btnKModyfikuj_clicked()
{
    QString imie = this->ui->txtKImie->text();

    QString nazwisko = this->ui->txtKNazwisko->text();

    QString email = this->ui->txtKemail->text();

    QString telefon = this->ui->txtKtelefon->text();

    QString ulica = this->ui->txtKulica->text();

    QString Miejscowosc = this->ui->txtKmiejscowosc->text();

    QString poczta = this->ui->txtKpoczta->text();

    //******DEBUG_LOG*******
    #ifdef LOG
    qDebug() << "Wpisane dane -> imie: " << imie << "nazwisko: " << nazwisko << "nazwa: " << login << "czy jest pracownikiem?: " << pracownik << " :: program.cpp";
    #endif
    //**********************

    {
        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************


        if(baza.transaction())
        {


            QSqlQuery query(baza);

            query.prepare("UPDATE `Gabinet`.`klient` SET `imie` = '"+ imie +"', `nazwisko` = '"+ nazwisko +"', `email` = '"+ email +"', `telefon` = '"
                          ""+ telefon +"', `ulica` = '"+ ulica +"', `Miejscowosc` = '"+ Miejscowosc +"', `poczta` = '"+ poczta +"' WHERE klient_id = '" +  QString::number(id_rekordu) + "';");

            if( !query.exec() )
            {

                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

                msgRetry("ERROR", "NIE ZMODYFIKOWANO DANYCH KLIENTA");
            }
            else
            {
                msgOK("WARNING", "ZMODYFIKOWANO DANE KLIENTA");
            }


           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit :: program.cpp";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction :: program.cpp";
           #endif
           //**********************

        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode :: program.cpp";
            #endif
            //**********************
        }

    }

        szukajKlient();
}


void Program::on_btnKUsun_clicked()
{

    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    if(baza.transaction())
    {

        QMessageBox msgBox;

        QMessageBox::StandardButton reply;

        reply = msgBox.question(this, "WARRRNING", "CZY NA PEWNO CHCESZ USUNAC KLIENTA", QMessageBox::Yes | QMessageBox::No);

        QSqlQuery query(baza);

        if(reply == QMessageBox::Yes)
        {
            query.prepare("DELETE FROM `Gabinet`.`klient` WHERE klient_id = '" +  QString::number(id_rekordu) + "';");

            if( !query.exec() )
            {
                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

                msgRetry("ERROR", "USUWANIE KLIENTA SIE NIE POWIODLO BLAD LACZENIA Z BAZA");
            }
            else
            {
                msgOK("WARNING", "USUNIETO KLIENTA");

                clearRecord();
            }

           if(!baza.commit())
           {
               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug()<<"Failed to commit :: program.cpp";
               #endif
               //**********************

               baza.rollback();
           }

           //******DEBUG_LOG*******
           #ifdef LOG
           qDebug() << "Inserted using Qt Transaction :: program.cpp";
           #endif
           //**********************

        }
        else
        {
            msgRetry("ERROR", "USUWANIE KLIENTA NIE POWIODLO SIE");
        }
    }
    else
    {
        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug() << "Failed to start transaction mode :: program.cpp";
        #endif
        //**********************
    }

    szukajKlient();

}


void Program::on_dgKlienci_clicked(const QModelIndex &index)
{
    this->ui->btnKModyfikuj->setEnabled(1);

    this->ui->btnKUsun->setEnabled(1);

    //dobranie się do numeru id kliknietego wiersza i do aktualnie nacisnietej komorki
    //QString currentCell = this->ui->dgUzytkownicy->currentIndex().data(Qt::DisplayRole).toString();
    id_rekordu = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id: " <<  id_rekordu << " :: program.cpp";
    #endif
    //**********************

    QString imie = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();

    QString nazwisko = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(2).data(Qt::DisplayRole).toString();

    QString email = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(3).data(Qt::DisplayRole).toString();

    QString telefon = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(4).data(Qt::DisplayRole).toString();

    QString ulica = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(5).data(Qt::DisplayRole).toString();

    QString Miejscowosc = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(6).data(Qt::DisplayRole).toString();

    QString poczta = this->ui->dgKlienci->selectionModel()->selectedIndexes().at(7).data(Qt::DisplayRole).toString();

    this->ui->txtKImie->setText(imie);

    this->ui->txtKNazwisko->setText(nazwisko);

    this->ui->txtKemail->setText(email);

    this->ui->txtKtelefon->setText(telefon);

    this->ui->txtKulica->setText(ulica);

    this->ui->txtKmiejscowosc->setText(Miejscowosc);

    this->ui->txtKpoczta->setText(poczta);

}

void Program::szukajKlient()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT * FROM Gabinet.klient WHERE CONCAT(imie, ' ', nazwisko) "
                         "LIKE '%"+ this->ui->txtKszukaj->text() +"%' ORDER BY nazwisko;");

    try {

        this->ui->dgKlienci->setModel(queryModel);

        this->ui->dgKlienci->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgKlienci->hideColumn(0); //chowanie kolumny z id

        this->ui->dgKlienci->hideColumn(8);

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }
}


//---------------------------------------------WIADOMOSCI--------------------------------------------------------
//------------------------------OK-------------RETRY------------BLAD_LACZENIA------------------------------------
void Program::MsgBladLaczeniaBazy()
{
    QMessageBox msgBox;

    msgBox.setWindowTitle("ERROR");

    msgBox.setInformativeText("Blad polaczenia z baza danych");

    msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);

    msgBox.setDefaultButton(QMessageBox::Retry);

    msgBox.exec();
}

void Program::msgOK(QString title, QString msg)
{
    QMessageBox msgBox;

    msgBox.setWindowTitle(title);

    msgBox.setInformativeText(msg);

    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.exec();
}

void Program::msgRetry(QString title, QString msg)
{
    QMessageBox msgBox;

    msgBox.setWindowTitle(title);

    msgBox.setInformativeText(msg);

    msgBox.setStandardButtons(QMessageBox::Retry);

    msgBox.exec();
}

//--------------------------DO POPRAWY-----------------------


//wzcytywanie informacji z bazy danych -> jedna funkcja ze zmienna ktora tabela i selekt
//w uslugach dziala opcja modyfikuj i usun gdy wszystkie rekordy usuniete sa -> uodpornic sie
//MOZNA DODAC DO WSZYTSKICH POL TEKSTOWYCH retsrykcje co moga zawierac jak w kliencie
//klient numer mieszkania nie ma w bazie sql rodzielic jakos do ulicy zeby szlo
//sprawdzenie przy kliencie czy jak dodaje to pola nie puste

//------------------------------------------------Wizyty----------------------------------------------------------

int id_klienta;
int id_pracownika;
int id_uslugi;
int godzina_od;
int godzina_do;
bool weekend = false;
QGridLayout *myLayout  = new QGridLayout;



void Program::pokaszPracownikowWizyty()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT uzytkownik_id, imie, nazwisko, uzytkownik_nazwa AS login, pracownik FROM uzytkownik "
                         "WHERE CONCAT(imie, ' ', nazwisko, uzytkownik_nazwa) LIKE '%"+ this->ui->txtRPracownikSzukaj->text() +"%' AND pracownik = '1' ORDER BY nazwisko;");

    try {

        this->ui->dgRPracownik->setModel(queryModel);

        this->ui->dgRPracownik->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgRPracownik->hideColumn(0); //chowanie kolumny z id


    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }

}

void Program::showUslugiWizyty()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT * FROM uslugi WHERE nazwa LIKE '%"+ this->ui->txtRuslugaSzukaj->text() +"%' ORDER BY nazwa;");

    try {

        this->ui->dgRUsluga->setModel(queryModel);

        this->ui->dgRUsluga->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgRUsluga->hideColumn(0); //chowanie kolumny z id

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }

}

void Program::szukajKlientWizyty()
{
    QSqlDatabase baza = QSqlDatabase::database();

    //******DEBUG_LOG*******
    #ifdef LOG
    if(!baza.open())
    {
        qDebug() << "nie otwarta baza :: program.cpp";
    }
    #endif
    //**********************

    queryModel = new QSqlQueryModel(this);

    queryModel->setQuery("SELECT * FROM Gabinet.klient WHERE CONCAT(imie, ' ', nazwisko) "
                         "LIKE '%"+ this->ui->txtRklientSzukaj->text() +"%' ORDER BY nazwisko;");

    try {

        this->ui->dgRKlient->setModel(queryModel);

        this->ui->dgRKlient->setSelectionBehavior(QAbstractItemView::SelectRows); //zaznaczanie calego wiersza

        this->ui->dgRKlient->hideColumn(0); //chowanie kolumny z id

        this->ui->dgRKlient->hideColumn(8);

    } catch (const std::exception &e)
    {

        //******DEBUG_LOG*******
        #ifdef LOG
        qDebug()<<"Blad polaczenia z baza danych! :: program.cpp";
        qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        MsgBladLaczeniaBazy();
    }
}

void Program::on_btnRSzukajUsluga_clicked()
{
    showUslugiWizyty();
}

 void Program::on_btnRSzukajPracownik_clicked()
 {
    pokaszPracownikowWizyty();
 }

 void Program::on_btnRSzukajKlient_clicked()
 {
    szukajKlientWizyty();
 }

void Program::on_dgRUsluga_clicked(const QModelIndex &index)
{
    id_uslugi = this->ui->dgRUsluga->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id: " <<  id_rekordu << " :: program.cpp";
    #endif
    //**********************

    QString nazwa = this->ui->dgRUsluga->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();

    this->ui->txtRUsluga->setText(nazwa);
}

void Program::on_dgRKlient_clicked(const QModelIndex &index)
{
    id_klienta = this->ui->dgRKlient->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id: " <<  id_rekordu << " :: program.cpp";
    #endif
    //**********************
    QString imie = this->ui->dgRKlient->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();

    QString nazwisko = this->ui->dgRKlient->selectionModel()->selectedIndexes().at(2).data(Qt::DisplayRole).toString();

    this->ui->txtRklient->setText(imie + " " + nazwisko);
}

void Program::on_dgRPracownik_clicked(const QModelIndex &index)
{
    id_pracownika = this->ui->dgRPracownik->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();

    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id: " <<  id_rekordu << " :: program.cpp";
    #endif
    //**********************
    QString imie = this->ui->dgRPracownik->selectionModel()->selectedIndexes().at(1).data(Qt::DisplayRole).toString();

    QString nazwisko = this->ui->dgRPracownik->selectionModel()->selectedIndexes().at(2).data(Qt::DisplayRole).toString();

    this->ui->txtRPracownik->setText(imie + " " + nazwisko);

    QDate date = this->ui->calendarWidget->selectedDate();

    dataGodzinyPracy(date);

    createButtons();

}

void Program::actionButtonClick(QString text)
{
    if(this->ui->txtRTermin->text().length() > 0)
    {
        zamienDate();

        QString data = this->ui->txtRTermin->text();


        QString dataPlusGodzina = data + " " + text;

        this->ui->txtRTermin->setText(dataPlusGodzina);

    }
    else
    {
        msgOK("WARRNING", "wybierz najpierw datę potem godzinę");
    }

}

void Program::createButtons()
{
    cleaningGBGodziny();

    if(weekend != true)
    {

        QStringList texts;

        QStringList zajeteTerminy;

        for (int i = 0; i < (godzina_do-godzina_od); i++)
        {

            int godzinaiInt = godzina_od + i;

            QString godzina;

            if(godzinaiInt < 10)
            {
                godzina = "0" + QString::number(godzinaiInt);
            }
            else
            {
                godzina =  QString::number(godzinaiInt);
            }

            for (int j=0; j<60; j+=30)
            {
                QString minuta;
                if(j==0)
                {
                    minuta ="00";
                }
                else
                {
                    minuta = QString::number(j);
                }

                QString calosc = godzina + ":" + minuta;

                texts.append(calosc);
            }

        }

        QSqlDatabase baza = QSqlDatabase::database();

        QSqlQuery zapytanie;

        zapytanie.exec("SELECT rezerwacja_od, status FROM wizyty WHERE uzytkownik_id = '"+ QString::number(id_pracownika) +"'");

        while(zapytanie.next())
        {
            zajeteTerminy.append(zapytanie.value(0).toString());
        }

        for (int i = 0; i < texts.size(); ++i)
        {
            QString text = texts[i];

            QPushButton *button = new QPushButton(text);

            for (int j=0; j<zajeteTerminy.size(); j++)
            {
                if( zajeteTerminy[j].mid(0,10) == this->ui->txtRTermin->text().mid(0,10))
                {
                    if(text == zajeteTerminy[j].mid(11,5))
                    {
                        button->setStyleSheet("background-color:red;");

                    }
                }
            }

            //QString style =  button->styleSheet();

            button->setMaximumWidth(40);

            button->setMaximumHeight(20);

            connect(button, &QPushButton::clicked, [this, text] { actionButtonClick(text); });

            myLayout->addWidget(button, i+1, 3);
        }

        this->ui->gbRGodziny->setLayout(myLayout);

    }


}

void Program::on_calendarWidget_clicked(const QDate &date)
{

    dataGodzinyPracy(date);

}

void Program::dataGodzinyPracy(const QDate &date)
{
    int dzien =  date.dayOfWeek();

    QString praca_od;
    QString praca_do;


    switch(dzien)
    {
    case 1:
        weekend = false;
        praca_od = "pon_od";
        praca_do = "pon_do";
        break;
    case 2:
        weekend = false;
        praca_od = "wt_od";
        praca_do = "wt_do";
        break;
    case 3:
        weekend = false;
        praca_od = "sr_od";
        praca_do = "sr_do";
        break;
    case 4:
        weekend = false;
        praca_od = "cz_od";
        praca_do = "cz_do";
        break;
    case 5:
        weekend = false;
        praca_od = "pt_od";
        praca_do = "pt_do";
        break;
    case 6:
        weekend = true;
        break;
    case 7:
        weekend = true;
        break;
    }

    zamienDate();

    if(weekend == false)
    {
        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************

        QSqlQueryModel zapytanie;

        zapytanie.setQuery("SELECT "+praca_od+", "+ praca_do +" FROM godziny WHERE uzytkownik_id = '"+ QString::number(id_pracownika) +"';");

        praca_od = zapytanie.record(0).value(0).toString().mid(0,2);

        praca_do = zapytanie.record(0).value(1).toString().mid(0,2);

        godzina_od = praca_od.toInt();

        godzina_do = praca_do.toInt();

        if(id_pracownika != 0)
        {
            createButtons();
        }
        else
        {
            msgRetry("ERROR", "Wybierz pracownika");
        }
    }
    else
    {
        cleaningGBGodziny();

        this->ui->gbRGodziny->setLayout(myLayout);
    }
}

void Program::cleaningGBGodziny()
{
    while( myLayout->count() )
    {
        QWidget* widget = myLayout->itemAt(0)->widget();
        if( widget )
        {
            myLayout->removeWidget(widget);
            delete widget;
        }
    }
}


void Program::on_btnRDodaj_clicked()
{
    if(id_klienta != 0 && id_pracownika != 0 && id_uslugi !=0  && this->ui->txtRTermin->text().length() > 10)
    {

        QSqlDatabase baza = QSqlDatabase::database();

        //******DEBUG_LOG*******
        #ifdef LOG
        if(!baza.open())
        {
            qDebug() << "nie otwarta baza :: program.cpp";
        }
        #endif
        //**********************

        if(baza.transaction())
        {
            if(true)
            {
                QSqlQuery query(baza);

                QString data_od = this->ui->txtRTermin->text();

                QString data_do;

               // dataGodzinyPracy(date);

                query.prepare("INSERT INTO `Gabinet`.`wizyty` (`klient_id`, `uslugi_id`, `uzytkownik_id`, `rezerwacja_od`, `rezerwacja_do`, `status`) "
                              "VALUES (:klient_id, :uslugi_id, :uzytkownik_id, :rezerwacja_od, '1970-01-01 00:00:00', :status);");

                query.bindValue( ":klient_id", id_klienta);

                query.bindValue( ":uslugi_id", id_uslugi);

                query.bindValue( ":uzytkownik_id", id_pracownika);

                query.bindValue( ":rezerwacja_od", data_od);

                //query.bindValue( ":klient_id", id_klienta);

                query.bindValue( ":status", "oczekuje");

                if( !query.exec() )
                {
                    //******DEBUG_LOG*******
                    #ifdef LOG
                    qDebug() << "Failed to add tag :: program.cpp";
                    #endif
                    //**********************

                    msgRetry("ERROR", "NIE UDALO SIE DODAC WIZYTY");
                }
                else
                {
                    msgOK("WARNING", "DODANO WIZYTE");
                }

               if(!baza.commit())
               {
                   //******DEBUG_LOG*******
                   #ifdef LOG
                   qDebug()<<"Failed to commit :: program.cpp";
                   #endif
                   //**********************

                   baza.rollback();
               }

               //******DEBUG_LOG*******
               #ifdef LOG
               qDebug() << "Inserted using Qt Transaction :: program.cpp";
               #endif
               //**********************
            }
        }
        else
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to start transaction mode :: program.cpp";
            #endif
            //**********************
        }
    }
    else
    {
        msgOK("WARRNING","uzupełnij wybór");
    }

    createButtons();
}


void Program::on_btnRUsun_clicked()
{

}


void Program::on_btnRmodyfikuj_clicked()
{

}

void Program::zamienDate()
{
    QDate date = this->ui->calendarWidget->selectedDate();

    int year;

    int monthInt;

    int dayInt;

    date.getDate( &year, &monthInt, &dayInt);

//    qDebug() << year << " " << month << " " << day;

    QString day;

    QString month;

    if(dayInt <10)
    {
        day = "0" + QString::number(dayInt);
    }
    else
    {
         day = QString::number(dayInt);
    }

    if(monthInt <10)
    {
        month = "0" + QString::number(monthInt);
    }
    else
    {
        month = QString::number(monthInt);
    }

    this->ui->txtRTermin->setText(QString::number(year) + "-" + month + "-" + day);
}

