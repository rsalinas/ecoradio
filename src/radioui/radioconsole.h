#ifndef RADIOCONSOLE_H
#define RADIOCONSOLE_H

#include <QMainWindow>

#include <QTimer>

#include "radiostub.h"

namespace Ui {
class RadioConsole;
}

class RadioConsole : public QMainWindow
{
    Q_OBJECT

public:
    explicit RadioConsole(QWidget *parent = 0);
    ~RadioConsole();

private slots:    

    void on_startProgramButton_clicked();
    void on_endProgramButton_clicked();
    void updateClock();
    void on_pauseButton_clicked();
    void on_resumeButton_clicked();
    void on_pttButton_pressed();

    void on_pttButton_released();

    void on_skipButton_clicked();

public slots:
    void startProgram(int id);
    void vuMeterUpdate(int channel, int value);

private:
    Ui::RadioConsole *ui;
    QTimer * m_wallclockTimer;
    RadioStub m_stub;
};

#endif // RADIOCONSOLE_H