
#pragma once

#include <QStatusBar>
#include <QLabel>

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);
    ~StatusBar() = default;

public slots:
    void updateCursorPosition(int line, int column);
    void updateLanguage(const QString &language);
    void updateEncoding(const QString &encoding);
    void updateLineEnding(const QString &lineEnding);

private:
    QLabel *cursorPositionLabel;
    QLabel *languageLabel;
    QLabel *encodingLabel;
    QLabel *lineEndingLabel;

    void setupLabels();
    void setupStyle();

};
