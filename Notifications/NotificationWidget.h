#pragma once

#include "Result/Result.h"

#include <QWidget>

#include <functional>

class QPaintEvent;
class QPushButton;

struct NotificationParams
{
    Result message;
    QString title;
    QString detailsButtonText = "Details";
    std::function<void()> callback;
};

class NotificationWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint position READ pos WRITE move)
    Q_PROPERTY(float opacity READ windowOpacity WRITE setWindowOpacity)

public:
    explicit NotificationWidget(const NotificationParams& params, QWidget* parent = 0);

signals:
    void CloseButtonClicked(NotificationWidget* current);
    void DetailsButtonClicked(NotificationWidget* current);

private slots:
    void OnCloseButtonClicked();
    void OnDetailsButtonClicked();

private:
    void InitUI(const NotificationParams& params);

    void paintEvent(QPaintEvent* event) override;

    QPushButton* closeButton = nullptr;
    QPushButton* detailsButton = nullptr;
};
