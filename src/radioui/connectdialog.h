#pragma once

#include <QMainWindow>
#include <QWidget>

class RadioStub;
class RadioConsole;

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();

private slots:
    void on_pushButton_clicked();
    void onConnected();

private:
    Ui::ConnectDialog *ui;

    RadioStub * m_stub = nullptr;
    RadioConsole * m_console = nullptr;
};
	
