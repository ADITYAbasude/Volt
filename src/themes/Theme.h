#pragma once

#include <QString>
#include <QColor>
#include <QFont>
#include <QObject>
#include <QVariant>
#include <memory>
#include <unordered_map>

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

signals:
    void themeChanged();

private:
    Theme() = default;
    void loadJsonTheme(const QString& themePath);

    QString m_currentTheme;
    std::unordered_map<QString, QColor> m_colors;
    std::unordered_map<QString, QFont> m_fonts;
    std::unordered_map<QString, QVariant> m_dimensions;
};
