#include "newprogramdialog.h"

#include "ui_newprogramdialog.h"

#include "wsstub/radiostub.h"
#include "radioconsole.h"

class MyModel : public QAbstractTableModel {
    Q_OBJECT
public:
    MyModel(const QList<Program> &programs,
            QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    const QList<Program> m_programs;
};


NewProgramDialog::NewProgramDialog(RadioStub &stub, RadioConsole &main, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewProgramDialog)
    , m_stub(stub)
    , m_main(main)
{
    ui->setupUi(this);

    //    ui->programList->clear();
    for (const auto &l : main.getPrograms()) {
        //        ui->programList->addItem(l.name);
    }
    MyModel * m = new MyModel(main.getPrograms(), this);
    ui->programTableView->setModel(m);
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
    //    emit startProgram(ui->programTableView->currentIndex());
}


void NewProgramDialog::programListReady(QStringList list)
{
    //    ui->programList->clear();
    //    for (const auto &l : list) {
    //        ui->programList->addItem(l);;
    //    }
}



MyModel::MyModel(const QList<Program> &programs,
                 QObject *parent)
    : QAbstractTableModel(parent)
    , m_programs(programs)
{

}

int MyModel::rowCount(const QModelIndex &parent ) const
{
    return m_programs.size();

}
int MyModel::columnCount(const QModelIndex &parent ) const
{
    return 2;
}
QVariant MyModel::data(const QModelIndex &index, int role ) const
{
//    qDebug() << __FUNCTION__ << index << role;
    switch (role) {
    case Qt::DisplayRole:

        switch (index.column()) {
        case 0:
            return m_programs[index.row()].name;
        case 1:
            return m_programs[index.row()].ts;
        }

    case Qt::ToolTipRole:
        return m_programs[index.row()].name;
    }
    return QVariant();
}

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const {
//    qDebug() << __FUNCTION__ << section << orientation << role;
    if (role == Qt::DisplayRole)
       {
           if (orientation == Qt::Horizontal) {
               switch (section)
               {
               case 0:
                   return QString("Name");
               case 1:
                   return QString("Ts");
               }
           }
       }
       return QVariant();
}

#include "newprogramdialog.moc"


