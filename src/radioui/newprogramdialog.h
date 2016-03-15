#pragma once

#include <QDialog>

class RadioStub ;
class RadioConsole;

namespace Ui {
class NewProgramDialog;
}

class NewProgramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProgramDialog(RadioStub &stub, RadioConsole &main, QWidget *parent = 0);
    ~NewProgramDialog();

signals:
    void startProgram(int id);

private slots:
    void on_buttonBox_accepted();

public slots:

    void programListReady(QStringList list);

private:
    Ui::NewProgramDialog *ui;
    RadioStub &m_stub;
    RadioConsole &m_main;
};
