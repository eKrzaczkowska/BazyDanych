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

private:

    struct Uzytkownik
    {
        int id = 0;
        QString imie = "";
        QString nazwisko = "";
        QString haslo = "";
        bool jestPracownikiem = false;
    };

    void PobieranieDanych(QString nazwaUzytkownikaLog, struct Uzytkownik *uzytkownik);

    void btnZmien_pokaz();

    void showRecords();

    void clearRecord();

    void czasPracy(QString poczatek, QString koniec);

    void showUslugi();

    Ui::Program *ui;

};

#endif // PROGRAM_H
