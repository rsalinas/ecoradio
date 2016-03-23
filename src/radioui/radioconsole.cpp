#include "radioconsole.h"

#include "ui_radioconsole.h"

#include <QDebug>
#include <QDateTime>
#include <QtGlobal>
#include <QAbstractTableModel>

#include "newprogramdialog.h"

RadioConsole::RadioConsole(RadioStub * stub, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RadioConsole),
    m_wallclockTimer(new QTimer(this)),
    m_stub(stub)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    qDebug() << qRegisterMetaType<ProgramTime>();
    qDebug() << qRegisterMetaType<std::shared_ptr<ProgramTime> >();
    qDebug() << qRegisterMetaType<QList<std::shared_ptr<ProgramTime>> >();

    ui->setupUi(this);
    updateClock();
    (connect(m_wallclockTimer, SIGNAL(timeout()), this, SLOT(updateClock())));
    m_wallclockTimer->start(1000);
    (QObject::connect(m_stub, SIGNAL(vuMeterUpdate(int,int)), this, SLOT(vuMeterUpdate(int, int))));

    (QObject::connect(m_stub, SIGNAL(newProgram(std::shared_ptr<ProgramTime> ,
                                                 QList<std::shared_ptr<ProgramTime>>)),
                      this, SLOT(newProgram(std::shared_ptr<ProgramTime>,
                                            QList<std::shared_ptr<ProgramTime>>))));
    (QObject::connect(m_stub, SIGNAL(currentSong(QString)), this, SLOT(currentSong(QString))));
    (QObject::connect(m_stub, SIGNAL(nextSong(QString)), this, SLOT(nextSong(QString))));
    (QObject::connect(m_stub, SIGNAL(currentPos(float, float)), this, SLOT(currentPos(float, float))));
    ui->vumMain->setMaximum(255);
    ui->vumMain->setMinimum(0);
    ui->vumMain->setFormat("");

    ui->currentPosSlider->setTickInterval(100);
}

RadioConsole::~RadioConsole()
{
    qDebug() << __FUNCTION__;
    delete ui;
}




void RadioConsole::on_startProgramButton_clicked()
{
    qDebug() << __FUNCTION__;
    NewProgramDialog * d = new NewProgramDialog(*m_stub, *this, this);    
    QObject::connect(d, SIGNAL(startProgram(ProgramTime,QString,int)), m_stub, SLOT(startProgram(ProgramTime,QString,int)));
    d->setModal(true);
    d->show();
    d->raise();
    d->activateWindow();
}

void RadioConsole::on_endProgramButton_clicked()
{
    qDebug() << __FUNCTION__;
    m_stub->endProgram();
}

void RadioConsole::updateClock() {
    ui->currentTimeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void RadioConsole::on_pauseButton_clicked()
{
    m_stub->pause();
}

void RadioConsole::on_resumeButton_clicked()
{
    m_stub->resume();
}


void RadioConsole::on_pttButton_pressed()
{
    m_stub->ptt(true);
}

void RadioConsole::on_pttButton_released()
{
    m_stub->ptt(false);
}

void RadioConsole::on_skipButton_clicked()
{
    m_stub->skipSong();
}


void RadioConsole::vuMeterUpdate(int channel, int value) {
    //    qDebug() << "vumeter update" << channel<< value;
    ui->vumMain->setValue(value*100/SHRT_MAX);
}




void RadioConsole::newProgram(const std::shared_ptr<ProgramTime> current,
                              const QList<std::shared_ptr<ProgramTime>> nextPrograms) {
    qDebug() << __FUNCTION__ << "!!!"<< *current << nextPrograms.size();
    m_current = current;
    m_nextPrograms = nextPrograms;
    ui->statusBar->showMessage(current->ts.toString("hh:mm:ss ")+ current->name);
    ui->programList->clear();
    for (auto s : nextPrograms) {
        ui->programList->addItem(s->name);
    }
}

void RadioConsole::currentSong(QString currentSong) {
    ui->currentSongEdit->setText(currentSong);
}

void RadioConsole::nextSong(QString nextSong) {
    ui->nextSongEdit->setText(nextSong);
}

void RadioConsole::currentPos(float pos, float total) {

    auto value = int(pos*100/total);
    //    qDebug() << "current pos client: "<< pos << value;
    ui->currentPosSlider->setValue(value);
    QTime postime = QTime::fromMSecsSinceStartOfDay(int(pos*1000));
    QTime totaltime = QTime::fromMSecsSinceStartOfDay(int(total*1000));
    ui->currentPosEdit->setText(postime.toString("hh:mm:ss")
                                + "/" + totaltime.toString("hh:mm:ss"));
}


QList<ProgramTime> RadioConsole::getPrograms() {
    QList<ProgramTime> ret;
    for (auto p : m_nextPrograms) {
        ret.push_back(*p);
    }
    qDebug() << "programasn" << ret.size();

    return ret;
}


void RadioConsole::on_actionConfigure_triggered()
{
    close();
}


#include "radioconsole.moc"
