#include "Notifications/NotificationLayout.h"
#include "Notifications/NotificationWidget.h"

#include <QEvent>
#include <QPropertyAnimation>

struct NotificationLayout::NotificationWidgetParams
{
    void InitAnimation(NotificationWidget* target)
    {
        positionAnimation = new QPropertyAnimation(target, "position", target);
        const int durationTimeMs = 120;
        positionAnimation->setDuration(durationTimeMs);

        opacityAnimation = new QPropertyAnimation(target, "opacity", target);
        opacityAnimation->setDuration(durationTimeMs * 2);
    }

    void DecrementTime(int elapsedMs)
    {
        if (elapsedMs > remainTimeMs)
        {
            remainTimeMs = 0;
        }
        else
        {
            remainTimeMs -= elapsedMs;
        }
    }

    std::function<void()> callback;
    int remainTimeMs = 0;
    QPropertyAnimation* positionAnimation = nullptr;
    QPropertyAnimation* opacityAnimation = nullptr;
};

NotificationLayout::NotificationLayout()
    : QObject(nullptr)
{
    basicTimer.start(60, this);
}

NotificationLayout::~NotificationLayout()
{
    Clear();
}

void NotificationLayout::AddNotificationWidget(QWidget* parent, const NotificationParams& params)
{
    if (notifications.contains(parent) == false)
    {
        parent->installEventFilter(this);
        connect(parent, &QObject::destroyed, this, &NotificationLayout::OnParentWidgetDestroyed);
    }

    NotificationWidget* widget = new NotificationWidget(params, parent);
    connect(widget, &NotificationWidget::CloseButtonClicked, this, &NotificationLayout::OnCloseClicked);
    connect(widget, &NotificationWidget::DetailsButtonClicked, this, &NotificationLayout::OnDetailsClicked);
    connect(widget, &QObject::destroyed, this, &NotificationLayout::OnWidgetDestroyed);

    NotificationWidgetParams widgetParams;
    widgetParams.InitAnimation(widget);

    widgetParams.callback = std::move(params.callback);
    widgetParams.remainTimeMs = displayTimeMs;

    notifications[parent].emplace_back(widget, widgetParams);

    LayoutWidgets(parent);
}

void NotificationLayout::LayoutWidgets(QWidget* parent)
{
    int totalHeight = 0;
    Q_ASSERT(notifications.contains(parent));
    WindowNotifications& widgets = notifications[parent];

    int size = std::min(static_cast<int>(widgets.size()), maximumDisplayCount);
    std::vector<NotificationPair> widgetsToDisplay(widgets.begin(), widgets.begin() + size);
    if (layoutType & Qt::AlignTop)
    {
        std::reverse(widgetsToDisplay.begin(), widgetsToDisplay.end());
    }

    for (const NotificationPair& pair : widgetsToDisplay)
    {
        NotificationWidget* widget = pair.first;
        bool justCreated = false;
        if (widget->isVisible() == false)
        {
            justCreated = true;
            QPropertyAnimation* opacityAnimation = pair.second.opacityAnimation;
            widget->show();
            opacityAnimation->setStartValue(0.0f);
            opacityAnimation->setEndValue(1.0f);
            opacityAnimation->start();
        }

        int x = (layoutType & Qt::AlignLeft) ? 0 : (parent->width() - widget->width());
        int y = (layoutType & Qt::AlignTop) ? totalHeight : (parent->height() - widget->height() - totalHeight);
        QPoint widgetPos(x, y);

        //noticationWidget marked as window inside Qt, in this case we need to use global coordinates
        //if not mark it as window - on OS X notification will be behind from RenderWidget
        widgetPos = parent->mapToGlobal(widgetPos);

        if (justCreated)
        {
            widget->move(widgetPos);
        }
        else
        {
            QPropertyAnimation* positionAnimation = pair.second.positionAnimation;
            positionAnimation->stop();
            positionAnimation->setStartValue(widget->pos());
            positionAnimation->setEndValue(widgetPos);
            positionAnimation->start();
        }

        totalHeight += widget->size().height();
    }
}

void NotificationLayout::Clear()
{
    for (WindowNotifications& widgets : notifications)
    {
        for (WindowNotifications::iterator iter = widgets.begin(); iter != widgets.end(); ++iter)
        {
            QWidget* widget = iter->first;
            disconnect(widget, &QObject::destroyed, this, &NotificationLayout::OnWidgetDestroyed);
            delete widget;
        }
    }
    notifications.clear();
}

bool NotificationLayout::eventFilter(QObject* object, QEvent* event)
{
    QEvent::Type type = event->type();
    QWidget* sender = qobject_cast<QWidget*>(object);
    if (type == QEvent::Resize || type == QEvent::Move)
    {
        LayoutWidgets(sender);
    }
    return QObject::eventFilter(object, event);
}

void NotificationLayout::timerEvent(QTimerEvent* /*event*/)
{
    int elapsedMs = elapsedTimer.restart();
    for (AllNotifications::Iterator iter = notifications.begin(); iter != notifications.end(); ++iter)
    {
        QWidget* parent = iter.key();
        if (parent->isActiveWindow())
        {
            WindowNotifications& widgets = iter.value();
            for (WindowNotifications::iterator iter = widgets.begin(); iter != widgets.end();)
            {
                NotificationWidget* widget = iter->first;
                NotificationWidgetParams& params = iter->second;
                params.DecrementTime(elapsedMs);
                if (params.remainTimeMs == 0)
                {
                    iter = widgets.erase(iter);
                    disconnect(widget, &QObject::destroyed, this, &NotificationLayout::OnWidgetDestroyed);
                    delete widget;
                }
                else
                {
                    ++iter;
                }
            }
            LayoutWidgets(parent);
            return;
        }
    }
}

void NotificationLayout::SetLayoutType(Qt::Alignment type)
{
    if (layoutType == type)
    {
        return;
    }

    Q_ASSERT((type & Qt::AlignLeft || type & Qt::AlignRight) &&
             (type & Qt::AlignBottom) || (type & Qt::AlignTop));

    layoutType = type;

    //now remove all notifications
    Clear();
}

void NotificationLayout::SetDisplayTimeMs(int displayTimeMs_)
{
    displayTimeMs = displayTimeMs_;   
}

void NotificationLayout::OnCloseClicked(NotificationWidget* notification)
{
    delete notification;
}

void NotificationLayout::OnDetailsClicked(NotificationWidget* notification)
{
    QWidget* parent = notification->parentWidget();
    Q_ASSERT(notifications.contains(parent));
    WindowNotifications& widgets = notifications[parent];
    WindowNotifications::iterator iter = std::find_if(widgets.begin(), widgets.end(), [notification](const NotificationPair& pair) {
        return pair.first == notification;
    });
    Q_ASSERT(iter != widgets.end());
    iter->second.callback();

    delete notification;
}

void NotificationLayout::OnWidgetDestroyed()
{
    NotificationWidget* notification = static_cast<NotificationWidget*>(sender());
    for (AllNotifications::Iterator iter = notifications.begin(); iter != notifications.end(); ++iter)
    {
        WindowNotifications& widgets = *iter;
        WindowNotifications::iterator widgetsIter = std::find_if(widgets.begin(), widgets.end(), [notification](const NotificationPair& pair) {
            return pair.first == notification;
        });
        if (widgetsIter != widgets.end())
        {
            widgets.erase(widgetsIter);
            LayoutWidgets(iter.key());
            return;
        }
    }
}

void NotificationLayout::OnParentWidgetDestroyed()
{
    QWidget* senderWidget = static_cast<QWidget*>(sender());
    notifications.remove(senderWidget);
}
