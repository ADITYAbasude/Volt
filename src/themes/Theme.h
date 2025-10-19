#pragma once

#include <QColor>
#include <QFont>
#include <QMap>
#include <QMargins>
#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>

class Theme : public QObject {
  Q_OBJECT

public:
  static Theme &instance();

  void loadTheme(const QString &themeName);
  QString currentTheme() const { return m_currentTheme; }

  // Getters
  QColor getColor(const QString &key) const;
  QColor getColor(const QString &key, const QColor &fallback) const;
  QFont getFont(const QString &key) const;
  QVariant getDimension(const QString &key) const;

  // Dimension helper methods
  int getDimensionInt(const QString &key, int fallback = 0) const;
  QMargins getDimensionMargins(const QString &key,
                               const QMargins &fallback = QMargins()) const;
  QSize getDimensionSize(const QString &key,
                         const QSize &fallback = QSize()) const;
  QMargins
  getDimensionMarginsFromArray(const QString &key,
                               const QMargins &fallback = QMargins()) const;

  QString getStyle(const QString &key, QString fallback = QString()) const;

  // Carbon icon helper
  QChar getCarbonIconChar(const QString &iconName) const;

  // Style helper methods for accessing nested properties
  bool getStyleBool(const QString &styleKey, const QString &property,
                    bool fallback = false) const;
  int getStyleInt(const QString &styleKey, const QString &property,
                  int fallback = 0) const;
  QString getStyleString(const QString &styleKey, const QString &property,
                         const QString &fallback = QString()) const;

signals:
  void themeChanged();

private:
  Theme() = default;
  void loadJsonTheme(const QString &themePath);

  QString m_currentTheme;
  QMap<QString, QColor> m_colors;
  QMap<QString, QFont> m_fonts;
  QMap<QString, QVariant> m_dimensions;
  QMap<QString, QString> m_styles;
};
