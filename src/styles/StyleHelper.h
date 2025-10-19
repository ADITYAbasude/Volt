#pragma once

#include <QObject>
#include <QColor>
#include <QFont>
#include <QString>
#include <QMargins>

class Theme;

class StyleHelper : public QObject
{
    Q_OBJECT

public:
    static StyleHelper &instance();

    QString getTreeViewStyle(const QColor &bgColor, const QColor &fgColor,
                             const QColor &selectionBg, const QColor &selectionFg,
                             const QColor &hoverBg, const QColor &primaryColor,
                             const QFont &font) const;

    QString getTabWidgetStyle(const QColor &bgColor, const QColor &fgColor) const;

    QString getLabelStyle(const QColor &fgColor) const;

    QString getDockWidgetStyle(const QColor &bgColor) const;

    QString getMenuBarStyle(const QColor &bgColor, const QColor &fgColor, const QColor &borderColor,
                            const QColor &selectionBg, const QColor &selectionFg, int itemHeight,
                            const QMargins &padding, const QFont &font) const;

    QString getEditorTabBarStyle(const QColor &tabBG, const QColor &tabFG, const QColor &tabActiveBG,
                                 const QColor &tabHoverBG) const;

private:
    StyleHelper(QObject *parent = nullptr);
    ~StyleHelper() = default;
    StyleHelper(const StyleHelper &) = delete;
    StyleHelper &operator=(const StyleHelper &) = delete;
};