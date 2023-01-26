#ifndef PROGRAM_H
#define PROGRAM_H

#include <QDialog>

namespace Ui {
class Program;
}

class Program : public QDialog
{
    Q_OBJECT

public:
    explicit Program(QWidget *parent = nullptr);
    ~Program();

private:
    Ui::Program *ui;
};

#endif // PROGRAM_H
