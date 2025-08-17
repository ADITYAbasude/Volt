#include "Theme.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>
#include <QMargins>
#include <QVariant>
#include "../logging/VoltLogger.h"

Theme& Theme::instance() {
    static Theme instance;
    return instance;
}

void Theme::loadTheme(const QString& themeName) {
    // Use applicationDirPath instead of currentPath for reliable theme file location
    QString appDir = QCoreApplication::applicationDirPath();
    QString themePath = appDir + "/themes/themes/" + themeName + ".json";  // Fixed double nested path
    
    VOLT_THEME_F("Loading theme from: %1", themePath);
    loadJsonTheme(themePath);
    m_currentTheme = themeName;
    emit themeChanged();
}

void Theme::loadJsonTheme(const QString& themePath) {
    QFile file(themePath);
    if (!file.open(QIODevice::ReadOnly)) {
        VOLT_ERROR_F("Failed to open theme file: %1", themePath);
        VOLT_DEBUG_F("File exists: %1", QFile::exists(themePath) ? "true" : "false");
        VOLT_DEBUG_F("Current working directory: %1", QDir::currentPath());
        VOLT_DEBUG_F("Application directory: %1", QCoreApplication::applicationDirPath());
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull()) {
        VOLT_ERROR_F("Failed to parse JSON from theme file: %1", themePath);
        return;
    }
    
    QJsonObject root = doc.object();
    VOLT_THEME_F("Successfully loaded theme JSON with %1 root elements", root.size());

    // Load colors
    m_colors.clear();
    QJsonObject colors = root["colors"].toObject();
    VOLT_THEME_F("Loading %1 colors from theme", colors.size());
    for (auto it = colors.begin(); it != colors.end(); ++it) {
        QColor color(it.value().toString());
        if (color.isValid()) {
            m_colors[it.key()] = color;
            VOLT_TRACE_F2("Loaded color: %1 = %2", it.key(), it.value().toString());
        } else {
            VOLT_WARN_F2("Invalid color: %1 = %2", it.key(), it.value().toString());
        }
    }

    // Load fonts
    m_fonts.clear();
    QJsonObject fonts = root["fonts"].toObject();
    VOLT_THEME_F("Loading %1 fonts from theme", fonts.size());
    for (auto it = fonts.begin(); it != fonts.end(); ++it) {
        QJsonObject fontObj = it.value().toObject();
        QFont font;
        font.setFamily(fontObj["family"].toString());
        font.setPointSize(fontObj["size"].toInt());
        font.setWeight(fontObj["weight"].toString() == "bold" ? QFont::Bold : QFont::Normal);
        m_fonts[it.key()] = font;
        VOLT_TRACE_F3("Loaded font: %1 = %2 %3pt", it.key(), fontObj["family"].toString(), fontObj["size"].toInt());
    }

    // Load dimensions
    m_dimensions.clear();
    QJsonObject dimensions = root["dimensions"].toObject();
    VOLT_THEME_F("Loading %1 dimensions from theme", dimensions.size());
    for (auto it = dimensions.begin(); it != dimensions.end(); ++it) {
        if (it.value().isArray()) {
            QJsonArray arr = it.value().toArray();
            QMargins margins(arr[0].toInt(), arr[1].toInt(), arr[2].toInt(), arr[3].toInt());
            m_dimensions[it.key()] = QVariant::fromValue(margins);
        } else {
            m_dimensions[it.key()] = QVariant::fromValue(it.value().toInt());
        }
        VOLT_TRACE_F("Loaded dimension: %1", it.key());
    }

    // Load styles
    m_styles.clear();
    QJsonObject styles = root["styles"].toObject();
    VOLT_THEME_F("Loading %1 styles from theme", styles.size());
    for (auto it = styles.begin(); it != styles.end(); ++it) {
        if (it.value().isObject()) {
            // Convert nested JSON object to string for storage
            QJsonDocument styleDoc(it.value().toObject());
            m_styles[it.key()] = QString::fromUtf8(styleDoc.toJson(QJsonDocument::Compact));
            VOLT_TRACE_F2("Loaded style object: %1 = %2", it.key(), m_styles[it.key()]);
        } else {
            // Handle simple string values
            m_styles[it.key()] = it.value().toString();
            VOLT_TRACE_F2("Loaded style string: %1 = %2", it.key(), it.value().toString());
        }
    }
    
    VOLT_THEME("Theme loading completed successfully!");
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

int Theme::getDimensionInt(const QString& key, int fallback) const {
    auto it = m_dimensions.find(key);
    if (it != m_dimensions.end()) {
        return it->second.toInt();
    }
    return fallback;
}

QMargins Theme::getDimensionMargins(const QString& key, const QMargins& fallback) const {
    auto it = m_dimensions.find(key);
    if (it != m_dimensions.end()) {
        return it->second.value<QMargins>();
    }
    return fallback;
}

QSize Theme::getDimensionSize(const QString& key, const QSize& fallback) const {
    auto it = m_dimensions.find(key);
    if (it != m_dimensions.end()) {
        QVariant var = it->second;
        if (var.canConvert<QSize>()) {
            return var.value<QSize>();
        }
        // If stored as array, convert [width, height]
        if (var.canConvert<QMargins>()) {
            QMargins margins = var.value<QMargins>();
            return QSize(margins.left(), margins.top());
        }
    }
    return fallback;
}

QMargins Theme::getDimensionMarginsFromArray(const QString& key, const QMargins& defaultValue) const
{
    // Use the stored dimensions map instead of undefined themeData
    auto it = m_dimensions.find(key);
    if (it != m_dimensions.end()) {
        QVariant var = it->second;
        if (var.canConvert<QMargins>()) {
            return var.value<QMargins>();
        }
    }
    
    VOLT_TRACE_F("Dimension key not found or invalid: %1 - using default", key);
    return defaultValue;
}

QString Theme::getStyle(const QString& key, QString fallback) const {
    auto it = m_styles.find(key);
    if (it != m_styles.end()) {
        return it->second;
    }
    return fallback;
}

bool Theme::getStyleBool(const QString& styleKey, const QString& property, bool fallback) const {
    auto it = m_styles.find(styleKey);
    if (it != m_styles.end()) {
        QJsonDocument doc = QJsonDocument::fromJson(it->second.toUtf8());
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            if (obj.contains(property)) {
                return obj[property].toBool(fallback);
            }
        }
    }
    return fallback;
}

