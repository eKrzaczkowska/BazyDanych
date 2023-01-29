#ifndef PROGRAM_H
#define PROGRAM_H

#include <QDialog>
#include <string>
#include <iostream>
#include <QtSql/QSqlDatabase>

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

protected:

    ~Program();

private slots:
    void on_txtHStare_textChanged(const QString &arg1);

    void on_txtHNowe_textChanged(const QString &arg1);

    void on_txtHPowtorzone_textChanged(const QString &arg1);

    void on_btnHZmien_clicked();

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

    Ui::Program *ui;

};

#endif // PROGRAM_H
