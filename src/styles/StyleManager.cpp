#include "StyleManager.h"
#include "../themes/Theme.h"
#include "../logging/VoltLogger.h"

#include <QApplication>
#include <QAbstractScrollArea>
#include <QScrollBar>

StyleManager &StyleManager::instance()
{
    static StyleManager instance;
    return instance;
}

StyleManager::StyleManager()
{
    // Connect to theme changes to update styles
    connect(&Theme::instance(), &Theme::themeChanged, this, &StyleManager::updateStyles);
    updateStyles();
}

void StyleManager::updateStyles()
{
    applyGlobalScrollbarStyle();
    // applyGlobalScrollbarStyle will also apply scrollbar policies
}

// Editor style getters
int StyleManager::getEditorTabWidth() const
{
    // Get from theme if available, otherwise use default
    QVariant tabWidth = Theme::instance().getDimension("editor.tabWidth");
    return tabWidth.isValid() ? tabWidth.toInt() : 4;
}

QMargins StyleManager::getEditorMargins() const
{
    // Get from theme if available, otherwise use default
    QVariant margins = Theme::instance().getDimension("editor.margins");
    return margins.isValid() ? margins.value<QMargins>() : QMargins(5, 5, 5, 5);
}

// Menu style getters
int StyleManager::getMenuItemHeight() const
{
    QVariant height = Theme::instance().getDimension("menu.itemHeight");
    return height.isValid() ? height.toInt() : 24;
}

QMargins StyleManager::getMenuMargins() const
{
    QVariant margins = Theme::instance().getDimension("menu.padding");
    return margins.isValid() ? margins.value<QMargins>() : QMargins(8, 4, 8, 4);
}

// Status bar style getters
int StyleManager::getStatusBarHeight() const
{
    QVariant height = Theme::instance().getDimension("statusBar.height");
    return height.isValid() ? height.toInt() : 22;
}

QMargins StyleManager::getStatusBarPadding() const
{
    QVariant padding = Theme::instance().getDimension("statusBar.padding");
    return padding.isValid() ? padding.value<QMargins>() : QMargins(5, 2, 5, 2);
}

QString StyleManager::getGlobalScrollbarStyleSheet() const
{
    Theme &theme = Theme::instance();

    QColor bgColor = theme.getColor("scrollbar.background", QColor(240, 240, 240));
    QColor handleColor = theme.getColor("scrollbar.handle", QColor(180, 180, 180));
    QColor handleHoverColor = theme.getColor("scrollbar.handleHover", QColor(150, 150, 150));
    QColor handlePressedColor = theme.getColor("scrollbar.handlePressed", QColor(120, 120, 120));

    int width = theme.getDimensionInt("scrollbar.width", 14);
    int handleRadius = theme.getDimensionInt("scrollbar.handleRadius", 6);
    int handleMargin = theme.getDimensionInt("scrollbar.handleMargin", 2);
    int minHandleLength = theme.getDimensionInt("scrollbar.minimumHandleLength", 20);

    bool smoothScrolling = theme.getStyleBool("scrollbar", "smoothScrolling", true);
    int scrollSensitivity = theme.getStyleInt("scrollbar", "scrollSensitivity", 3);
    int animationDuration = theme.getStyleInt("scrollbar", "animationDuration", 150);

    return QString(R"(
        /* Vertical Scrollbar */
        QScrollBar:vertical {
            border: none;
            background: %1;
            width: %2px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: %3;
            min-height: %7px;
            border-radius: %5px;
            margin: %6px;
        }
        QScrollBar::handle:vertical:hover {
            background: %4;
        }
        QScrollBar::handle:vertical:pressed {
            background: %8;
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
            subcontrol-position: none;
        }
        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
            background: transparent;
        }
        
        /* Horizontal Scrollbar */
        QScrollBar:horizontal {
            border: none;
            background: %1;
            height: %2px;
            margin: 0;
        }
        QScrollBar::handle:horizontal {
            background: %3;
            min-width: %7px;
            border-radius: %5px;
            margin: %6px;
        }
        QScrollBar::handle:horizontal:hover {
            background: %4;
        }
        QScrollBar::handle:horizontal:pressed {
            background: %8;
        }
        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {
            width: 0px;
            subcontrol-position: none;
        }
        QScrollBar::add-page:horizontal,
        QScrollBar::sub-page:horizontal {
            background: transparent;
        }
        
        QScrollBar::corner {
            background: %1;
        }
        QScrollBar {
            qproperty-smoothScrolling: %9;
            qproperty-scrollSensitivity: %10;
            qproperty-animationDuration: %11;
        }
    )")
        .arg(bgColor.name())
        .arg(width)
        .arg(handleColor.name())
        .arg(handleHoverColor.name())
        .arg(handleRadius)
        .arg(handleMargin)
        .arg(minHandleLength)
        .arg(handlePressedColor.name())
        .arg(smoothScrolling ? "true" : "false")
        .arg(scrollSensitivity)
        .arg(animationDuration);
}

void StyleManager::applyGlobalScrollbarStyle() const
{
    QApplication *app = qApp;
    if (app)
    {
        QString currentStyle = app->styleSheet();
        QString scrollbarStyle = getGlobalScrollbarStyleSheet();
        app->setStyleSheet(currentStyle + "\n" + scrollbarStyle);

        VOLT_UI("Global scrollbar styling applied");

        // Also apply scrollbar policies to all widgets
        applyScrollbarPolicyToAll();
    }
}

void StyleManager::applyScrollbarPolicy(QWidget *widget) const
{
    if (!widget)
        return;

    // Check if the widget is a scrollable area
    QAbstractScrollArea *scrollArea = qobject_cast<QAbstractScrollArea *>(widget);
    if (scrollArea)
    {
        Theme &theme = Theme::instance();

        // Get scrollbar policies from theme
        QString verticalPolicy = theme.getStyleString("scrollbar", "verticalPolicy", "asNeeded");
        QString horizontalPolicy = theme.getStyleString("scrollbar", "horizontalPolicy", "asNeeded");

        // Convert string policy to Qt enum
        Qt::ScrollBarPolicy vPolicy = Qt::ScrollBarAsNeeded;
        Qt::ScrollBarPolicy hPolicy = Qt::ScrollBarAsNeeded;

        if (verticalPolicy == "alwaysOn")
        {
            vPolicy = Qt::ScrollBarAlwaysOn;
        }
        else if (verticalPolicy == "alwaysOff")
        {
            vPolicy = Qt::ScrollBarAlwaysOff;
        }

        if (horizontalPolicy == "alwaysOn")
        {
            hPolicy = Qt::ScrollBarAlwaysOn;
        }
        else if (horizontalPolicy == "alwaysOff")
        {
            hPolicy = Qt::ScrollBarAlwaysOff;
        }

        // Apply the policies
        scrollArea->setVerticalScrollBarPolicy(vPolicy);
        scrollArea->setHorizontalScrollBarPolicy(hPolicy);
    }
}

void StyleManager::applyScrollbarPolicyToAll() const
{
    QApplication *app = qApp;
    if (!app)
        return;

    // Find all scrollable widgets in the application
    QList<QAbstractScrollArea *> scrollAreas = app->findChildren<QAbstractScrollArea *>();

    for (QAbstractScrollArea *scrollArea : scrollAreas)
    {
        applyScrollbarPolicy(scrollArea);
    }
}

void StyleManager::setupWidgetScrollbars(QWidget *widget)
{
    StyleManager::instance().applyScrollbarPolicy(widget);
}