#pragma once

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexercpp.h>
#include "../themes/Theme.h"

class CodeEditor : public QsciScintilla
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);
    bool hasUnsavedChanges() const { return isFileHasUnsavedChanges; }
    void markAsSaved();
    void setLoadingFile(bool loading) { isLoadingFile = loading; }

signals:
    void fileModificationChanged(bool hasChanges);

public slots:
    void applyTheme();
    void refreshTheme();

private slots:
    void updateMarginColors();
    void onLinesChanged();

private:
    void setupEditor();
    void configureMargins();
    void configureLexer();

    QsciLexerCPP *lexer;
    bool isFileHasUnsavedChanges;
    bool isLoadingFile;
    QString lastSavedContent;
};
