#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QAction>
#include <QKeySequence>
#include "../editor/CodeEditor.h"
#include "../themes/Theme.h"
#include "../logging/VoltLogger.h"
#include "../styles/StyleManager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Volt Editor");
    resize(1200, 800);

    setupEditor();
    setupMenuBar();
    setupStatusBar();
    setupSidebar();
    applyTheme();

    // Connect theme change signal to refresh all components
    connect(&Theme::instance(), &Theme::themeChanged, this, &MainWindow::onThemeChanged);
}

void MainWindow::setupEditor()
{
    // Create and set the editor as central widget
    CodeEditor *editor = new CodeEditor(this);
    // setCentralWidget(editor); //! have to remove

    // setup QTab centeral widget
    editorTab = new QTabWidget(this);
    setCentralWidget(editorTab);

    // Apply scrollbar policy to the editor
    StyleManager::setupWidgetScrollbars(editor);

    // TODO: Show here welcome tab instead to this welcome message
    //  Add some sample text to verify the editor is working
    editor->setText("// Welcome to Volt Editor\n\n"
                    "#include <iostream>\n\n"
                    "int main() {\n"
                    "    std::cout << \"Hello, World!\" << std::endl;\n"
                    "    return 0;\n"
                    "}\n");

    int idx = editorTab->addTab(editor, "Welcome");
    editorTab->setTabToolTip(idx, "Welcome");
    editorTab->setTabIcon(idx, QIcon(":/icons/app.ico"));
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

void MainWindow::setupSidebar()
{
    sidebar = new Sidebar(this);
    addDockWidget(Qt::LeftDockWidgetArea, sidebar);

    // Connect sidebar signals
    connect(sidebar, &Sidebar::fileDoubleClicked,
            this, &MainWindow::openFile);
}

/*
 * Applies the current theme to the main window components.
 * This includes editor margins, tab width, menu styling, and status bar theme.
 * THIS FUNCTION IS CALLED ON THEME CHANGE SIGNAL TOO
 */
void MainWindow::applyTheme()
{
    Theme &theme = Theme::instance();

    StyleManager::instance().applyGlobalScrollbarStyle();

    // Apply editor margins from JSON
    QMargins editorMargins = theme.getDimensionMarginsFromArray("editor.margins", QMargins(5, 5, 5, 5));
    int tabWidth = theme.getDimensionInt("editor.tabWidth", 4);

    if (editorTab)
    {
        for (int i = 0; i < editorTab->count(); ++i)
        {
            CodeEditor *ed = qobject_cast<CodeEditor *>(editorTab->widget(i));
            if (ed)
            {
                ed->refreshTheme();
            }
        }
    }

    if (statusBar)
    {
        statusBar->applyTheme();
    }

    // Apply menu styling from JSON
    if (menuBar())
    {
        QColor menuBg = theme.getColor("menu.background", QColor("#252526"));
        QColor menuFg = theme.getColor("menu.foreground", QColor("#CCCCCC"));
        QColor menuBorder = theme.getColor("menu.border", QColor("#454545"));
        QColor menuSelectionBg = theme.getColor("menu.selectionBackground", QColor("#04395E"));
        QColor menuSelectionFg = theme.getColor("menu.selectionForeground", QColor("#FFFFFF"));

        int menuItemHeight = theme.getDimensionInt("menu.itemHeight", 24);
        QMargins menuPadding = theme.getDimensionMarginsFromArray("menu.padding", QMargins(8, 4, 8, 4));

        QFont menuFont = theme.getFont("menu");
        if (menuFont.family().isEmpty())
        {
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
                                     .arg(menuBg.name())           // %1 - background
                                     .arg(menuFg.name())           // %2 - foreground
                                     .arg(menuBorder.name())       // %3 - border
                                     .arg(menuPadding.top())       // %4 - padding top
                                     .arg(menuPadding.right())     // %5 - padding right
                                     .arg(menuPadding.bottom())    // %6 - padding bottom
                                     .arg(menuFont.family())       // %7 - font family
                                     .arg(menuFont.pointSize())    // %8 - font size
                                     .arg(menuPadding.left())      // %9 - padding left
                                     .arg(menuItemHeight)          // %10 - item height
                                     .arg(menuSelectionBg.name())  // %11 - selection background
                                     .arg(menuSelectionFg.name()); // %12 - selection foreground

        menuBar()->setStyleSheet(menuStyleSheet);

        qDebug() << "Applied menu theme - Background:" << menuBg.name() << "Height:" << menuItemHeight;
    }

    // Apply theme to status bar
    if (statusBar)
    {
        statusBar->applyTheme();
    }

    qDebug() << "MainWindow theme application completed!";
}

void MainWindow::onThemeChanged()
{
    VOLT_THEME("Theme changed, refreshing MainWindow components");
    applyTheme();

    if (editorTab)
    {
        for (int i = 0; i < editorTab->count(); i++)
        {
            CodeEditor *ed = qobject_cast<CodeEditor *>(editorTab->widget(i));
            if (ed)
            {
                ed->refreshTheme();
            }
        }
    }

    if (statusBar)
    {
        statusBar->applyTheme();
    }
}

static int findTabIndexForPath(QTabWidget *tabWidget, const QString &filePath)
{
    if (!tabWidget || filePath.isEmpty())
        return -1;

    for (int i = 0; i < tabWidget->count(); ++i)
    {
        // Use tabToolTip to store file path for each tab
        QString tabPath = tabWidget->tabToolTip(i);
        if (tabPath == filePath)
            return i;
    }
    return -1;
}

void MainWindow::openFile(const QString &filePath)
{
    // create new editor tab

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile())
    {
        QMessageBox::warning(this, "Error", "File does not exist: " + filePath);
        return;
    }

    // Check that this file is not already open
    int existingIndex = findTabIndexForPath(editorTab, filePath);
    if (existingIndex != -1)
    {
        editorTab->setCurrentIndex(existingIndex);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "Cannot open file: " + filePath);
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // create new editor instance
    CodeEditor *editor = new CodeEditor(this);
    editor->setText(content);

    int idx = editorTab->addTab(editor, fileInfo.fileName());
    editorTab->setTabToolTip(idx, fileInfo.fileName());
    editorTab->setTabIcon(idx, QIcon(":/icons/app.ico"));
    editorTab->setCurrentIndex(idx);

    setWindowTitle(QString("Volt Editor - %1").arg(fileInfo.fileName()));

    VoltLogger::instance().info("Opened file: %1", filePath);
}

void MainWindow::openFolder(const QString &folderPath)
{
    if (sidebar)
    {
        sidebar->setRootPath(folderPath);
        VoltLogger::instance().info("Opened folder: %1", folderPath);
    }
}