#pragma once

#include <QPushButton>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>
#include <QSize>

/**
 * @brief A reusable icon button component that supports SVG icons with high-DPI rendering
 * 
 * This component provides:
 * - SVG icon rendering with device pixel ratio support
 * - Customizable button and icon sizes
 * - Theme-aware styling
 * - VS Code-like appearance
 */
class IconButton : public QPushButton
{
    Q_OBJECT

public:
    explicit IconButton(QWidget *parent = nullptr);
    explicit IconButton(const QString &iconPath, QWidget *parent = nullptr);
    explicit IconButton(const QString &iconPath, const QString &tooltip, QWidget *parent = nullptr);
    
    ~IconButton() = default;

    // Icon management
    void setIconPath(const QString &iconPath);
    void setIconSize(const QSize &size);
    void setButtonSize(const QSize &size);
    
    // Styling
    void setIconColor(const QColor &color);
    void applyVSCodeStyle();
    void applyCustomStyle(const QString &styleSheet);

    // Getters
    QString iconPath() const { return m_iconPath; }
    QSize iconSize() const { return m_iconSize; }

public slots:
    void refreshIcon();
    void applyTheme();

private:
    void setupButton();
    void renderIcon();
    QPixmap createIconPixmap(const QString &iconPath, const QSize &size, const QColor &color = QColor());

    QString m_iconPath;
    QSize m_iconSize;
    QSize m_buttonSize;
    QColor m_iconColor;
    bool m_useThemeColor;
};
