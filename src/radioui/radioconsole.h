#ifndef RADIOCONSOLE_H
#define RADIOCONSOLE_H

#include <QMainWindow>
#include <QTimer>
#include <QList>

#include "wsstub/radiostub.h"

namespace Ui {
class RadioConsole;
}

class RadioConsole : public QMainWindow
{
    Q_OBJECT

public:
    explicit RadioConsole(RadioStub * stub, QWidget *parent = nullptr);
    ~RadioConsole();

    QList<ProgramTime> getPrograms();

private slots:
    void on_startProgramButton_clicked();
    void on_endProgramButton_clicked();
    void updateClock();
    void on_pauseButton_clicked();
    void on_resumeButton_clicked();
    void on_pttButton_pressed();
    void on_pttButton_released();
    void on_skipButton_clicked();
    void newProgram(std::shared_ptr<ProgramTime> current,
                    QList<std::shared_ptr<ProgramTime>> nextPrograms);
    void currentSong(QString currentSong);
    void nextSong(QString nextSong);
    void vuMeterUpdate(int channel, int value);
    void currentPos(float pos, float total);


    /* test */

    void on_actionConfigure_triggered();

signals:
    void test0signal(QList<std::shared_ptr<ProgramTime>> p);
    void test1signal(int);
public slots:
    void test0slot(QList<std::shared_ptr<ProgramTime>> p) {}
    void test1slot(int) {}

    /* end test */


private:
    Ui::RadioConsole *ui;
    QTimer * m_wallclockTimer;
    RadioStub * m_stub;
    std::shared_ptr<ProgramTime> m_current;
    QList<std::shared_ptr<ProgramTime>> m_nextPrograms;
};

#endif // RADIOCONSOLE_H
