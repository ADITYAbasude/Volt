#pragma once

#include <QPushButton>
#include <QColor>
#include <QFont>

/**
 * @brief A reusable filled color button component with customizable colors and styling
 * 
 * This component provides:
 * - Customizable background and text colors
 * - Hover and pressed state styling
 * - Theme-aware styling
 * - Various button styles (primary, secondary, accent, etc.)
 */
class FilledColorButton : public QPushButton
{
    Q_OBJECT

public:
    enum ButtonStyle {
        Primary,
        Secondary,
        Accent,
        Success,
        Warning,
        Danger,
        Custom
    };

    explicit FilledColorButton(QWidget *parent = nullptr);
    explicit FilledColorButton(const QString &text, QWidget *parent = nullptr);
    explicit FilledColorButton(const QString &text, ButtonStyle style, QWidget *parent = nullptr);
    
    ~FilledColorButton() = default;

    // Style management
    void setButtonStyle(ButtonStyle style);
    void setCustomColors(const QColor &background, const QColor &text, 
                        const QColor &hover = QColor(), const QColor &pressed = QColor());
    
    // Color setters
    void setBackgroundColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setHoverColor(const QColor &color);
    void setPressedColor(const QColor &color);
    void setBorderRadius(int radius);
    
    // Getters
    QColor backgroundColor() const { return m_backgroundColor; }
    QColor textColor() const { return m_textColor; }
    QColor hoverColor() const { return m_hoverColor; }
    QColor pressedColor() const { return m_pressedColor; }
    ButtonStyle buttonStyle() const { return m_buttonStyle; }

public slots:
    void applyTheme();
    void refreshStyle();

private:
    void setupButton();
    void updateStyleSheet();
    void loadThemeColors();

    ButtonStyle m_buttonStyle;
    QColor m_backgroundColor;
    QColor m_textColor;
    QColor m_hoverColor;
    QColor m_pressedColor;
    int m_borderRadius;
    bool m_useThemeColors;
};
