//pliki naglowkowe
#include "logowanie.h"
#include "program.h"

//#define LOG


//okno LOGOWANIA
logInWindow::logInWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::logInWindow) 
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
        qDebug() << "Bazy danych nie udało się otworzyc :: logowanie.cpp";
    }
    else
    {
        qDebug() << "Baza danych otwarta :: logowanie.cpp";
    }
    #endif
    //**********************

}

logInWindow::~logInWindow()
{
    delete ui;

    QSqlDatabase baza = QSqlDatabase::database();

    baza.close();

    QSqlDatabase::removeDatabase(baza.connectionName());
}

//po kliknieciu przyciku zaloguj
void logInWindow::on_btnZaloguj_clicked()
{
    //Pobieranie info z text boxa
    QString nazwaUzytkownikaLogowanie = ui->txtUzytkownik->text();

    QString hasloLogowanie = ui->txtHaslo->text();

    //******DEBUG_LOG*******
    #ifdef LOG
        qDebug()<<"nazwa uzytkownika text box: "<< nazwaUzytkownikaLogowanie << " :: logowanie.cpp";
        qDebug()<<"haslo text box: "<< hasloLogowanie << " :: logowanie.cpp";
    #endif
    //**********************

    //connecting to mysql
    QSqlDatabase baza = QSqlDatabase::database();

    try
    {
        bool zgodne = true;

        //sciaganie z bazy danych wszystkich rekordow uzytkownik_nazwa i haslo

        QSqlQuery zapytanie;

        zapytanie.exec("SELECT uzytkownik_nazwa, haslo FROM uzytkownik");

        while(zapytanie.next())
        {

            //******DEBUG_LOG*******
            #ifdef LOG
                qDebug()<<"nazwa uzytkownika: "<<zapytanie.value(0).toString()  << " :: logowanie.cpp";
                qDebug()<<"haslo: "<<zapytanie.value(1).toString()  << " :: logowanie.cpp";
            #endif
            //**********************

            QString nazwaUzytkownika = zapytanie.value(0).toString();

            QString haslo = zapytanie.value(1).toString();

            if(nazwaUzytkownikaLogowanie == nazwaUzytkownika && hasloLogowanie == haslo)
            {
                zgodne = true;

                //******DEBUG_LOG*******
                #ifdef LOG
                    qDebug() << "Logowanie udane :: logowanie.cpp";
                #endif
                //**********************

                this->hide();

                Program * mainWindow  = new Program(nazwaUzytkownika);

                mainWindow->show();

                delete this->ui;

            }
            else
            {
                zgodne = false;
            }

        }

        //******DEBUG_LOG*******
        #ifdef LOG
            qDebug() << "Logowanie sie nie powiodlo :: logowanie.cpp";
        #endif
        //**********************

        if(zgodne == false)
        {

            QMessageBox msgBox;

            msgBox.setWindowTitle("WARNING");

            msgBox.setInformativeText("Bledna nazwa uzytkownika lub niepoprawne haslo");

            msgBox.setStandardButtons(QMessageBox::Retry);

            msgBox.setDefaultButton(QMessageBox::Retry);

            msgBox.exec();

        }


    } catch (const std::exception &e)
    {
        //******DEBUG_LOG*******
        #ifdef LOG
            qDebug() <<"Blad polaczenia z baza danych! :: logowanie.cpp";
            qDebug() << "ERROR load: " << baza.lastError().text();
        #endif
        //**********************

        QMessageBox msgBox;

        msgBox.setWindowTitle("ERROR");

        msgBox.setInformativeText("Blad polaczenia z baza danych :: logowanie.cpp");

        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);

        msgBox.setDefaultButton(QMessageBox::Retry);

        int ret = msgBox.exec();
    }

}



