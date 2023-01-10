#pragma once

#include "NotificationWidget.h"

#include <QObject>
#include <QMap>
#include <QElapsedTimer>
#include <QBasicTimer>

class NotificationLayout : public QObject
{
    Q_OBJECT

public:
    NotificationLayout();
    ~NotificationLayout() override;

    void AddNotificationWidget(QWidget* parent, const NotificationParams& params);
    void SetLayoutType(Qt::Alignment align);
    void SetDisplayTimeMs(int displayTimeMS);

private slots:
    void OnCloseClicked(NotificationWidget* notification);
    void OnDetailsClicked(NotificationWidget* notification);
    void OnWidgetDestroyed();
    void OnParentWidgetDestroyed();

private:
    void LayoutWidgets(QWidget* parent);
    void Clear();

    bool eventFilter(QObject* object, QEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

    struct NotificationWidgetParams;
    using NotificationPair = std::pair<NotificationWidget*, NotificationWidgetParams>;
    using WindowNotifications = std::vector<NotificationPair>;
    using AllNotifications = QMap<QWidget*, WindowNotifications>;
    AllNotifications notifications;

    Qt::Alignment layoutType = Qt::AlignTop | Qt::AlignRight;

    int displayTimeMs = 10000;
    const int maximumDisplayCount = 5;
    QElapsedTimer elapsedTimer;
    QBasicTimer basicTimer;
};
