#include "CustomTabBar.h"
#include <QPainter>
#include <QStyleOptionTab>
#include "themes/Theme.h"

CustomTabBar::CustomTabBar(QWidget *parent)
    : QTabBar(parent)
{
    setDrawBase(false);
    setExpanding(true);
    setElideMode(Qt::ElideNone);
    setIconSize(QSize(24, 24));
}

QSize CustomTabBar::tabSizeHint(int index) const
{
    QSize size = QTabBar::tabSizeHint(index);
    int dim = qMax(32, qMax(size.width(), size.height()));
    return QSize(dim, dim);
}

void CustomTabBar::drawBottomBorder(QPainter &p, const QRect &rect, QColor &color, qreal thickness, int horizontalPadding)
{
    p.save();
    QPen pen(color);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::RoundJoin);
    // thickness is in device-independent pixels, but scale it by devicePixelRatioF for physical pixels
    const qreal dpr = p.device()->devicePixelRatioF();
    pen.setWidthF(thickness * dpr);
    pen.setCosmetic(true);
    p.setPen(pen);

    qreal y = rect.bottom() - (pen.widthF() / (2.0 * dpr));
    QPointF left(rect.left() + horizontalPadding, y);
    QPointF right(rect.right() - horizontalPadding, y);
    p.drawLine(left, right);
    p.restore();
}

void CustomTabBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QColor primary = Theme::instance().getColor("primary");

    for (int i = 0; i < count(); ++i)
    {
        QStyleOptionTab tabOpt;
        initStyleOption(&tabOpt, i);

        QRect tabRect = this->tabRect(i);
        bool isSelected = (tabOpt.state & QStyle::State_Selected);

        QStyleOptionTab drawOpt;
        initStyleOption(&drawOpt, i);
        drawOpt.rect = tabRect;
        style()->drawControl(QStyle::CE_TabBarTabShape, &drawOpt, &painter, this);

        QStyleOptionTab labelOpt = drawOpt;
        labelOpt.icon = QIcon();
        style()->drawControl(QStyle::CE_TabBarTabLabel, &labelOpt, &painter, this);

        // Draw cached icon
        QIcon cachedIcon = getCachedIcon(i, isSelected, false);
        if (!cachedIcon.isNull())
        {
            QSize iconSize = this->iconSize();
            qreal deviceRatio = devicePixelRatio();

            QPixmap pixmap = cachedIcon.pixmap(iconSize * deviceRatio);
            pixmap.setDevicePixelRatio(deviceRatio);

            QPoint topLeft = tabRect.center() - QPoint(pixmap.width() / (2 * deviceRatio),
                                                       pixmap.height() / (2 * deviceRatio));
            painter.drawPixmap(topLeft, pixmap);
        }

        if (isSelected)
        {
            QColor borderColor(primary);
            drawBottomBorder(painter, tabRect, borderColor, 1.0);
        }
    }
}

QIcon CustomTabBar::getCachedIcon(int index, bool isSelected, bool isHovered) const
{
    QIcon originalIcon = tabIcon(index);
    if (originalIcon.isNull())
    {
        return QIcon();
    }

    QString cacheKey = QString("tab_%1_%2_%3_%4")
                           .arg(index)
                           .arg(isSelected ? "selected" : "normal")
                           .arg(isHovered ? "hovered" : "normal")
                           .arg(devicePixelRatio(), 0, 'f', 1);

    if (m_iconCache.contains(cacheKey))
    {
        return m_iconCache[cacheKey];
    }

    QIcon::Mode iconMode = isSelected ? QIcon::Selected : (isHovered ? QIcon::Active : QIcon::Normal);

    // Create cached version of the icon
    QIcon cachedIcon = originalIcon;
    m_iconCache[cacheKey] = cachedIcon;

    return cachedIcon;
}
