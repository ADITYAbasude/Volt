#include "IconButton.h"
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"

#include <QApplication>

IconButton::IconButton(QWidget *parent)
    : QPushButton(parent)
    , m_iconSize(24, 24)
    , m_buttonSize(32, 32)
    , m_useThemeColor(true)
{
    setupButton();
}

IconButton::IconButton(const QString &iconPath, QWidget *parent)
    : QPushButton(parent)
    , m_iconPath(iconPath)
    , m_iconSize(24, 24)
    , m_buttonSize(32, 32)
    , m_useThemeColor(true)
{
    setupButton();
    renderIcon();
}

IconButton::IconButton(const QString &iconPath, const QString &tooltip, QWidget *parent)
    : QPushButton(parent)
    , m_iconPath(iconPath)
    , m_iconSize(24, 24)
    , m_buttonSize(32, 32)
    , m_useThemeColor(true)
{
    setupButton();
    setToolTip(tooltip);
    renderIcon();
}

void IconButton::setupButton()
{
    setFixedSize(m_buttonSize);
    applyVSCodeStyle();
    
    // Connect to theme changes
    connect(&Theme::instance(), &Theme::themeChanged, this, &IconButton::applyTheme);
}

void IconButton::setIconPath(const QString &iconPath)
{
    if (m_iconPath != iconPath) {
        m_iconPath = iconPath;
        renderIcon();
    }
}

void IconButton::setIconSize(const QSize &size)
{
    if (m_iconSize != size) {
        m_iconSize = size;
        QPushButton::setIconSize(size);
        renderIcon();
    }
}

void IconButton::setButtonSize(const QSize &size)
{
    if (m_buttonSize != size) {
        m_buttonSize = size;
        setFixedSize(size);
    }
}

void IconButton::setIconColor(const QColor &color)
{
    if (m_iconColor != color) {
        m_iconColor = color;
        m_useThemeColor = false;
        renderIcon();
    }
}

void IconButton::applyVSCodeStyle()
{
    Theme &theme = Theme::instance();
    
    QColor fgColor = theme.getColor("explorer.foreground", QColor("#CCCCCC"));
    QColor hoverBg = theme.getColor("explorer.hoverBackground", QColor("#2A2D2E"));
    
    QFont explorerFont = theme.getFont("explorer");
    if (explorerFont.family().isEmpty()) {
        explorerFont = QFont("Segoe UI", 9);
    }

    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            color: %1;
            border: none;
            margin: 0px;
            padding: 0px;
            min-width: 0px;
            min-height: 0px;
            font-family: "%3";
            font-size: %4pt;
        }
        QPushButton:hover {
            background-color: %2;
            border: 1px solid %2;
        }
        QPushButton:pressed {
            background-color: %2;
        }
        QPushButton:disabled {
            opacity: 0.5;
        }
    )")
        .arg(fgColor.name())
        .arg(hoverBg.name())
        .arg(explorerFont.family())
        .arg(explorerFont.pointSize());

    setStyleSheet(buttonStyle);
}

void IconButton::applyCustomStyle(const QString &styleSheet)
{
    setStyleSheet(styleSheet);
}

void IconButton::refreshIcon()
{
    renderIcon();
}

void IconButton::applyTheme()
{
    applyVSCodeStyle();
    if (m_useThemeColor) {
        renderIcon();
    }
}

void IconButton::renderIcon()
{
    if (m_iconPath.isEmpty()) {
        return;
    }

    QColor iconColor = m_iconColor;
    if (m_useThemeColor) {
        Theme &theme = Theme::instance();
        iconColor = theme.getColor("explorer.foreground", QColor("#CCCCCC"));
    }

    QPixmap pixmap = createIconPixmap(m_iconPath, m_iconSize, iconColor);
    setIcon(QIcon(pixmap));
    QPushButton::setIconSize(m_iconSize);
}

QPixmap IconButton::createIconPixmap(const QString &iconPath, const QSize &size, const QColor &color)
{
    // Get device pixel ratio for high-DPI rendering
    qreal dpr = devicePixelRatioF();
    QSize pixmapSize(size.width() * dpr, size.height() * dpr);

    QSvgRenderer renderer(iconPath);
    if (!renderer.isValid()) {
        VOLT_ERROR_F("Failed to load SVG icon: %1", iconPath);
        return QPixmap();
    }

    QPixmap pixmap(pixmapSize);
    pixmap.fill(Qt::transparent);
    pixmap.setDevicePixelRatio(dpr);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Render the SVG
    renderer.render(&painter);
    
    // Apply color tint if specified
    if (color.isValid() && color != Qt::transparent) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), color);
    }
    
    painter.end();

    return pixmap;
}
