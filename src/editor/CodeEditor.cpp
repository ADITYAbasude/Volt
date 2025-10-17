#include "CodeEditor.h"
#include "../themes/Theme.h"
#include "../logging/VoltLogger.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QsciScintilla(parent), lexer(nullptr)
{
    Theme::instance().loadTheme("dark");

    setupEditor();
    applyTheme();

    //* For updating margin colors on cursor move *// 
    connect(this, &QsciScintilla::cursorPositionChanged,
            this, &CodeEditor::updateMarginColors);
}

void CodeEditor::setupEditor()
{
    // Editor behavior
    setAutoIndent(true);
    setIndentationGuides(true);
    setIndentationsUseTabs(false);
    setTabWidth(4);
    setBraceMatching(QsciScintilla::SloppyBraceMatch);
    setSelectionToEol(true);
    setWrapMode(QsciScintilla::WrapNone);
    setCaretWidth(2);

    // Viewport and scrollbars
    setViewportMargins(0, 0, 0, 0);
    SendScintilla(SCI_SETHSCROLLBAR, 0);
    SendScintilla(SCI_SETVSCROLLBAR, 0);
    SendScintilla(SCI_SETYCARETPOLICY, CARET_SLOP | CARET_EVEN, 0L);

    // Frame
    setFrameStyle(QFrame::NoFrame);
}

void CodeEditor::applyTheme()
{
    Theme &theme = Theme::instance();

    // Get theme colors
    QColor bg = theme.getColor("editor.background");
    QColor fg = theme.getColor("editor.foreground");
    QColor selectionBg = theme.getColor("editor.selectionBackground");
    QColor currentLineBg = theme.getColor("editor.currentLine");
    QColor caretColor = theme.getColor("editor.cursor");
    QColor matchBrace = theme.getColor("editor.matchingBrace");
    QColor indentGuide = theme.getColor("editor.indent.guide");

    // Get font
    QFont editorFont = theme.getFont("editor");
    if (editorFont.family().isEmpty())
    {
        editorFont = QFont("Consolas", 10);
    }

    // Clear and set default styles
    SendScintilla(SCI_CLEARDOCUMENTSTYLE);
    SendScintilla(SCI_STYLESETBACK, STYLE_DEFAULT, bg.rgb());
    SendScintilla(SCI_STYLESETFORE, STYLE_DEFAULT, fg.rgb());
    SendScintilla(SCI_STYLESETFONT, STYLE_DEFAULT, editorFont.family().toUtf8().data());
    SendScintilla(SCI_STYLESETSIZE, STYLE_DEFAULT, editorFont.pointSize());
    SendScintilla(SCI_STYLECLEARALL);

    // Apply base colors and font
    setPaper(bg);
    setColor(fg);
    setFont(editorFont);

    // Configure widget background
    setAutoFillBackground(true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, bg);
    pal.setColor(QPalette::Window, bg);
    setPalette(pal);
    setBackgroundRole(QPalette::Base);

    // Apply stylesheet
    setStyleSheet(QString("QsciScintilla { background-color: %1; border: none; outline: none; }").arg(bg.name()));

    configureLexer();

    // Configure margins AFTER lexer is set
    configureMargins();

    // Selection
    setSelectionBackgroundColor(selectionBg);
    setSelectionForegroundColor(Qt::white);
    SendScintilla(SCI_SETSELFORE, true);

    // Caret and current line
    setCaretForegroundColor(caretColor);
    setCaretLineVisible(true);
    setCaretLineBackgroundColor(currentLineBg);
    setCaretWidth(2);

    // Brace matching
    setMatchedBraceBackgroundColor(matchBrace);
    setMatchedBraceForegroundColor(Qt::black);
    setUnmatchedBraceForegroundColor(Qt::red);
    setUnmatchedBraceBackgroundColor(Qt::yellow);

    // Indentation guides
    setIndentationGuidesBackgroundColor(bg);
    setIndentationGuidesForegroundColor(indentGuide);

    // Force repaint
    SendScintilla(SCI_COLOURISE, 0, -1);
    update();
}

