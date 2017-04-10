#include "Notifications/NotificationWidget.h"

#include <QString>
#include <QTimer>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QPropertyAnimation>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QPushButton>

#include <QPainter>
#include <Qt>

namespace NotificationWidgetDetails
{
static std::vector<std::pair<QMessageBox::Icon, Result::ResultType>> notificationIconsConvertor =
{
  std::make_pair(QMessageBox::Information, Result::RESULT_SUCCESS),
  std::make_pair(QMessageBox::Warning, Result::RESULT_WARNING),
  std::make_pair(QMessageBox::Critical, Result::RESULT_ERROR)
};

QMessageBox::Icon Convert(const Result::ResultType& type)
{
    using IconNode = std::pair<QMessageBox::Icon, Result::ResultType>;
    auto iter = std::find_if(notificationIconsConvertor.begin(), notificationIconsConvertor.end(), [type](const IconNode& node)
                             {
                                 return node.second == type;
                             });
    Q_ASSERT(iter != notificationIconsConvertor.end());
    return iter->first;
}

QString ColorToHTML(const QColor& color)
{
    QString ret = QString("#%1%2%3%4")
                  .arg(color.alpha(), 2, 16, QChar('0'))
                  .arg(color.red(), 2, 16, QChar('0'))
                  .arg(color.green(), 2, 16, QChar('0'))
                  .arg(color.blue(), 2, 16, QChar('0'));
    return ret;
}
} //namespace NotificationWidgetDetails

NotificationWidget::NotificationWidget(const NotificationParams& params, QWidget* parent)
    : QWidget(parent)
{
    //operator | declared in the global namespace
    //without this string compilation will be failed
    using ::operator|;
    Qt::WindowFlags flags = (Qt::FramelessWindowHint | // Disable window decoration
                             Qt::Tool // Discard display in a separate window
                             );
    setWindowFlags(flags);

    setAttribute(Qt::WA_TranslucentBackground); // Indicates that the background will be transparent
    setAttribute(Qt::WA_ShowWithoutActivating); // At the show, the widget does not get the focus automatically

    InitUI(params);

    QDesktopWidget* desktop = QApplication::desktop();
    QRect geometry = desktop->availableGeometry(parent);
    setFixedWidth(geometry.width() / 5);
    setMaximumHeight(geometry.height() / 3);
}

void NotificationWidget::OnCloseButtonClicked()
{
    emit CloseButtonClicked(this);
}

void NotificationWidget::OnDetailsButtonClicked()
{
    emit DetailsButtonClicked(this);
}

void NotificationWidget::InitUI(const NotificationParams& params)
{
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QVBoxLayout* messageLayout = new QVBoxLayout();
    messageLayout->setSpacing(5);
    mainLayout->addItem(messageLayout);

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setSpacing(10);
    messageLayout->addItem(titleLayout);

    QFont currentFont = font();
    QFontMetrics fm(currentFont);
    int fontHeight = fm.height();
    QLabel* iconLabel = new QLabel();
    QMessageBox::Icon icon = NotificationWidgetDetails::Convert(params.message.type);
    QPixmap image = QMessageBox::standardIcon(icon).scaled(QSize(fontHeight, fontHeight), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    iconLabel->setPixmap(image);
    iconLabel->setScaledContents(false);
    titleLayout->addWidget(iconLabel);

    if (params.title.isEmpty() == false)
    {
        QString title = params.title;
        QLabel* labelTitle = new QLabel(title);
        labelTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        labelTitle->setStyleSheet("font-weight: bold;");
        titleLayout->addWidget(labelTitle);
    }

    QString message = params.message.message;
    QLabel* labelMessage = new QLabel(message);
    labelMessage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    labelMessage->setWordWrap(true);
    messageLayout->addWidget(labelMessage);

    QPalette palette;
    QColor baseColor = palette.color(QPalette::Midlight);
    QColor buttonColor = baseColor;
    buttonColor.setAlpha(0);
    QColor pressedColor = baseColor;
    pressedColor.setAlpha(255);

    QString styleSheet = QString("QPushButton {"
                                 "border-radius: 1px;"
                                 "background-color: " +
                                 NotificationWidgetDetails::ColorToHTML(buttonColor) + ";"
                                                                                       "padding: 5px;"
                                                                                       "}"
                                                                                       "QPushButton:pressed {"
                                                                                       "background-color: " +
                                 NotificationWidgetDetails::ColorToHTML(pressedColor) + ";"
                                                                                        "}");

    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(5);
    {
        closeButton = new QPushButton(tr("Close"));
        closeButton->setObjectName("CloseButton");
        closeButton->setStyleSheet(styleSheet);
        closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
        buttonsLayout->addWidget(closeButton);
        connect(closeButton, &QPushButton::clicked, this, &NotificationWidget::OnCloseButtonClicked);
    }
    if (params.callback)
    {
        detailsButton = new QPushButton(params.detailsButtonText);
        detailsButton->setObjectName("DetailsButton");
        detailsButton->setStyleSheet(styleSheet);
        detailsButton->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
        buttonsLayout->addWidget(detailsButton);
        connect(detailsButton, &QPushButton::clicked, this, &NotificationWidget::OnDetailsButtonClicked);
    }
    mainLayout->addItem(buttonsLayout);
    setLayout(mainLayout);
}

void NotificationWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect roundedRect;
    const int radius = 10;
    roundedRect.setX(rect().x() + radius / 2);
    roundedRect.setY(rect().y() + radius / 2);
    roundedRect.setWidth(rect().width() - radius);
    roundedRect.setHeight(rect().height() - radius);

    QPalette palette;
    QColor rectColor = palette.color(QPalette::Window);
    painter.setBrush(QBrush(rectColor));
    QPen roundedRectPen(Qt::black);
    painter.setPen(roundedRectPen);

    painter.drawRoundedRect(roundedRect, radius, radius);

    QRect closeButtonGeometry = closeButton->geometry();
    QColor lineColor = palette.color(QPalette::Text);
    QPen pen(lineColor);
    pen.setWidth(1);
    painter.setPen(pen);
    //horizontal line
    if (detailsButton != nullptr)
    {
        QRect detailsButtonGeometry = detailsButton->geometry();
        int y = (closeButtonGeometry.bottom() + detailsButtonGeometry.top()) / 2;
        QPoint left(qMin(closeButtonGeometry.left(), detailsButtonGeometry.left()), y);
        QPoint right(qMax(closeButtonGeometry.right(), detailsButtonGeometry.right()), y);
        painter.drawLine(left, right);
    }

    //vertical line
    //close button and details button have Preferred size policy
    int x = closeButtonGeometry.left() - pen.width();
    QPoint top(x, roundedRect.top() + roundedRectPen.width());
    QPoint bottom(x, roundedRect.bottom() - roundedRectPen.width());
    painter.drawLine(top, bottom);
}
