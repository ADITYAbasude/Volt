#include "StyleManager.h"
#include "../themes/Theme.h"

StyleManager& StyleManager::instance() {
    static StyleManager instance;
    return instance;
}

StyleManager::StyleManager() {
    // Connect to theme changes to update styles
    connect(&Theme::instance(), &Theme::themeChanged, this, &StyleManager::updateStyles);
    updateStyles();
}

void StyleManager::updateStyles() {
    // Styles will be updated whenever the theme changes
    // Currently using default values, but could load from theme
}

// Editor style getters
int StyleManager::getEditorTabWidth() const {
    // Get from theme if available, otherwise use default
    QVariant tabWidth = Theme::instance().getDimension("editor.tabWidth");
    return tabWidth.isValid() ? tabWidth.toInt() : 4;
}

QMargins StyleManager::getEditorMargins() const {
    // Get from theme if available, otherwise use default
    QVariant margins = Theme::instance().getDimension("editor.margins");
    return margins.isValid() ? margins.value<QMargins>() : QMargins(5, 5, 5, 5);
}

// Menu style getters
int StyleManager::getMenuItemHeight() const {
    QVariant height = Theme::instance().getDimension("menu.itemHeight");
    return height.isValid() ? height.toInt() : 24;
}

QMargins StyleManager::getMenuMargins() const {
    QVariant margins = Theme::instance().getDimension("menu.padding");
    return margins.isValid() ? margins.value<QMargins>() : QMargins(8, 4, 8, 4);
}

// Status bar style getters
int StyleManager::getStatusBarHeight() const {
    QVariant height = Theme::instance().getDimension("statusBar.height");
    return height.isValid() ? height.toInt() : 22;
}

QMargins StyleManager::getStatusBarPadding() const {
    QVariant padding = Theme::instance().getDimension("statusBar.padding");
    return padding.isValid() ? padding.value<QMargins>() : QMargins(5, 2, 5, 2);
}