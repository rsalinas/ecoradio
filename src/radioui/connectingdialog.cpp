#include "connectingdialog.h"
#include "ui_connectingdialog.h"

ConnectingDialog::ConnectingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectingDialog)
{
    ui->setupUi(this);
}

ConnectingDialog::~ConnectingDialog()
{
    delete ui;
}
