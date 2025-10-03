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