int Theme::getStyleInt(const QString& styleKey, const QString& property, int fallback) const {
    auto it = m_styles.find(styleKey);
    if (it != m_styles.end()) {
        QJsonDocument doc = QJsonDocument::fromJson(it->second.toUtf8());
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            if (obj.contains(property)) {
                return obj[property].toInt(fallback);
            }
        }
    }
    return fallback;
}

QString Theme::getStyleString(const QString& styleKey, const QString& property, const QString& fallback) const {
    auto it = m_styles.find(styleKey);
    if (it != m_styles.end()) {
        QJsonDocument doc = QJsonDocument::fromJson(it->second.toUtf8());
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            if (obj.contains(property)) {
                return obj[property].toString(fallback);
            }
        }
    }
    return fallback;
}

QChar Theme::getCarbonIconChar(const QString& iconName) const {
    QFile file(":/icons/icons-carbon.json");
    if (!file.open(QIODevice::ReadOnly)) {
        VOLT_ERROR("Failed to open icons-carbon.json file");
        return QChar(); // Return invalid character
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull()) {
        VOLT_ERROR("Failed to parse icons-carbon.json file");
        return QChar();
    }

    QJsonObject root = doc.object();
    QJsonObject iconDefinitions = root["iconDefinitions"].toObject();
    
    if (!iconDefinitions.contains(iconName)) {
        // VOLT_ERROR_F("Icon '%1' not found in icons-carbon.json", iconName);
        return QChar();
    }

    QJsonObject iconObj = iconDefinitions[iconName].toObject();
    QString fontCharacter = iconObj["fontCharacter"].toString();
    
    // Convert unicode escape sequence (e.g., "\\e081") to actual unicode value
    if (fontCharacter.startsWith("\\e")) {
        bool ok;
        // Extract the hex part after \e (e.g., "081" from "\\e081")
        QString hexPart = fontCharacter.mid(2);
        // Convert to unicode value, adding 0xe000 base for private use area
        int unicodeValue = 0xe000 + hexPart.toInt(&ok, 16);
        if (ok) {
            return QChar(unicodeValue);
        }
    }
    
    VOLT_ERROR_F("Invalid font character format for icon '%1'", iconName);
    VOLT_ERROR_F("Font character value was: %1", fontCharacter);
    return QChar();
}