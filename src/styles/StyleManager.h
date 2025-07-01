#pragma once

#include <QObject>
#include <QMargins>
#include <QString>
#include <QColor>
#include <QWidget>
#include <QAbstractScrollArea>

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

    QString getGlobalScrollbarStyleSheet() const;
    void applyGlobalScrollbarStyle() const;
    
    // Scrollbar policy management
    void applyScrollbarPolicy(QWidget* widget) const;
    void applyScrollbarPolicyToAll() const;
    
    // Helper method to set up scrollbar for newly created widgets
    static void setupWidgetScrollbars(QWidget* widget);

    // scrollbar style getter

    int getScrollbarWidth() const;
    int getScrollbarHandleRadius() const;
    QColor getScrollbarHandle() const;
    QColor getScrollbarBackground() const;
    QColor getScrollbarHandleHover() const;

    // get styles properties
    bool isScrollbarSmoothScrolling() const;
    int getScrollbarScrollSensitivity() const;
    int getScrollbarAnimationDuration() const;

private:
    StyleManager();
    void updateStyles();
};