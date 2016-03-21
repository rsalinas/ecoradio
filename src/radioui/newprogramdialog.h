#pragma once

#include <QDialog>

#include "common/program.h"

class RadioStub;
class RadioConsole;

namespace Ui {
class NewProgramDialog;
}

class MyModel;

class NewProgramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProgramDialog(RadioStub &stub, RadioConsole &main, QWidget *parent = 0);
    ~NewProgramDialog();

signals:
    void startProgram(ProgramTime program, QString title, int delay);

private slots:
    void on_buttonBox_accepted();

public slots:

    void programListReady(QStringList list);

private:
    Ui::NewProgramDialog *ui;
    RadioStub &m_stub;
    RadioConsole &m_main;
    MyModel * m;
};
