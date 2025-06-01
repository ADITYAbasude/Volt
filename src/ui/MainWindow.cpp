#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include "../editor/CodeEditor.h"
#include <styles/StyleManager.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Volt Editor");
    resize(800, 600);
    
    setupEditor();
    setupMenuBar();
    setupStatusBar();
}

void MainWindow::setupEditor()
{
    // Create and set the editor as central widget
    editor = new CodeEditor(this);
    setCentralWidget(editor);
    
    // Apply basic setup instead of theme
    editor->applyTheme();


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

void MainWindow::applyTheme()
{
    // Simplified for hardcoded approach
    editor->setTabWidth(4);
    editor->setContentsMargins(0, 0, 0, 0);
    
    // Set font manually
    QFont font("Consolas", 10);
    font.setFixedPitch(true);
    editor->setFont(font);
}