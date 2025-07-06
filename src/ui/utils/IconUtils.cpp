#include "IconUtils.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>

namespace IconUtils {
QPixmap loadSvgIcon(const QString &iconPath, int size) {
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);
    QSvgRenderer renderer(iconPath);
    QPainter painter(&pixmap);
    renderer.render(&painter);
    return pixmap;
}
}
