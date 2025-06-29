#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include "../editor/CodeEditor.h"
#include "../themes/Theme.h"
#include "../logging/VoltLogger.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Volt Editor");
    resize(800, 600);
    
    setupEditor();
    setupMenuBar();
    setupStatusBar();
    applyTheme();
    
    // Connect theme change signal to refresh all components
    connect(&Theme::instance(), &Theme::themeChanged, this, &MainWindow::onThemeChanged);
}

void MainWindow::setupEditor()
{
    // Create and set the editor as central widget
    editor = new CodeEditor(this);
    setCentralWidget(editor);
    
    // Add some sample text to verify the editor is working
    editor->setText("// Welcome to Volt Editor\n\n"
                   "#include <iostream>\n\n"
                   "int main() {\n"
                   "    std::cout << \"Hello, World!\" << std::endl;\n"
                   "    return 0;\n"
                   "}\n");
}

void MainWindow::setupMenuBar()
{
    fileMenu = new FileMenu(this);
    menuBar()->addMenu(fileMenu);
}

void MainWindow::setupStatusBar()
{
    statusBar = new StatusBar(this);
    setStatusBar(statusBar);
}

/*
    * Applies the current theme to the main window components.
    * This includes editor margins, tab width, menu styling, and status bar theme.
    * THIS FUNCTION IS CALLED ON THEME CHANGE SIGNAL TOO
*/
void MainWindow::applyTheme()
{
    Theme& theme = Theme::instance();
    
    qDebug() << "=== MAINWINDOW THEME APPLICATION ===";
    
    // Apply editor margins from JSON
    QMargins editorMargins = theme.getDimensionMarginsFromArray("editor.margins", QMargins(5, 5, 5, 5));
    if (editor) {
        editor->setContentsMargins(editorMargins);
        
        // Apply tab width from JSON
        int tabWidth = theme.getDimensionInt("editor.tabWidth", 4);
        editor->setTabWidth(tabWidth);
        
    }
    
    // Apply menu styling from JSON
    if (menuBar()) {
        QColor menuBg = theme.getColor("menu.background", QColor("#252526"));
        QColor menuFg = theme.getColor("menu.foreground", QColor("#CCCCCC"));
        QColor menuBorder = theme.getColor("menu.border", QColor("#454545"));
        QColor menuSelectionBg = theme.getColor("menu.selectionBackground", QColor("#04395E"));
        QColor menuSelectionFg = theme.getColor("menu.selectionForeground", QColor("#FFFFFF"));
        
        int menuItemHeight = theme.getDimensionInt("menu.itemHeight", 24);
        QMargins menuPadding = theme.getDimensionMarginsFromArray("menu.padding", QMargins(8, 4, 8, 4));
        
        QFont menuFont = theme.getFont("menu");
        if (menuFont.family().isEmpty()) {
            menuFont = QFont("Segoe UI", 9);
        }
        
        QString menuStyleSheet = QString(R"(
            QMenuBar {
                background-color: %1;
                color: %2;
                border-bottom: 1px solid %3;
                font-family: "%7";
                font-size: %8pt;
            }
            QMenuBar::item {
                background-color: transparent;
                padding: %4px %5px %6px %9px;
                min-height: %10px;
                border: none;
            }
            QMenuBar::item:selected {
                background-color: %11;
                color: %12;
            }
            QMenuBar::item:pressed {
                background-color: %11;
                color: %12;
            }
            QMenu {
                background-color: %1;
                color: %2;
                border: 1px solid %3;
                font-family: "%7";
                font-size: %8pt;
            }
            QMenu::item {
                padding: %4px %5px %6px %9px;
                min-height: %10px;
            }
            QMenu::item:selected {
                background-color: %11;
                color: %12;
            }
        )")
        .arg(menuBg.name())                 // %1 - background
        .arg(menuFg.name())                 // %2 - foreground  
        .arg(menuBorder.name())             // %3 - border
        .arg(menuPadding.top())             // %4 - padding top
        .arg(menuPadding.right())           // %5 - padding right
        .arg(menuPadding.bottom())          // %6 - padding bottom
        .arg(menuFont.family())             // %7 - font family
        .arg(menuFont.pointSize())          // %8 - font size
        .arg(menuPadding.left())            // %9 - padding left
        .arg(menuItemHeight)                // %10 - item height
        .arg(menuSelectionBg.name())        // %11 - selection background
        .arg(menuSelectionFg.name());       // %12 - selection foreground
        
        menuBar()->setStyleSheet(menuStyleSheet);
        
        qDebug() << "Applied menu theme - Background:" << menuBg.name() << "Height:" << menuItemHeight;
    }
    
    // Apply theme to status bar
    if (statusBar) {
        statusBar->applyTheme();
    }
    
    qDebug() << "MainWindow theme application completed!";
}

void MainWindow::onThemeChanged()
{
    VOLT_THEME("Theme changed, refreshing MainWindow components");
    applyTheme();
    
    if (editor) {
        editor->refreshTheme();
    }
    
    if (statusBar) {
        statusBar->applyTheme();
    }
}