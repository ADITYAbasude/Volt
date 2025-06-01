#pragma once

#include <QObject>
#include <QMargins>
class StyleManager : public QObject
{
    Q_OBJECT
public:
    static StyleManager &instance();

    // Editor Style
    int getEditorTabWidth() const;
    QMargins getEditorMargins() const;

    // Menu Style
    int getMenuItemHeight() const;
    QMargins getMenuMargins() const;

    // Status Bar Style
    int getStatusBarHeight() const;
    QMargins getStatusBarPadding() const;

private:
    StyleManager();
    void updateStyles();
};