#include "FilledColorButton.h"
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"

FilledColorButton::FilledColorButton(QWidget *parent)
    : QPushButton(parent)
    , m_buttonStyle(Primary)
    , m_borderRadius(4)
    , m_useThemeColors(true)
{
    setupButton();
}

FilledColorButton::FilledColorButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , m_buttonStyle(Primary)
    , m_borderRadius(4)
    , m_useThemeColors(true)
{
    setupButton();
}

FilledColorButton::FilledColorButton(const QString &text, ButtonStyle style, QWidget *parent)
    : QPushButton(text, parent)
    , m_buttonStyle(style)
    , m_borderRadius(4)
    , m_useThemeColors(true)
{
    setupButton();
}

void FilledColorButton::setupButton()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(32);
    
    loadThemeColors();
    updateStyleSheet();
    
    // Connect to theme changes
    connect(&Theme::instance(), &Theme::themeChanged, this, &FilledColorButton::applyTheme);
}

void FilledColorButton::setButtonStyle(ButtonStyle style)
{
    if (m_buttonStyle != style) {
        m_buttonStyle = style;
        m_useThemeColors = (style != Custom);
        loadThemeColors();
        updateStyleSheet();
    }
}

void FilledColorButton::setCustomColors(const QColor &background, const QColor &text, 
                                       const QColor &hover, const QColor &pressed)
{
    m_buttonStyle = Custom;
    m_useThemeColors = false;
    m_backgroundColor = background;
    m_textColor = text;
    m_hoverColor = hover.isValid() ? hover : background.lighter(110);
    m_pressedColor = pressed.isValid() ? pressed : background.darker(110);
    updateStyleSheet();
}

void FilledColorButton::setBackgroundColor(const QColor &color)
{
    if (m_backgroundColor != color) {
        m_backgroundColor = color;
        m_useThemeColors = false;
        updateStyleSheet();
    }
}

void FilledColorButton::setTextColor(const QColor &color)
{
    if (m_textColor != color) {
        m_textColor = color;
        m_useThemeColors = false;
        updateStyleSheet();
    }
}

void FilledColorButton::setHoverColor(const QColor &color)
{
    if (m_hoverColor != color) {
        m_hoverColor = color;
        updateStyleSheet();
    }
}

void FilledColorButton::setPressedColor(const QColor &color)
{
    if (m_pressedColor != color) {
        m_pressedColor = color;
        updateStyleSheet();
    }
}

void FilledColorButton::setBorderRadius(int radius)
{
    if (m_borderRadius != radius) {
        m_borderRadius = radius;
        updateStyleSheet();
    }
}

void FilledColorButton::applyTheme()
{
    if (m_useThemeColors) {
        loadThemeColors();
        updateStyleSheet();
    }
}

void FilledColorButton::refreshStyle()
{
    updateStyleSheet();
}

void FilledColorButton::loadThemeColors()
{
    Theme &theme = Theme::instance();
    
    switch (m_buttonStyle) {
        case Primary:
            m_backgroundColor = theme.getColor("primary");
            m_textColor = theme.getColor("button.foreground");
            m_hoverColor = theme.getColor("button.primary.hoverBackground", m_backgroundColor.lighter(110));
            m_pressedColor = theme.getColor("button.primary.pressedBackground", m_backgroundColor.darker(110));
            break;
            
        case Secondary:
            m_backgroundColor = theme.getColor("button.secondary.background", QColor("#5A5A5A"));
            m_textColor = theme.getColor("button.foreground");
            m_hoverColor = theme.getColor("button.secondary.hoverBackground", m_backgroundColor.lighter(110));
            m_pressedColor = theme.getColor("button.secondary.pressedBackground", m_backgroundColor.darker(110));
            break;
            
        case Accent:
            m_backgroundColor = theme.getColor("button.accent.background", QColor("#14A085"));
            m_textColor = theme.getColor("button.accent.foreground", QColor("#FFFFFF"));
            m_hoverColor = theme.getColor("button.accent.hoverBackground", m_backgroundColor.lighter(110));
            m_pressedColor = theme.getColor("button.accent.pressedBackground", m_backgroundColor.darker(110));
            break;
            
        case Success:
            m_backgroundColor = theme.getColor("button.success.background", QColor("#28A745"));
            m_textColor = theme.getColor("button.success.foreground", QColor("#FFFFFF"));
            m_hoverColor = theme.getColor("button.success.hoverBackground", m_backgroundColor.lighter(110));
            m_pressedColor = theme.getColor("button.success.pressedBackground", m_backgroundColor.darker(110));
            break;
            
        case Warning:
            m_backgroundColor = theme.getColor("button.warning.background", QColor("#FFC107"));
            m_textColor = theme.getColor("button.warning.foreground", QColor("#212529"));
            m_hoverColor = theme.getColor("button.warning.hoverBackground", m_backgroundColor.lighter(110));
            m_pressedColor = theme.getColor("button.warning.pressedBackground", m_backgroundColor.darker(110));
            break;
            
        case Danger:
            m_backgroundColor = theme.getColor("button.danger.background", QColor("#DC3545"));
            m_textColor = theme.getColor("button.danger.foreground", QColor("#FFFFFF"));
            m_hoverColor = theme.getColor("button.danger.hoverBackground", m_backgroundColor.lighter(110));
            m_pressedColor = theme.getColor("button.danger.pressedBackground", m_backgroundColor.darker(110));
            break;
            
        case Custom:
            // Custom colors are set manually, don't override
            break;
    }
}

void FilledColorButton::updateStyleSheet()
{
    Theme &theme = Theme::instance();
    QFont buttonFont = theme.getFont("button");
    if (buttonFont.family().isEmpty()) {
        buttonFont = QFont("Segoe UI", 9);
    }

    QString styleSheet = QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            border: none;
            border-radius: %5px;
            padding: 8px 16px;
            font-family: "%6";
            font-size: %7pt;
            font-weight: normal;
            text-align: center;
        }
        QPushButton:hover {
            background-color: %3;
        }
        QPushButton:pressed {
            background-color: %4;
        }
        QPushButton:disabled {
            background-color: %1;
            opacity: 0.5;
        }
        QPushButton:focus {
            outline: none;
            border: 2px solid %3;
        }
    )")
        .arg(m_backgroundColor.name())     // %1
        .arg(m_textColor.name())          // %2  
        .arg(m_hoverColor.name())         // %3
        .arg(m_pressedColor.name())       // %4
        .arg(m_borderRadius)              // %5
        .arg(buttonFont.family())         // %6
        .arg(buttonFont.pointSize());     // %7

    setStyleSheet(styleSheet);
}
