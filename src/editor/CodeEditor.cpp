#include "CodeEditor.h"
#include "../styles/StyleManager.h"
#include "../themes/Theme.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QsciScintilla(parent), lexer(nullptr)
{
    // Initialize the theme to dark theme
    Theme::instance().loadTheme("dark"); 
    
    // Set up the editor first
    setupEditor();
    
    // Apply comprehensive dark theme
    applyTheme();
}

void CodeEditor::setupEditor()
{
    // Configure basic editor appearance (margins, line numbers)
    setMarginType(0, QsciScintilla::NumberMargin);
    setMarginWidth(0, fontMetrics().horizontalAdvance(QLatin1Char('9')) * 6);
    setMarginLineNumbers(0, true);
    
    // Set viewport margins
    setViewportMargins(0, 0, 0, 0);

    // Editor behaviors
    setAutoIndent(true);
    setIndentationGuides(true);
    setIndentationsUseTabs(false);
    setTabWidth(4);
    setBraceMatching(QsciScintilla::SloppyBraceMatch);
    setSelectionToEol(true);
    setWrapMode(QsciScintilla::WrapNone);

    // Caret settings
    setCaretWidth(2);

    // Scrollbars
    SendScintilla(SCI_SETHSCROLLBAR, 1);
    SendScintilla(SCI_SETVSCROLLBAR, 1);
    
    // Enable folding
    setFolding(QsciScintilla::BoxedTreeFoldStyle);
}

