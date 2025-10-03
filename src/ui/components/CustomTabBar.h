#pragma once

#include <QTabBar>
#include <QHash>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize tabSizeHint(int index) const override;

private:
    QIcon getCachedIcon(int index, bool isSelected, bool isHovered) const;
    static void drawBottomBorder(QPainter &painter, const QRect &tabRect, QColor &color, qreal thickness, int horizontalPadding = 0);
    mutable QHash<QString, QIcon> m_iconCache;
};
