#pragma once

#include <QString>
#include <QColor>
#include <QFont>
#include <QObject>
#include <QVariant>
#include <memory>
#include <unordered_map>
#include <QMargins>
#include <QSize>

class Theme : public QObject {
    Q_OBJECT

public:
    static Theme& instance();

    void loadTheme(const QString& themeName);
    QString currentTheme() const { return m_currentTheme; }

    // Getters
    QColor getColor(const QString& key) const;
    QColor getColor(const QString& key, const QColor& fallback) const;
    QFont getFont(const QString& key) const;
    QVariant getDimension(const QString& key) const;
    
    // Dimension helper methods
    int getDimensionInt(const QString& key, int fallback = 0) const;
    QMargins getDimensionMargins(const QString& key, const QMargins& fallback = QMargins()) const;
    QSize getDimensionSize(const QString& key, const QSize& fallback = QSize()) const;
    QMargins getDimensionMarginsFromArray(const QString& key, const QMargins& fallback = QMargins()) const;

    QString getStyle(const QString& key, QString fallback = QString()) const;
    
    // Style helper methods for accessing nested properties
    bool getStyleBool(const QString& styleKey, const QString& property, bool fallback = false) const;
    int getStyleInt(const QString& styleKey, const QString& property, int fallback = 0) const;
    QString getStyleString(const QString& styleKey, const QString& property, const QString& fallback = QString()) const;

signals:
    void themeChanged();

private:
    Theme() = default;
    void loadJsonTheme(const QString& themePath);

    QString m_currentTheme;
    std::unordered_map<QString, QColor> m_colors;
    std::unordered_map<QString, QFont> m_fonts;
    std::unordered_map<QString, QVariant> m_dimensions;
    std::unordered_map<QString, QString> m_styles; 
};