void CodeEditor::applyTheme() {
    Theme& theme = Theme::instance();
    
    // Get colors from theme (with sensible fallbacks only if JSON completely fails)
    QColor bg = theme.getColor("editor.background");
    if (!bg.isValid()) {
        bg = QColor("#1e1e1e");
        qDebug() << "Using fallback background color";
    }
    
    QColor fg = theme.getColor("editor.foreground");
    if (!fg.isValid()) {
        fg = QColor("#d4d4d4");
        qDebug() << "Using fallback foreground color";
    }
    
    QColor marginBg = theme.getColor("editor.lineNumber.background");
    if (!marginBg.isValid()) marginBg = bg;
    
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
    
    // Syntax colors from theme
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
    
    // Get font from theme
    QFont editorFont = theme.getFont("editor");
    if (editorFont.family().isEmpty()) {
        editorFont = QFont("Consolas", 10);
        qDebug() << "Using fallback font: Consolas 10pt";
    } else {
        qDebug() << "Using theme font:" << editorFont.family() << editorFont.pointSize() << "pt";
    }
    
    // Debug: Show which colors are being used
    qDebug() << "=== THEME COLORS APPLIED ===";
    qDebug() << "Background:" << bg.name() << "(valid:" << bg.isValid() << ")";
    qDebug() << "Foreground:" << fg.name() << "(valid:" << fg.isValid() << ")";
    qDebug() << "Comment:" << commentColor.name() << "(valid:" << commentColor.isValid() << ")";
    qDebug() << "String:" << stringColor.name() << "(valid:" << stringColor.isValid() << ")";
    qDebug() << "Keyword:" << keywordColor.name() << "(valid:" << keywordColor.isValid() << ")";
    qDebug() << "Variable:" << variableColor.name() << "(valid:" << variableColor.isValid() << ")";
    
    // === STEP 1: CLEAR ALL EXISTING STYLES ===
    SendScintilla(SCI_CLEARDOCUMENTSTYLE);
    
    // === STEP 2: SET BASE STYLE ===
    SendScintilla(SCI_STYLESETBACK, STYLE_DEFAULT, bg.rgb());
    SendScintilla(SCI_STYLESETFORE, STYLE_DEFAULT, fg.rgb());
    SendScintilla(SCI_STYLESETFONT, STYLE_DEFAULT, editorFont.family().toUtf8().data());
    SendScintilla(SCI_STYLESETSIZE, STYLE_DEFAULT, editorFont.pointSize());
    SendScintilla(SCI_STYLECLEARALL);
    
    // === STEP 3: HIGH-LEVEL EDITOR SETTINGS ===
    setPaper(bg);
    setColor(fg);
    setFont(editorFont);
    
    // === STEP 4: MARGINS ===
    setMarginsBackgroundColor(marginBg);
    setMarginsForegroundColor(marginFg);
    setMarginsFont(editorFont);
    
    // === STEP 5: SELECTION ===
    setSelectionBackgroundColor(selectionBg);
    setSelectionForegroundColor(Qt::white);
    SendScintilla(SCI_SETSELFORE, true);
    
    // === STEP 6: CARET AND CURRENT LINE ===
    setCaretForegroundColor(caretColor);
    setCaretLineVisible(true);
    setCaretLineBackgroundColor(currentLineBg);
    setCaretWidth(2);
    
    // === STEP 7: BRACE MATCHING ===
    setMatchedBraceBackgroundColor(matchBrace);
    setMatchedBraceForegroundColor(Qt::black);
    setUnmatchedBraceForegroundColor(Qt::red);
    setUnmatchedBraceBackgroundColor(Qt::yellow);
    
    // === STEP 8: INDENTATION GUIDES ===
    setIndentationGuidesBackgroundColor(bg);
    setIndentationGuidesForegroundColor(indentGuide);
    
    // === STEP 9: FOLDING ===
    setFoldMarginColors(marginBg, marginBg.lighter(120));
    
    // === STEP 10: CREATE LEXER ===
    if (!lexer) {
        lexer = new QsciLexerCPP(this);
    }
    
    // === STEP 11: COMPREHENSIVE SYNTAX HIGHLIGHTING ===
    
    // Set all styles to have the dark background first
    for (int style = 0; style <= 40; ++style) {
        lexer->setPaper(bg, style);
        lexer->setFont(editorFont, style);
    }
    
    // Default/Whitespace
    lexer->setColor(fg, QsciLexerCPP::Default);
    
    // Comments (all types)
    lexer->setColor(commentColor, QsciLexerCPP::Comment);
    lexer->setColor(commentColor, QsciLexerCPP::CommentLine);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDoc);
    lexer->setColor(commentColor, QsciLexerCPP::CommentLineDoc);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDocKeyword);
    lexer->setColor(commentColor, QsciLexerCPP::CommentDocKeywordError);
    
    // Numbers
    lexer->setColor(numberColor, QsciLexerCPP::Number);
    
    // Keywords (make them bold)
    QFont keywordFont = editorFont;
    keywordFont.setBold(true);
    lexer->setColor(keywordColor, QsciLexerCPP::Keyword);
    lexer->setFont(keywordFont, QsciLexerCPP::Keyword);
    
    // Secondary keywords (types)
    lexer->setColor(classColor, QsciLexerCPP::KeywordSet2);
    lexer->setFont(keywordFont, QsciLexerCPP::KeywordSet2);
    
    // Strings (all types)
    lexer->setColor(stringColor, QsciLexerCPP::DoubleQuotedString);
    lexer->setColor(stringColor, QsciLexerCPP::SingleQuotedString);
    lexer->setColor(stringColor, QsciLexerCPP::RawString);
    lexer->setColor(stringColor, QsciLexerCPP::VerbatimString);
    lexer->setColor(stringColor, QsciLexerCPP::HashQuotedString);
    
    // Identifiers
    lexer->setColor(variableColor, QsciLexerCPP::Identifier);
    
    // Operators
    lexer->setColor(operatorColor, QsciLexerCPP::Operator);
    
    // Preprocessor
    lexer->setColor(preprocessorColor, QsciLexerCPP::PreProcessor);
    lexer->setColor(stringColor, QsciLexerCPP::PreProcessorComment);
    
    // Classes
    lexer->setColor(classColor, QsciLexerCPP::GlobalClass);
    lexer->setFont(keywordFont, QsciLexerCPP::GlobalClass);
    
    // UUIDs
    lexer->setColor(numberColor, QsciLexerCPP::UUID);
    
    // Regex
    lexer->setColor(stringColor, QsciLexerCPP::Regex);
    
    // User literals
    lexer->setColor(numberColor, QsciLexerCPP::UserLiteral);
    
    // Task markers
    lexer->setColor(commentColor, QsciLexerCPP::TaskMarker);
    QFont italicFont = editorFont;
    italicFont.setItalic(true);
    lexer->setFont(italicFont, QsciLexerCPP::TaskMarker);
    
    // Escape sequences
    lexer->setColor(QColor("#d7ba7d"), QsciLexerCPP::EscapeSequence);
    
    // === STEP 12: APPLY LEXER ===
    setLexer(lexer);
    
    // === STEP 13: FORCE COMPLETE REFRESH ===
    SendScintilla(SCI_COLOURISE, 0, -1);
    update();
    viewport()->update();
    repaint();
    
    qDebug() << "=== THEME APPLICATION COMPLETED ===";
}

void CodeEditor::refreshTheme() {
    qDebug() << "CodeEditor refreshTheme() called";
    applyTheme();
}