void CodeEditor::configureLexer()
{
    Theme &theme = Theme::instance();

    QColor bg = theme.getColor("editor.background");
    QColor fg = theme.getColor("editor.foreground");
    QColor commentColor = theme.getColor("syntax.comment");
    QColor stringColor = theme.getColor("syntax.string");
    QColor numberColor = theme.getColor("syntax.number");
    QColor keywordColor = theme.getColor("syntax.keyword");
    QColor classColor = theme.getColor("syntax.class");
    QColor variableColor = theme.getColor("syntax.variable");
    QColor operatorColor = theme.getColor("syntax.operator");
    QColor preprocessorColor = theme.getColor("syntax.preprocessor");

    QFont editorFont = theme.getFont("editor");
    if (editorFont.family().isEmpty())
    {
        editorFont = QFont("Consolas", 10);
    }

    if (!lexer)
    {
        lexer = new QsciLexerCPP(this);
    }

    // Set base colors for all styles
    for (int style = 0; style <= 40; ++style)
    {
        lexer->setPaper(bg, style);
        lexer->setFont(editorFont, style);
    }

    // Configure syntax colors
    lexer->setColor(fg, QsciLexerCPP::Default);

    // Comments
    lexer->setColor(commentColor, QsciLexerCPP::Comment);
    lexer->setColor(commentColor, QsciLexerCPP::CommentLine);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDoc);
    lexer->setColor(commentColor, QsciLexerCPP::CommentLineDoc);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDocKeyword);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDocKeywordError);
    lexer->setColor(commentColor, QsciLexerCPP::TaskMarker);

    // Numbers
    lexer->setColor(numberColor, QsciLexerCPP::Number);
    lexer->setColor(numberColor, QsciLexerCPP::UUID);
    lexer->setColor(numberColor, QsciLexerCPP::UserLiteral);

    // Keywords
    QFont keywordFont = editorFont;
    keywordFont.setBold(true);
    lexer->setColor(keywordColor, QsciLexerCPP::Keyword);
    lexer->setFont(keywordFont, QsciLexerCPP::Keyword);
    lexer->setColor(classColor, QsciLexerCPP::KeywordSet2);
    lexer->setFont(keywordFont, QsciLexerCPP::KeywordSet2);

    // Strings
    lexer->setColor(stringColor, QsciLexerCPP::DoubleQuotedString);
    lexer->setColor(stringColor, QsciLexerCPP::SingleQuotedString);
    lexer->setColor(stringColor, QsciLexerCPP::RawString);
    lexer->setColor(stringColor, QsciLexerCPP::VerbatimString);
    lexer->setColor(stringColor, QsciLexerCPP::HashQuotedString);
    lexer->setColor(stringColor, QsciLexerCPP::PreProcessorComment);
    lexer->setColor(stringColor, QsciLexerCPP::Regex);

    // Identifiers and operators
    lexer->setColor(variableColor, QsciLexerCPP::Identifier);
    lexer->setColor(operatorColor, QsciLexerCPP::Operator);

    // Preprocessor
    lexer->setColor(preprocessorColor, QsciLexerCPP::PreProcessor);

    // Classes
    lexer->setColor(classColor, QsciLexerCPP::GlobalClass);
    lexer->setFont(keywordFont, QsciLexerCPP::GlobalClass);

    // Special formatting
    QFont italicFont = editorFont;
    italicFont.setItalic(true);
    lexer->setFont(italicFont, QsciLexerCPP::TaskMarker);
    lexer->setColor(QColor("#d7ba7d"), QsciLexerCPP::EscapeSequence);

    setLexer(lexer);
}

void CodeEditor::configureMargins()
{
    Theme &theme = Theme::instance();

    QColor marginBg = theme.getColor("editor.lineNumber.background");
    QColor marginFg = theme.getColor("editor.lineNumber.foreground");

    QFont editorFont = theme.getFont("editor");
    if (editorFont.family().isEmpty())
    {
        editorFont = QFont("Consolas", 10);
    }

    // Margin 0: Line numbers
    setMarginType(0, QsciScintilla::NumberMargin);
    setMarginWidth(0, fontMetrics().horizontalAdvance(QLatin1Char('9')) * 6);
    setMarginLineNumbers(0, true);
    SendScintilla(SCI_SETMARGINBACKN, 0, marginBg.rgb());

    // Apply global margin colors
    setMarginsBackgroundColor(marginBg);
    setMarginsForegroundColor(marginFg);
    setMarginsFont(editorFont);

    // Configure line number style
    SendScintilla(SCI_STYLESETBACK, STYLE_LINENUMBER, marginBg.rgb());
    SendScintilla(SCI_STYLESETFORE, STYLE_LINENUMBER, marginFg.rgb());
    SendScintilla(SCI_STYLESETFONT, STYLE_LINENUMBER, editorFont.family().toUtf8().data());
    SendScintilla(SCI_STYLESETSIZE, STYLE_LINENUMBER, editorFont.pointSize());

    // Configure markers
    for (int m = 0; m < 32; ++m)
    {
        SendScintilla(SCI_MARKERSETBACK, m, marginBg.rgb());
        SendScintilla(SCI_MARKERSETFORE, m, marginFg.rgb());
    }

    // Configure folding
    setFoldMarginColors(marginBg, marginBg);
    setFolding(QsciScintilla::BoxedTreeFoldStyle);
    SendScintilla(SCI_SETMARGINBACKN, 2, marginBg.rgb());
}

void CodeEditor::updateMarginColors()
{
    Theme &theme = Theme::instance();

    QColor marginBg = theme.getColor("editor.lineNumber.background");
    QColor marginFg = theme.getColor("editor.lineNumber.foreground");
    QColor currentLineBg = theme.getColor("editor.currentLine");

    int currentLine, currentCol;
    getCursorPosition(&currentLine, &currentCol);

    // Get total lines for margin width calculation
    int totalLines = lines();

    // Update each line's margin color
    for (int line = 0; line < totalLines; ++line)
    {
        if (line == currentLine)
        {
            SendScintilla(SCI_MARGINSETSTYLE, line, STYLE_LINENUMBER);
        }
    }
}

void CodeEditor::refreshTheme()
{
    applyTheme();
}
