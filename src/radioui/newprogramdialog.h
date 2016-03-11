#ifndef NEWPROGRAMDIALOG_H
#define NEWPROGRAMDIALOG_H

#include <QDialog>
class RadioStub ;

namespace Ui {
class NewProgramDialog;
}

class NewProgramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProgramDialog(RadioStub &stub, QWidget *parent = 0);
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
};

#endif // NEWPROGRAMDIALOG_H
