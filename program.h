#ifndef PROGRAM_H
#define PROGRAM_H

#include <QDialog>
#include <string>
#include <iostream>

namespace Ui {
class Program;
}

class Program : public QDialog
{
    Q_OBJECT

public:

    explicit Program(QWidget *parent = nullptr);

    Program(QString nazwaUzytkownikaLog);

protected:

    ~Program();

private:

    struct Uzytkownik
    {
        int id = 0;
        QString imie = "";
        QString nazwisko = "";
        bool jestPracownikiem = false;
    };

    void PobieranieDanych(QString nazwaUzytkownikaLog, struct Uzytkownik *uzytkownik);

    Ui::Program *ui;

};

#endif // PROGRAM_H
