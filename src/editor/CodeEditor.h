#pragma once

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>
#include "../themes/Theme.h"

class CodeEditor : public QsciScintilla
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);
    
public slots:
    void applyTheme();
    void refreshTheme();  // Force complete theme refresh
    
private:
    void setupEditor();
    QsciLexerCPP *lexer;
};
