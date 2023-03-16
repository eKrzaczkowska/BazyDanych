#ifndef PROGRAM_H
#define PROGRAM_H

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
#include <QDialog>
#include <iostream>
#include <QtSql/QSqlTableModel>
#include <QFile>
#include <QSqlQuery>
#include <QRegularExpressionValidator>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QObject>
#include <QDataWidgetMapper>
#include <QSignalMapper>



//#include <QRegExpValidator>

namespace Ui {
class Program;
}

class Program : public QDialog
{
    Q_OBJECT

public:

    explicit Program(QWidget *parent = nullptr);

    Program(QString nazwaUzytkownikaLog);

    void LaczenieDoSQL(QSqlDatabase *baza);

    QSqlQueryModel *queryModel;

protected:

    ~Program();

private slots:
    void on_txtHStare_textChanged(const QString &arg1);

    void on_txtHNowe_textChanged(const QString &arg1);

    void on_txtHPowtorzone_textChanged(const QString &arg1);

    void on_btnHZmien_clicked();

    void on_btnPSzukaj_clicked();

    void on_dgUzytkownicy_clicked(const QModelIndex &index);

    void on_btnPdodaj_clicked();

    void on_btnPmodyfikuj_clicked();

    void on_btnPusun_clicked();

    void on_cbP_stateChanged(int arg1);

    void on_btn7_15_clicked();

    void on_btn8_16_clicked();

    void on_btn9_17_clicked();

    void on_btn10_18_clicked();

    void on_btnUSzukaj_clicked();

    void on_btnUDodaj_clicked();

    void on_btnUUsun_clicked();

    void on_btnUModyfikuj_clicked();

    void on_dgZabiegi_clicked(const QModelIndex &index);

    void on_btnPUSzukaj_clicked();

    void on_dgPUPracownicy_clicked(const QModelIndex &index);

    void on_dgPUDodajUsluge_clicked(const QModelIndex &index);

    void on_dgPUUslugi_clicked(const QModelIndex &index);

    void on_btnKSzukaj_clicked();

    void on_btnKDodaj_clicked();

    void on_btnKModyfikuj_clicked();

    void on_btnKUsun_clicked();

    void on_dgKlienci_clicked(const QModelIndex &index);

    void on_btnRSzukajUsluga_clicked();

    void on_btnRSzukajPracownik_clicked();

    void on_btnRSzukajKlient_clicked();

    void on_dgRUsluga_clicked(const QModelIndex &index);

    void on_dgRKlient_clicked(const QModelIndex &index);

    void on_dgRPracownik_clicked(const QModelIndex &index);

    void actionButtonClick(QString text);

    void on_calendarWidget_clicked(const QDate &date);

    void on_btnRDodaj_clicked();

    void on_btnRUsun_clicked();

    void on_btnRmodyfikuj_clicked();

    //void on_tabWidget_currentChanged(int index);

private:

    void PobieranieDanych(QString nazwaUzytkownikaLog, struct Uzytkownik *uzytkownik);

    void btnZmien_pokaz();

    void showRecords();

    void clearRecord();

    void czasPracy(QString poczatek, QString koniec);

    void showUslugi();

    void UzytkownicySzukaj();

    void pokaszPracownikow();

    void PokazUslugi();

    void MsgBladLaczeniaBazy();

    void msgOK(QString title, QString msg);

    void msgRetry(QString title, QString msg);

    void PokazUslugiPracownika();

    void DodajUsluge();

    void usunUsluge();

    void szukajKlient();

    void pokaszPracownikowWizyty();

    void showUslugiWizyty();

    void szukajKlientWizyty();

    void createButtons();

    void wyswietl(QString text);

    void dataGodzinyPracy(const QDate &date);

    void cleaningGBGodziny();

    void zamienDate();

    Ui::Program *ui;

};

#endif // PROGRAM_H
