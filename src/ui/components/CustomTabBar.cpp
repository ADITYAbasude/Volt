#include "CustomTabBar.h"
#include <QPainter>
#include <QStyleOptionTab>

CustomTabBar::CustomTabBar(QWidget *parent)
    : QTabBar(parent)
{
    setDrawBase(false);
    setExpanding(false);
    setElideMode(Qt::ElideNone);
    setIconSize(QSize(24, 24));
}

QSize CustomTabBar::tabSizeHint(int index) const
{
    QSize size = QTabBar::tabSizeHint(index);
    int dim = qMax(32, qMax(size.width(), size.height()));
    return QSize(dim, dim);
}

void CustomTabBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);


    QStyleOptionTab opt;
    for (int i = 0; i < count(); ++i)
    {
        QStyleOptionTab backOpt;
        initStyleOption(&backOpt, i);

        backOpt.icon = QIcon();
        backOpt.text = QString();
        
        QRect backRect = tabRect(i);
        backOpt.rect = backRect;
        
        style()->drawControl(QStyle::CE_TabBarTabShape, &backOpt, &painter, this);
        style()->drawControl(QStyle::CE_TabBarTabLabel, &backOpt, &painter, this);

        QIcon icon = tabIcon(i);
        if (!icon.isNull())
        {
            QSize iconSize = this->iconSize();

            qreal deviceRatio = devicePixelRatio();
            QPixmap pixmap = icon.pixmap(iconSize * deviceRatio);
            pixmap.setDevicePixelRatio(deviceRatio);

            // Center the icon in the tab
            QPoint topLeft = backRect.center() - QPoint(pixmap.width() / (2 * deviceRatio),
                                                        pixmap.height() / (2 * deviceRatio));
            painter.drawPixmap(topLeft, pixmap);
        }
    }
}
