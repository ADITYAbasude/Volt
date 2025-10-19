#include "StyleHelper.h"

StyleHelper &StyleHelper::instance()
{
    static StyleHelper instance;
    return instance;
}

StyleHelper::StyleHelper(QObject *parent)
    : QObject(parent)
{
}

QString StyleHelper::getTreeViewStyle(const QColor &bgColor, const QColor &fgColor,
                                      const QColor &selectionBg, const QColor &selectionFg,
                                      const QColor &hoverBg, const QColor &primaryColor,
                                      const QFont &font) const
{
    return QString(R"(
        QTreeView { background-color: %1; color: %2; border: none; outline: none; font-family: "%6"; font-size: %7pt; show-decoration-selected: 1; }
        QTreeView::viewport { background-color: %1; }
        QTreeView::item { padding: 2px 4px; border: none; min-height: 22px; }
        QTreeView::item:hover { background-color: %5; }
        QTreeView::item:selected { background-color: %8; color: %4; }
        QTreeView::item:selected:!active { background-color: %3; }
        QTreeView::branch { background-color: %1; border-image: none; }
    )")
        .arg(bgColor.name())
        .arg(fgColor.name())
        .arg(selectionBg.name())
        .arg(selectionFg.name())
        .arg(hoverBg.name())
        .arg(font.family())
        .arg(font.pointSize())
        .arg(primaryColor.name());
}

QString StyleHelper::getTabWidgetStyle(const QColor &bgColor, const QColor &fgColor) const
{
    QString accent = fgColor.name();

    return QString(R"(
        /* overall tab widget pane (content area) */
        QTabWidget::pane {
            background: %1;
            border: none;
        }

        /* tab bar container */
        QTabWidget::tab-bar { left: 0px; alignment: left; background: %1; }

        /* ensure the QTabWidget and QTabBar containers also use the same background */
        QTabWidget, QTabBar { background: %1; }

        QTabBar {
            background: %1;
            border: none;
            qproperty-drawBase: false;
            qproperty-expanding: false;
            outline: none;
        }

        /* each tab: use same background as pane for unselected to avoid showing fallback */
        QTabBar::tab {
            background: %1;
            color: %2;
            margin: 0;
            padding: 0;
            min-width: 32px;
            min-height: 32px;
            max-width: 32px;
            max-height: 32px;
        }

        /* explicitly style unselected tabs and other tab controls */
        QTabBar::tab:!selected { background: %1; }
        QTabBar::tab:disabled { background: %1; }
        QTabWidget::tab-bar QToolButton, QTabBar QToolButton { background: %1; }
        QTabBar::left-arrow, QTabBar::right-arrow, QTabBar::scroller { background: %1; }

        QTabBar::tab:not(:last) { margin-right: 6px; }
        QTabBar::tab:first { margin-left: 2px; }

        /* hover/selected states */
        QTabBar::tab:hover {
            background: rgba(255,255,255,0.03);
        }
        QTabBar::tab:selected {
            background: %1;
            color: %2;
            border-bottom: 2px solid %3;
            padding-bottom: 4px;
        }

        /* ensure pane doesn't introduce gaps */
        QTabWidget::pane { margin: 0; padding: 0; }
    )")
        .arg(bgColor.name())
        .arg(fgColor.name())
        .arg(accent);
}

QString StyleHelper::getLabelStyle(const QColor &fgColor) const
{
    return QString("color: %1;").arg(fgColor.name());
}

QString StyleHelper::getDockWidgetStyle(const QColor &bgColor) const
{
    return QString("QDockWidget { background-color: %1; border: none; }").arg(bgColor.name());
}

QString StyleHelper::getMenuBarStyle(const QColor &bgColor, const QColor &fgColor, const QColor &borderColor,
                                     const QColor &selectionBg, const QColor &selectionFg, int itemHeight,
                                     const QMargins &padding, const QFont &font) const
{
    return QString(R"(
        QMenuBar {
            background-color: %1;
            color: %2;
            border-bottom: 1px solid %3;
            font-family: "%7";
            font-size: %8pt;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: %4px %5px %6px %9px;
            min-height: %10px;
            border: none;
        }
        QMenuBar::item:selected {
            background-color: %11;
            color: %12;
        }
        QMenuBar::item:pressed {
            background-color: %11;
            color: %12;
        }
        QMenu {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            font-family: "%7";
            font-size: %8pt;
        }
        QMenu::item {
            padding: %4px %5px %6px %9px;
            min-height: %10px;
        }
        QMenu::item:selected {
            background-color: %11;
            color: %12;
        }
    )")
        .arg(bgColor.name())
        .arg(fgColor.name())
        .arg(borderColor.name())
        .arg(padding.top())
        .arg(padding.right())
        .arg(padding.bottom())
        .arg(font.family())
        .arg(font.pointSize())
        .arg(padding.left())
        .arg(itemHeight)
        .arg(selectionBg.name())
        .arg(selectionFg.name());
}

QString StyleHelper::getEditorTabBarStyle(const QColor &tabBG, const QColor &tabFG, const QColor &tabActiveBG,
                                          const QColor &tabHoverBG) const
{
    
    return QString(R"(
        QTabBar {
            background: %3;
            border: none;
            qproperty-drawBase: false;
            qproperty-expanding: false;
            outline: none;
        }

        QTabBar::tab {
            background: %1;
            color: %2;
            margin: 0;
            padding: 0 12px;
            min-width: 80px;
            min-height: 28px;
        }

        QTabBar::tab:!selected { background: %1; }
        QTabBar::tab:disabled { background: %1; }

        QTabBar::tab:not(:last) { margin-right: 2px; }
        QTabBar::tab:first { margin-left: 2px; }

        QTabBar::tab:hover {
            background: %4;
        }
        QTabBar::tab:selected {
            background: %3 !important;
            color: %2;
            border-bottom: 2px solid %2;
            padding-bottom: 4px;
        }
    )")
        .arg(tabBG.name())
        .arg(tabFG.name())
        .arg(tabActiveBG.name())
        .arg(tabHoverBG.name());
}