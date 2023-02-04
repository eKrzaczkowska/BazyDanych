//pliki naglowkowe
#include "logowanie.h"
#include "program.h"


//okno LOGOWANIA
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

//po kliknieciu przyciku zaloguj
void logInWindow::on_btnZaloguj_clicked()
{
    //Pobieranie info z text boxa
    QString nazwaUzytkownikaLogowanie = ui->txtUzytkownik->text();

    QString hasloLogowanie = ui->txtHaslo->text();

    //**********************
    //******DEBUG_LOG*******
    //qDebug()<<"nazwa uzytkownika text box: "<< nazwaUzytkownikaLogowanie;
    //qDebug()<<"haslo text box: "<< hasloLogowanie;
    //**********************


    //connecting to mysql
    QSqlDatabase baza;

    baza = QSqlDatabase::addDatabase("QMYSQL");

    baza.setDatabaseName("Gabinet"); // ustawiam nazwę biblioteki, pod którą chcę się podpiąć

    baza.setHostName("127.0.0.1"); // nazwa serwera

    baza.setPassword("password"); // hasło

    baza.setPort(3306); // port połączenia z bazą danych

    baza.setUserName("root"); // nazwa użytkownika

    try
    {

        //sciaganie z bazy danych wszystkich rekordow uzytkownik_nazwa i haslo
        baza.open();

        QSqlQuery zapytanie;

        zapytanie.exec("SELECT uzytkownik_nazwa, haslo FROM uzytkownik");

        //inny sposob dostania sie do id uzytkownika
        //zapytanie.exec("SELECT uzytkownik_id FROM uzytkownik WHERE uzytkownik_nazwa = '"+ui->txtUzytkownik->text()+"' AND haslo = '"+ui->txtHaslo->text()+"'");
        //qDebug()<<"id uzytkownika: "<< zapytanie.value(0).toInt();


        while(zapytanie.next())
        {

            //**********************
            //******DEBUG_LOG*******
            //qDebug()<<"nazwa uzytkownika: "<<zapytanie.value(0).toString();
            //qDebug()<<"haslo: "<<zapytanie.value(1).toString();
            //**********************

            QString nazwaUzytkownika = zapytanie.value(0).toString();

            QString haslo = zapytanie.value(1).toString();

            if(nazwaUzytkownikaLogowanie == nazwaUzytkownika && hasloLogowanie == haslo)
            {

                //**********************
                //******DEBUG_LOG*******
                //qDebug() << "Logowanie udane";
                //**********************

                this->hide();

                Program * mainWindow  = new Program(nazwaUzytkownika);

                mainWindow->show();

                baza.close();

                this->close();

            }
            else
            {

                //**********************
                //******DEBUG_LOG*******
                //qDebug() << "Logowanie sie nie powiodlo";
                //**********************

                QMessageBox msgBox;

                msgBox.setWindowTitle("WARNING");

                msgBox.setInformativeText("Bledna nazwa uzytkownika lub niepoprawne haslo");

                msgBox.setStandardButtons(QMessageBox::Retry);

                msgBox.setDefaultButton(QMessageBox::Retry);

                int ret = msgBox.exec();

            }
        }


    } catch (const std::exception &e)
    {

        //**********************
        //******DEBUG_LOG*******
        //qDebug()<<"Blad polaczenia z baza danych!";
        //qDebug() << "ERROR load: " << baza.lastError().text();
        //**********************

        QMessageBox msgBox;

        msgBox.setWindowTitle("ERROR");

        msgBox.setInformativeText("Blad polaczenia z baza danych :: logowanie.cpp");

        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Close);

        msgBox.setDefaultButton(QMessageBox::Retry);

        int ret = msgBox.exec();
    }

}



