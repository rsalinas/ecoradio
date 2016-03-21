#include "connectdialog.h"
#include "ui_connectdialog.h"

#include <QDebug>
#include "wsstub/radiostub.h"
#include "radioconsole.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

void ConnectDialog::on_pushButton_clicked()
{
    qDebug() << __FUNCTION__;
    QUrl url(ui->urlEdit->text());
    m_stub  = new RadioStub(url, this);
    QObject::connect(m_stub, SIGNAL(connected()), this, SLOT(onConnected()));
}


void ConnectDialog::onConnected() {
    m_console = new RadioConsole(m_stub);
    m_stub->setParent(m_console);
    m_stub = nullptr;
    m_console->show();
//    QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), this, SLOT(show()));
    QObject::connect(m_stub, SIGNAL(destroyed(QObject*)), this, SLOT(show()));
//    m_stub = nullptr;
    this->hide();    
}
