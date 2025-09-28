#include "CodeEditor.h"
#include "../themes/Theme.h"
#include "./logging/VoltLogger.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QsciScintilla(parent), lexer(nullptr)
{
    Theme::instance().loadTheme("dark"); 
    
    setupEditor();
        
    applyTheme();
}

void CodeEditor::setupEditor()
{
    setMarginType(0, QsciScintilla::NumberMargin);
    setMarginWidth(0, fontMetrics().horizontalAdvance(QLatin1Char('9')) * 6);
    setMarginLineNumbers(0, true);
    
    setViewportMargins(5, 0, 5, 0);

    setAutoIndent(true);
    setIndentationGuides(true);
    setIndentationsUseTabs(false);
    setTabWidth(4);
    setBraceMatching(QsciScintilla::SloppyBraceMatch);
    setSelectionToEol(true);
    setWrapMode(QsciScintilla::WrapNone);

    setCaretWidth(2);

    SendScintilla(SCI_SETHSCROLLBAR, 0); 
    SendScintilla(SCI_SETVSCROLLBAR, 0); 
    SendScintilla(SCI_SETYCARETPOLICY, CARET_SLOP | CARET_EVEN, 0L);
    
    setFolding(QsciScintilla::BoxedTreeFoldStyle);
}


void CodeEditor::applyTheme() {
    Theme& theme = Theme::instance();
    
    QColor bg = theme.getColor("editor.background");
    
    QColor fg = theme.getColor("editor.foreground");
    if (!fg.isValid()) {
        fg = QColor("#d4d4d4");
        qDebug() << "Using fallback foreground color";
    }
    
    QColor marginBg = theme.getColor("secondary");
    
    QColor marginFg = theme.getColor("editor.lineNumber.foreground");
    if (!marginFg.isValid()) marginFg = QColor("#858585");
    
    QColor selectionBg = theme.getColor("editor.selectionBackground");
    if (!selectionBg.isValid()) selectionBg = QColor("#264f78");
    
    QColor currentLineBg = theme.getColor("editor.currentLine");
    if (!currentLineBg.isValid()) currentLineBg = QColor("#2a2d2e");
    
    QColor caretColor = theme.getColor("editor.cursor");
    if (!caretColor.isValid()) caretColor = QColor("#f80303ff");
    
    QColor matchBrace = theme.getColor("editor.matchingBrace");
    if (!matchBrace.isValid()) matchBrace = QColor("#ffd700");
    
    QColor indentGuide = theme.getColor("editor.indent.guide");
    if (!indentGuide.isValid()) indentGuide = QColor("#404040");
    
    QColor commentColor = theme.getColor("syntax.comment");
    if (!commentColor.isValid()) commentColor = QColor("#6a9955");
    
    QColor stringColor = theme.getColor("syntax.string");
    if (!stringColor.isValid()) stringColor = QColor("#ce9178");
    
    QColor numberColor = theme.getColor("syntax.number");
    if (!numberColor.isValid()) numberColor = QColor("#b5cea8");
    
    QColor keywordColor = theme.getColor("syntax.keyword");
    if (!keywordColor.isValid()) keywordColor = QColor("#569cd6");
    
    QColor classColor = theme.getColor("syntax.class");
    if (!classColor.isValid()) classColor = QColor("#4ec9b0");
    
    QColor variableColor = theme.getColor("syntax.variable");
    if (!variableColor.isValid()) variableColor = QColor("#9cdcfe");
    
    QColor operatorColor = theme.getColor("syntax.operator");
    if (!operatorColor.isValid()) operatorColor = QColor("#d4d4d4");
    
    QColor preprocessorColor = theme.getColor("syntax.preprocessor");
    if (!preprocessorColor.isValid()) preprocessorColor = QColor("#569cd6");
    
    QFont editorFont = theme.getFont("editor");
    if (editorFont.family().isEmpty()) {
        editorFont = QFont("Consolas", 10);
        qDebug() << "Using fallback font: Consolas 10pt";
    } else {
        qDebug() << "Using theme font:" << editorFont.family() << editorFont.pointSize() << "pt";
    }

    SendScintilla(SCI_CLEARDOCUMENTSTYLE);
    
    SendScintilla(SCI_STYLESETBACK, STYLE_DEFAULT, bg.rgb());
    SendScintilla(SCI_STYLESETFORE, STYLE_DEFAULT, fg.rgb());
    SendScintilla(SCI_STYLESETFONT, STYLE_DEFAULT, editorFont.family().toUtf8().data());
    SendScintilla(SCI_STYLESETSIZE, STYLE_DEFAULT, editorFont.pointSize());
    SendScintilla(SCI_STYLECLEARALL);
    
    setPaper(bg);
    setColor(fg);
    setFont(editorFont);
    
    setAutoFillBackground(true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, bg);
    pal.setColor(QPalette::Window, bg);
    setPalette(pal);
    setBackgroundRole(QPalette::Base);
    setBraceMatching(QsciScintilla::SloppyBraceMatch);

    VoltLogger::VOLT_DEBUG_F("Margin background color: %1", marginBg.name());
    setMarginsBackgroundColor(marginBg);

    setFrameStyle(QFrame::NoFrame);
    setStyleSheet("QsciScintilla { border: none; outline: none; }");

    QColor gutterBg = marginBg.isValid() ? marginBg : bg;
    
    setMarginsBackgroundColor(gutterBg);
    setMarginsForegroundColor(marginFg);
    setMarginsFont(editorFont);
    setFoldMarginColors(gutterBg, gutterBg.darker(110));

    SendScintilla(SCI_STYLESETBACK, STYLE_LINENUMBER, gutterBg.rgb());
    SendScintilla(SCI_STYLESETFORE, STYLE_LINENUMBER, marginFg.rgb());

    for (int m = 0; m < 32; ++m) {
        SendScintilla(SCI_MARKERSETBACK, m, gutterBg.rgb());
        SendScintilla(SCI_MARKERSETFORE, m, marginFg.rgb());
    }
    
    setSelectionBackgroundColor(selectionBg);
    setSelectionForegroundColor(Qt::white);
    SendScintilla(SCI_SETSELFORE, true);
    
    setCaretForegroundColor(caretColor);
    setCaretLineVisible(true);
    setCaretLineBackgroundColor(currentLineBg);
    setCaretWidth(2);
    
    setMatchedBraceBackgroundColor(matchBrace);
    setMatchedBraceForegroundColor(Qt::black);
    setUnmatchedBraceForegroundColor(Qt::red);
    setUnmatchedBraceBackgroundColor(Qt::yellow);
    
    setIndentationGuidesBackgroundColor(bg);
    setIndentationGuidesForegroundColor(indentGuide);
    
    setFoldMarginColors(marginBg, marginBg.lighter(120));
    
    if (!lexer) {
        lexer = new QsciLexerCPP(this);
    }
    
    for (int style = 0; style <= 40; ++style) {
        lexer->setPaper(bg, style);
        lexer->setFont(editorFont, style);
    }
    
    lexer->setColor(fg, QsciLexerCPP::Default);
    
    lexer->setColor(commentColor, QsciLexerCPP::Comment);
    lexer->setColor(commentColor, QsciLexerCPP::CommentLine);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDoc);
    lexer->setColor(commentColor, QsciLexerCPP::CommentLineDoc);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDocKeyword);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDocKeywordError);
    
    lexer->setColor(numberColor, QsciLexerCPP::Number);
    
    QFont keywordFont = editorFont;
    keywordFont.setBold(true);
    lexer->setColor(keywordColor, QsciLexerCPP::Keyword);
    lexer->setFont(keywordFont, QsciLexerCPP::Keyword);
    
    lexer->setColor(classColor, QsciLexerCPP::KeywordSet2);
    lexer->setFont(keywordFont, QsciLexerCPP::KeywordSet2);
    
    lexer->setColor(stringColor, QsciLexerCPP::DoubleQuotedString);
    lexer->setColor(stringColor, QsciLexerCPP::SingleQuotedString);
    lexer->setColor(stringColor, QsciLexerCPP::RawString);
    lexer->setColor(stringColor, QsciLexerCPP::VerbatimString);
    lexer->setColor(stringColor, QsciLexerCPP::HashQuotedString);
    
    lexer->setColor(variableColor, QsciLexerCPP::Identifier);
    
    lexer->setColor(operatorColor, QsciLexerCPP::Operator);
    
    lexer->setColor(preprocessorColor, QsciLexerCPP::PreProcessor);
    lexer->setColor(stringColor, QsciLexerCPP::PreProcessorComment);
    
    lexer->setColor(classColor, QsciLexerCPP::GlobalClass);
    lexer->setFont(keywordFont, QsciLexerCPP::GlobalClass);
    
    lexer->setColor(numberColor, QsciLexerCPP::UUID);
    
    lexer->setColor(stringColor, QsciLexerCPP::Regex);
    
    lexer->setColor(numberColor, QsciLexerCPP::UserLiteral);
    
    lexer->setColor(commentColor, QsciLexerCPP::TaskMarker);
    QFont italicFont = editorFont;
    italicFont.setItalic(true);
    lexer->setFont(italicFont, QsciLexerCPP::TaskMarker);
    
    lexer->setColor(QColor("#d7ba7d"), QsciLexerCPP::EscapeSequence);
    
    setLexer(lexer);
    
    SendScintilla(SCI_COLOURISE, 0, -1);
    update();
    viewport()->update();
    repaint();

    setMarginWidth(0, fontMetrics().horizontalAdvance(QLatin1Char('9')) * 6);
    
    qDebug() << "=== THEME APPLICATION COMPLETED ===";
}

void CodeEditor::refreshTheme() {
    qDebug() << "CodeEditor refreshTheme() called";
    applyTheme();
}
