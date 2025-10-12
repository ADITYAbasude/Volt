
#include "StatusBar.h"
#include <QStatusBar>
#include <QLabel>
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{
    setupLabels();
    applyTheme();
}

void StatusBar::setupLabels()
{
    // Left side - notifications area (will be implemented later)
    addPermanentWidget(new QWidget(), 1); // Spaceholder for notifications area

    cursorPositionLabel = new QLabel("Ln 1, Col 1", this);
    languageLabel = new QLabel("C++", this);
    encodingLabel = new QLabel("UTF-8", this);
    lineEndingLabel = new QLabel("CRLF", this);

    // add items to the right side
    addPermanentWidget(cursorPositionLabel);
    addPermanentWidget(languageLabel);
    addPermanentWidget(encodingLabel);
    addPermanentWidget(lineEndingLabel);

    cursorPositionLabel->setToolTip("Go to Line/Column");
    languageLabel->setToolTip("Select Language Mode");
    encodingLabel->setToolTip("Select Encoding");
    lineEndingLabel->setToolTip("Select End of Line Sequence");
}

void StatusBar::applyTheme()
{
    Theme& theme = Theme::instance();
    
    QColor bgColor = theme.getColor("statusBar.background");
    QColor fgColor = theme.getColor("statusBar.foreground");
    QColor borderColor = theme.getColor("statusBar.border");
    
    int height = theme.getDimensionInt("statusBar.height", 22);
    QMargins padding = theme.getDimensionMarginsFromArray("statusBar.padding", QMargins(8, 0, 8, 0));
    
    QFont statusFont = theme.getFont("statusBar");
    if (statusFont.family().isEmpty()) {
        statusFont = QFont("Segoe UI", 9);
    }
    setFixedHeight(height);
    
    setFont(statusFont);
    if (cursorPositionLabel) cursorPositionLabel->setFont(statusFont);
    if (languageLabel) languageLabel->setFont(statusFont);
    if (encodingLabel) encodingLabel->setFont(statusFont);
    if (lineEndingLabel) lineEndingLabel->setFont(statusFont);
    
    // Create dynamic stylesheet from JSON values
    QString stylesheet = QString(R"(
        QStatusBar {
            background-color: %1;
            color: %2;
            border: none;
            border-top: 1px solid %3;
            font-family: "%4";
            font-size: %5pt;
        }
        QStatusBar::item {
            border: none;
            background: transparent;    
        }
        QStatusBar QLabel {
            color: %2;
            padding: %6px %7px %8px %9px;
            margin-left: 5px;
            margin-right: 5px;
            outline: none;
            font-family: "%4";
            font-size: %5pt;
        }
        QStatusBar QLabel:hover {
            background-color: %10;
        }
    )")
    .arg(bgColor.name())                    // %1 - background
    .arg(fgColor.name())                    // %2 - foreground
    .arg(borderColor.name())                // %3 - border
    .arg(statusFont.family())               // %4 - font family
    .arg(statusFont.pointSize())            // %5 - font size
    .arg(padding.top())                     // %6 - padding top
    .arg(padding.right())                   // %7 - padding right
    .arg(padding.bottom())                  // %8 - padding bottom
    .arg(padding.left())                    // %9 - padding left
    .arg(bgColor.lighter(120).name());      // %10 - hover color
    
    setStyleSheet(stylesheet);
    
    VoltLogger::instance().ui(" ===== VOLT STATUS BAR THEME SET SUCCESSFULLY =====");
}


/*
    * Updates the cursor position label in the status bar.
    * @param line The current line number.
    * @param column The current column number.
*/
void StatusBar::updateCursorPosition(int line, int column)
{
    cursorPositionLabel->setText(QString("Ln %1, Col %2").arg(line).arg(column));
}

/*
    * Updates the language label in the status bar.
    * @param language The language to display, e.g., "C++", "Python".
*/
void StatusBar::updateLanguage(const QString &language)
{
    languageLabel->setText(language);
}

/*
    * Updates the encoding label in the status bar.
    * @param encoding The encoding to display, e.g., "UTF-8", "ISO-8859-1".
*/
void StatusBar::updateEncoding(const QString &encoding)
{
    encodingLabel->setText(encoding);
}


void StatusBar::updateLineEnding(const QString &lineEnding)
{
    lineEndingLabel->setText(lineEnding);
}
