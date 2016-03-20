#pragma once

#include <QMainWindow>
#include <QWidget>


namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();

private:
    Ui::ConnectDialog *ui;
};
	
