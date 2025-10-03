#pragma once

#include <QObject>
#include <QColor>
#include <QFont>
#include <QString>

class Theme;

class StyleHelper : public QObject
{
    Q_OBJECT

public:
    static StyleHelper& instance();

    // Tree view styling
    QString getTreeViewStyle(const QColor &bgColor, const QColor &fgColor, 
                           const QColor &selectionBg, const QColor &selectionFg,
                           const QColor &hoverBg, const QColor &primaryColor,
                           const QFont &font) const;

    // Tab widget styling  
    QString getTabWidgetStyle(const QColor &bgColor, const QColor &fgColor) const;

    // Label styling
    QString getLabelStyle(const QColor &fgColor) const;

    // Dock widget styling
    QString getDockWidgetStyle(const QColor &bgColor) const;

private:
    StyleHelper(QObject *parent = nullptr);
    ~StyleHelper() = default;
    StyleHelper(const StyleHelper&) = delete;
    StyleHelper& operator=(const StyleHelper&) = delete;
};