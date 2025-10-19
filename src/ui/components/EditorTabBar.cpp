#include "EditorTabBar.h"
#include <QPainter>
#include <QStyleOptionTab>
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"
#include "../../styles/StyleHelper.h"

EditorTabBar::EditorTabBar(QWidget *parent) : QTabBar(parent) {
    setDrawBase(false);
    setExpanding(false);
    setMovable(true);
    setTabsClosable(true);
    setUsesScrollButtons(true);
    setElideMode(Qt::ElideRight);

    setShape(QTabBar::RoundedNorth);
    // setDocumentMode(true);

    applyTheme();
}

void EditorTabBar::setTabModified(int index, bool modified) {
    if (index < 0 || index >= count()) {
        return;
    }
    
    m_modifiedTabs[index] = modified;
    update(); 
    
}

bool EditorTabBar::isTabModified(int index) const {
    return m_modifiedTabs.value(index, false);
}

void EditorTabBar::applyTheme() {

    Theme &theme = Theme::instance();
    
    QColor tabBg = theme.getColor("editor.tab.background");
    QColor tabFg = theme.getColor("editor.foreground");
    QColor tabActiveBg = theme.getColor("editor.background");
    QColor tabHoverBg = theme.getColor("hoverColor");
    QColor primary = theme.getColor("primary");
    QColor border = theme.getColor("menu.border");

    VOLT_DEBUG_F("Applying EditorTabBar theme - Tab BG: %1", tabBg.name());
    VOLT_DEBUG_F("Applying EditorTabBar theme - Tab FG: %1", tabFg.name());
    VOLT_DEBUG_F("Applying EditorTabBar theme - Tab Active BG: %1", tabActiveBg.name());
    VOLT_DEBUG_F("Applying EditorTabBar theme - Tab Hover BG: %1", tabHoverBg.name());

    QString stylesheet = StyleHelper::instance().getEditorTabBarStyle(
        tabBg, tabFg, tabActiveBg, tabHoverBg, primary, border
    );    
    VOLT_DEBUG_F("EditorTabBar Stylesheet: %1", stylesheet);
    
    setStyleSheet(stylesheet);
}

void EditorTabBar::paintEvent(QPaintEvent *event) {
    QTabBar::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    Theme &theme = Theme::instance();
    QColor dotColor = theme.getColor("editor.modifiedIndicator");
    
    for (int i = 0; i < count(); ++i) {
        if (m_modifiedTabs.value(i, false)) {
            QRect tabRect = this->tabRect(i);
            
            int dotSize = 6;
            int x = tabRect.left() + 8;
            int y = tabRect.center().y() - dotSize / 2;
            
            painter.setBrush(dotColor);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(x, y, dotSize, dotSize);
        }
    }
}
