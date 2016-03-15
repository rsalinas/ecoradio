#include "newprogramdialog.h"
#include "ui_newprogramdialog.h"

#include "wsstub/radiostub.h"
#include "radioconsole.h"

NewProgramDialog::NewProgramDialog(RadioStub &stub, RadioConsole &main, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewProgramDialog)
    , m_stub(stub)
    , m_main(main)
{
    ui->setupUi(this);

//    ui->programList->clear();
    for (const auto &l : main.getPrograms()) {
        ui->programList->addItem(l.name);
    }
//    QObject::connect(&m_stub, SIGNAL(programListReady(QStringList)),
//                     this, SLOT(programListReady(QStringList)));
//    m_stub.getPrograms();

}

NewProgramDialog::~NewProgramDialog()
{
    delete ui;
}

void NewProgramDialog::on_buttonBox_accepted()
{
    emit startProgram(ui->programList->currentRow());
}


void NewProgramDialog::programListReady(QStringList list)
{
    ui->programList->clear();
    for (const auto &l : list) {
        ui->programList->addItem(l);;
    }
}
