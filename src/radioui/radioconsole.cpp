#include "radioconsole.h"

#include "ui_radioconsole.h"

#include <QDebug>
#include <QDateTime>

#include "newprogramdialog.h"

RadioConsole::RadioConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RadioConsole),
    m_wallclockTimer(new QTimer(this)),
    m_stub(QUrl("ws://localhost:1234")) //FIXME
{
    ui->setupUi(this);
    updateClock();
    QObject::connect(m_wallclockTimer, SIGNAL(timeout()), this, SLOT(updateClock()));
    m_wallclockTimer->start(1000);
    QObject::connect(&m_stub, SIGNAL(vuMeterUpdate(int,int)), this, SLOT(vuMeterUpdate(int, int)));
    QObject::connect(&m_stub, SIGNAL(newProgram(QString,QStringList)), this, SLOT(newProgram(QString,QStringList)));
    QObject::connect(&m_stub, SIGNAL(currentSong(QString)), this, SLOT(currentSong(QString)));
    QObject::connect(&m_stub, SIGNAL(nextSong(QString)), this, SLOT(nextSong(QString)));
    ui->vumMain->setMaximum(255);
    ui->vumMain->setMinimum(0);
    ui->vumMain->setFormat("");
}

RadioConsole::~RadioConsole()
{
    delete ui;
}

void RadioConsole::startProgram(int id) {
    qDebug() << __FUNCTION__ << id;
    m_stub.startProgram(id);
}


void RadioConsole::on_startProgramButton_clicked()
{
    qDebug() << __FUNCTION__;
    NewProgramDialog * d = new NewProgramDialog(m_stub, this);
    QObject::connect(d, SIGNAL(startProgram(int)), this, SLOT(startProgram(int)));
    d->setModal(true);
    d->show();
    d->raise();
    d->activateWindow();
}

void RadioConsole::on_endProgramButton_clicked()
{
    qDebug() << __FUNCTION__;
    m_stub.endProgram();
}

void RadioConsole::updateClock() {
    ui->currentTimeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void RadioConsole::on_pauseButton_clicked()
{
    m_stub.pause();
}

void RadioConsole::on_resumeButton_clicked()
{
    m_stub.resume();
}


void RadioConsole::on_pttButton_pressed()
{
    m_stub.ptt(true);
}

void RadioConsole::on_pttButton_released()
{
    m_stub.ptt(false);
}

void RadioConsole::on_skipButton_clicked()
{
    m_stub.skipSong();
}


void RadioConsole::vuMeterUpdate(int channel, int value) {
    //    qDebug() << "vumeter update" << channel<< value;
    ui->vumMain->setValue(value*100/SHRT_MAX);
}


void RadioConsole::newProgram(QString current, QStringList nextPrograms) {
    qDebug() << __FUNCTION__;
    ui->statusBar->showMessage(current);
    ui->programList->clear();
    for (auto s : nextPrograms) {
        ui->programList->addItem(s);
    }
}

void RadioConsole::currentSong(QString currentSong) {
    ui->currentSongEdit->setText(currentSong);
}

void RadioConsole::nextSong(QString nextSong) {
    ui->nextSongEdit->setText(nextSong);
}
