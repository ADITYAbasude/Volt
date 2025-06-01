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
    
    // === DEFINE COLORS DIRECTLY (FALLBACK APPROACH) ===
    QColor bg("#1e1e1e");           // Dark background
    QColor fg("#d4d4d4");           // Light foreground
    QColor marginBg("#1e1e1e");     // Margin background
    QColor marginFg("#858585");     // Line numbers
    QColor selectionBg("#264f78");  // Selection background
    QColor currentLineBg("#2a2d2e"); // Current line
    QColor caretColor("#ffffff");   // Cursor
    QColor matchBrace("#ffd700");   // Brace matching
    QColor indentGuide("#404040");  // Indent guides
    
    // Syntax colors
    QColor commentColor("#6a9955");    // Green comments
    QColor stringColor("#ce9178");     // Orange strings
    QColor numberColor("#b5cea8");     // Light green numbers
    QColor keywordColor("#569cd6");    // Blue keywords
    QColor classColor("#4ec9b0");      // Teal classes/types
    QColor variableColor("#9cdcfe");   // Light blue variables
    QColor operatorColor("#d4d4d4");   // Default operators
    QColor preprocessorColor("#569cd6"); // Blue preprocessor
    
    // Try to get colors from theme, use fallbacks if not available
    bg = theme.getColor("editor.background", bg);
    fg = theme.getColor("editor.foreground", fg);
    marginBg = theme.getColor("editor.lineNumber.background", marginBg);
    marginFg = theme.getColor("editor.lineNumber.foreground", marginFg);
    selectionBg = theme.getColor("editor.selectionBackground", selectionBg);
    currentLineBg = theme.getColor("editor.currentLine", currentLineBg);
    caretColor = theme.getColor("editor.cursor", caretColor);
    matchBrace = theme.getColor("editor.matchingBrace", matchBrace);
    indentGuide = theme.getColor("editor.indent.guide", indentGuide);
    
    commentColor = theme.getColor("syntax.comment", commentColor);
    stringColor = theme.getColor("syntax.string", stringColor);
    numberColor = theme.getColor("syntax.number", numberColor);
    keywordColor = theme.getColor("syntax.keyword", keywordColor);
    classColor = theme.getColor("syntax.class", classColor);
    variableColor = theme.getColor("syntax.variable", variableColor);
    operatorColor = theme.getColor("syntax.operator", operatorColor);
    preprocessorColor = theme.getColor("syntax.preprocessor", preprocessorColor);
    
    QFont editorFont = theme.getFont("editor");
    if (editorFont.family().isEmpty()) {
        editorFont = QFont("Consolas", 10);
    }
    
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
}

void CodeEditor::refreshTheme() {
    // Force complete theme reapplication
    if (lexer) {
        delete lexer;
        lexer = nullptr;
    }
    applyTheme();
}
