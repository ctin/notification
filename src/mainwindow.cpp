#include "Result/Result.h"
#include "Operation/Operation.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolTip>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_success_clicked()
{
    NotificationParams params;
    params.title = "Trying to find existed file";
    params.message = Operation::DoSomething(Result::RESULT_SUCCESS);
    notificationLayout.AddNotificationWidget(this, params);
}

void MainWindow::on_pushButton_error_clicked()
{
    NotificationParams params;
    params.title = "Trying to find not existed file";
    params.message = Operation::DoSomething(Result::RESULT_ERROR);
    params.detailsButtonText = "Try again";
    params.callback = [this](){
        on_pushButton_success_clicked();
    };
    notificationLayout.AddNotificationWidget(this, params);
}
