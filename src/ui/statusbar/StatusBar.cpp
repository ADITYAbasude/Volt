
#include "StatusBar.h"
#include <QStatusBar>
#include <QLabel>

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{
    setupLabels();
    setupStyle();
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

void StatusBar::setupStyle()
{
    setStyleSheet(R"(
        QStatusBar {
            background: #007ACC;
            color: white;
            border: none;
            border-top: 1px solid #15539E;
        }
        QStatusBar::item{
            border: none;
            background: transparent;    
        }
        QStatusBar QLabel {
            color: white;
            padding: 3px 6px;
            margin-left: 5px;
            margin-right: 5px;
            outline: none;
        }
        QStatusBar QLabel:hover {
            background: #1F8AD2;
        }
    )");
}

void StatusBar::updateCursorPosition(int line, int column)
{
    cursorPositionLabel->setText(QString("Ln %1, Col %2").arg(line).arg(column));
}

void StatusBar::updateLanguage(const QString &language)
{
    languageLabel->setText(language);
}

void StatusBar::updateEncoding(const QString &encoding)
{
    encodingLabel->setText(encoding);
}

void StatusBar::updateLineEnding(const QString &lineEnding)
{
    lineEndingLabel->setText(lineEnding);
}
