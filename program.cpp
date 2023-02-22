#include "program.h"
#include "ui_program.h"

//odznaczyc jezeli chcemy logi
#define LOG

//zmienne globalne

QString nazwaUzytkownika;

int id_rekordu;

int id_uslugiDoDodania;

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


}

Program::~Program()
{
    QSqlDatabase baza = QSqlDatabase::database();

    if(baza.open())
    {

        baza.close();
    }

    QSqlDatabase::removeDatabase(baza.connectionName());

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

            query.exec();

            clearRecord();

            msgOK("WARNING", "USUNIETO UZYTKOWNIKA");

            }
            else
            {
                msgRetry("ERROR", "USUWANIE UZYTKOWNIKA NIE POWIODLO SIE");
            }

            if( !query.exec() )
            {
                //******DEBUG_LOG*******
                #ifdef LOG
                qDebug() << "Failed to add tag :: program.cpp";
                #endif
                //**********************

               msgRetry("ERROR", "USUWNAIE UZYTKOWNIKA SIE NIE POWIODLO");
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

            query.exec();

            clearRecord();

            msgOK("WARNING", "USUNIETO USLUGE");

        }
        else
        {

            msgRetry("ERROR", "BLAD USUWANIA USLUGI");

        }

        if( !query.exec() )
        {
            //******DEBUG_LOG*******
            #ifdef LOG
            qDebug() << "Failed to add tag :: program.cpp";
            #endif
            //**********************

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

        MsgBladLaczeniaBazy();
    }
}

//klikniecie uslugi w wyborze uslug
void Program::on_dgPUDodajUsluge_clicked(const QModelIndex &index)
{
    id_uslugiDoDodania = this->ui->dgPUDodajUsluge->selectionModel()->selectedIndexes().at(0).data(Qt::DisplayRole).toInt();
    //******DEBUG_LOG*******
    #ifdef LOG
    //qDebug() << "nacisnieta komorka: " << currentCell << " :: program.cpp";
    qDebug() << "komorka o numerze id_uslugi: " <<  id_uslugiDoDodania << " :: program.cpp";
    #endif
    //**********************

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
