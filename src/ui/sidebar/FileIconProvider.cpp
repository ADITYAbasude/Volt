#include "FileIconProvider.h"
#include "../../themes/Theme.h"
#include <QPainter>
#include <QPixmap>

FileIconProvider::FileIconProvider()
    : m_iconFont("icons-carbon")
{
    m_iconFont.setPixelSize(16);
    
    // Pre-create common icons
    m_folderIcon = createIconFromFont(Theme::instance().getCarbonIconChar("symbol-folder"), QColor("#dcb67a"));
    m_fileIcon = createIconFromFont(Theme::instance().getCarbonIconChar("symbol-file"), QColor("#CCCCCC"));
}

QIcon FileIconProvider::icon(IconType type) const
{
    switch (type) {
        case Computer:
            return createIconFromFont(Theme::instance().getCarbonIconChar("network-3"), QColor("#CCCCCC"));
        case Desktop:
            return createIconFromFont(Theme::instance().getCarbonIconChar("desktop"), QColor("#CCCCCC"));
        case Trashcan:
            return createIconFromFont(Theme::instance().getCarbonIconChar("trash-can"), QColor("#CCCCCC"));
        case Network:
            return createIconFromFont(Theme::instance().getCarbonIconChar("network-3"), QColor("#CCCCCC"));
        case Drive:
            return createIconFromFont(Theme::instance().getCarbonIconChar("hard-drive"), QColor("#CCCCCC"));
        case Folder:
            return m_folderIcon;
        case File:
        default:
            return m_fileIcon;
    }
}

QIcon FileIconProvider::icon(const QFileInfo &info) const
{
    if (info.isDir()) {
        return m_folderIcon;
    }
    
    QString extension = info.suffix().toLower();
    QString cacheKey = extension.isEmpty() ? "file" : extension;
    
    if (m_iconCache.contains(cacheKey)) {
        return m_iconCache[cacheKey];
    }
    
    QString iconChar = getFileIconChar(extension);
    QColor iconColor = QColor("#CCCCCC");
    
    // Color-code by file type
    if (extension == "cpp" || extension == "c" || extension == "cc" || extension == "cxx") {
        iconColor = QColor("#659ad2");
    } else if (extension == "h" || extension == "hpp" || extension == "hxx") {
        iconColor = QColor("#a074c4");
    } else if (extension == "js" || extension == "ts" || extension == "jsx" || extension == "tsx") {
        iconColor = QColor("#f1e05a");
    } else if (extension == "py") {
        iconColor = QColor("#3572a5");
    } else if (extension == "java") {
        iconColor = QColor("#b07219");
    } else if (extension == "html" || extension == "htm") {
        iconColor = QColor("#e34c26");
    } else if (extension == "css") {
        iconColor = QColor("#563d7c");
    } else if (extension == "json" || extension == "xml") {
        iconColor = QColor("#4b32c3");
    } else if (extension == "md" || extension == "txt") {
        iconColor = QColor("#083fa1");
    } else if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "gif" || extension == "bmp" || extension == "svg") {
        iconColor = QColor("#d4af37");
    } else if (extension == "pdf") {
        iconColor = QColor("#ff2c2d");
    }
    
    QIcon fileIcon = createIconFromFont(iconChar, iconColor);
    m_iconCache[cacheKey] = fileIcon;
    return fileIcon;
}

QIcon FileIconProvider::createIconFromFont(const QString &iconChar, const QColor &color) const
{
    if (iconChar.isEmpty()) {
        return m_fileIcon;
    }
    
    qreal deviceRatio = 1.0;
    QPixmap pixmap(QSize(16, 16) * deviceRatio);
    pixmap.setDevicePixelRatio(deviceRatio);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    
    painter.setFont(m_iconFont);
    painter.setPen(color);
    
    QRect iconRect = pixmap.rect();
    if (deviceRatio > 1.0) {
        iconRect = QRect(0, 0, 16, 16);
    }
    
    painter.drawText(iconRect, Qt::AlignCenter | Qt::AlignVCenter, iconChar);
    
    return QIcon(pixmap);
}

QString FileIconProvider::getFileIconChar(const QString &extension) const
{
    static const QHash<QString, QString> fileTypeIcons = {
        {"cpp", "symbol-file"},
        {"c", "symbol-file"},
        {"cc", "symbol-file"},
        {"cxx", "symbol-file"},
        {"h", "symbol-file"},
        {"hpp", "symbol-file"},
        {"hxx", "symbol-file"},
        {"js", "symbol-file"},
        {"ts", "symbol-file"},
        {"jsx", "symbol-file"},
        {"tsx", "symbol-file"},
        {"py", "symbol-file"},
        {"java", "symbol-file"},
        {"html", "symbol-file"},
        {"htm", "symbol-file"},
        {"css", "symbol-file"},
        {"json", "symbol-file"},
        {"xml", "symbol-file"},
        {"md", "document"},
        {"txt", "document"},
        {"png", "image"},
        {"jpg", "image"},
        {"jpeg", "image"},
        {"gif", "image"},
        {"bmp", "image"},
        {"svg", "image"},
        {"pdf", "document-pdf"}
    };
    
    QString iconType = fileTypeIcons.value(extension, "symbol-file");
    return Theme::instance().getCarbonIconChar(iconType);
}