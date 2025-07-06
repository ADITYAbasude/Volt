#pragma once
#include <QPixmap>
#include <QString>

namespace IconUtils {
    // Loads an SVG icon from the given path and returns a QPixmap of the specified size
    QPixmap loadSvgIcon(const QString &iconPath, int size = 24);
}
