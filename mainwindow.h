#pragma once

#include "Notifications/NotificationLayout.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_success_clicked();
    void on_pushButton_error_clicked();
    
private:
    Ui::MainWindow *ui;
    NotificationLayout notificationLayout;
};
