#ifndef CONNECTINGDIALOG_H
#define CONNECTINGDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectingDialog;
}

class ConnectingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectingDialog(QWidget *parent = 0);
    ~ConnectingDialog();

private:
    Ui::ConnectingDialog *ui;
};

#endif // CONNECTINGDIALOG_H
