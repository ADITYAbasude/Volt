#include "Theme.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QMargins>
#include <QVariant>

Theme& Theme::instance() {
    static Theme instance;
    return instance;
}

void Theme::loadTheme(const QString& themeName) {
    QString themePath = QDir::currentPath() + "/themes/" + themeName + ".json";
    loadJsonTheme(themePath);
    m_currentTheme = themeName;
    emit themeChanged();
}

void Theme::loadJsonTheme(const QString& themePath) {
    QFile file(themePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open theme file:" << themePath;
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    // Load colors
    m_colors.clear();
    QJsonObject colors = root["colors"].toObject();
    for (auto it = colors.begin(); it != colors.end(); ++it) {
        m_colors[it.key()] = QColor(it.value().toString());
    }

    // Load fonts
    m_fonts.clear();
    QJsonObject fonts = root["fonts"].toObject();
    for (auto it = fonts.begin(); it != fonts.end(); ++it) {
        QJsonObject fontObj = it.value().toObject();
        QFont font;
        font.setFamily(fontObj["family"].toString());
        font.setPointSize(fontObj["size"].toInt());
        font.setWeight(fontObj["weight"].toString() == "bold" ? QFont::Bold : QFont::Normal);
        m_fonts[it.key()] = font;
    }

    // Load dimensions
    m_dimensions.clear();
    QJsonObject dimensions = root["dimensions"].toObject();
    for (auto it = dimensions.begin(); it != dimensions.end(); ++it) {
        if (it.value().isArray()) {
            QJsonArray arr = it.value().toArray();
            QMargins margins(arr[0].toInt(), arr[1].toInt(), arr[2].toInt(), arr[3].toInt());
            m_dimensions[it.key()] = QVariant::fromValue(margins);
        } else {
            m_dimensions[it.key()] = QVariant::fromValue(it.value().toInt());
        }
    }
}

QColor Theme::getColor(const QString& key) const {
    auto it = m_colors.find(key);
    return it != m_colors.end() ? it->second : QColor();
}

QColor Theme::getColor(const QString& key, const QColor& fallback) const {
    auto it = m_colors.find(key);
    return it != m_colors.end() ? it->second : fallback;
}

QFont Theme::getFont(const QString& key) const {
    auto it = m_fonts.find(key);
    return it != m_fonts.end() ? it->second : QFont();
}

QVariant Theme::getDimension(const QString& key) const {
    auto it = m_dimensions.find(key);
    return it != m_dimensions.end() ? it->second : QVariant();
}