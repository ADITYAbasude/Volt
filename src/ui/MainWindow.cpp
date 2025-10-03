#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QTabBar>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QApplication>
#include <QGuiApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QAction>
#include <QKeySequence>
#include "../editor/CodeEditor.h"
#include "../editor/Minimap.h"
#include <QHBoxLayout>
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
    CodeEditor *editor = new CodeEditor(this);

    editorTab = new QTabWidget(this);
    editorTab->setTabsClosable(true);
    editorTab->setMovable(true);
    editorTab->tabBar()->setExpanding(true);
    setCentralWidget(editorTab);

    if (editorTab->tabBar())
    {
        QTabBar *tb = editorTab->tabBar();
        tb->setElideMode(Qt::ElideRight);
        tb->setExpanding(false);
        tb->setUsesScrollButtons(true);
        tb->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tb, &QTabBar::customContextMenuRequested, this, &MainWindow::onTabContextMenuRequested);
    }

    connect(editorTab, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
    connect(editorTab, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);

    StyleManager::setupWidgetScrollbars(editor);

    editor->setStyleSheet("QsciScintilla { border: none; outline: none; }");

    // Welcome tab container
    QWidget *container = new QWidget(this);
    QHBoxLayout *h = new QHBoxLayout(container);
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(0);
    container->setLayout(h);

    // Add editor into container
    h->addWidget(editor, 1);

    // Create minimap for this editor
    Minimap *minimap = new Minimap(editor, container);
    h->addWidget(minimap);
    Theme &theme = Theme::instance();
    QColor editorBg = theme.getColor("editor.background");
    container->setStyleSheet(QString("background-color: %1;").arg(editorBg.name()));

    // Sample welcome content
    editor->setText("// Welcome to Volt Editor\n\n"
                    "#include <iostream>\n\n"
                    "int main() {\n"
                    "    std::cout << \"Hello, World!\" << std::endl;\n"
                    "    return 0;\n"
                    "}\n");

    int idx = editorTab->addTab(container, "Welcome");
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

    // Style the tab bar according to theme
    if (editorTab && editorTab->tabBar())
    {
        QColor tabBg = theme.getColor("editor.background");
        QColor tabFg = theme.getColor("editor.foreground");
        QColor primary = theme.getColor("primary");

            // Tab styling is applied via StyleHelper/StyleManager
    }

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
    VOLT_DEBUG_F("Finding tab index for path: %1", filePath);
    if (!tabWidget || filePath.isEmpty())
    {
        VOLT_DEBUG("Invalid tab widget or file path");
        return -1;
    }

    VOLT_DEBUG_F("Tab count: %1", tabWidget->count());
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        VOLT_DEBUG_F("Checking tab %1", i);

        // Prefer tabData (we will store full path there)
        QVariant data;
        if (tabWidget->tabBar())
        {
            data = tabWidget->tabBar()->tabData(i);
        }
        if (data.isValid() && data.toString() == filePath)
        {
            VOLT_DEBUG_F2("Found tab %1 for path: %2 (tabData)", i, filePath);
            return i;
        }

        // Fallback: check tooltip (older code stored file name here)
        QString tabPath = tabWidget->tabToolTip(i);
        if (tabPath == filePath)
        {
            VOLT_DEBUG_F2("Found tab %1 for path: %2 (tooltip)", i, filePath);
            return i;
        }

        // Also compare displayed tab text (filename) as a last resort
        QString tabText = tabWidget->tabText(i);
        if (tabText == QFileInfo(filePath).fileName())
        {
            VOLT_DEBUG_F2("Found tab %1 for path: %2 (tabText)", i, filePath);
            return i;
        }
    }

    VOLT_DEBUG_F("No tab found for path: %1", filePath);
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

    // Apply theme and scrollbars to new editor
    StyleManager::setupWidgetScrollbars(editor);
    editor->refreshTheme();

    editor->setStyleSheet("QsciScintilla { border: none; outline: none; }");

    QWidget *container = new QWidget(this);
    QHBoxLayout *h = new QHBoxLayout(container);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(4);
    container->setLayout(h);
    h->addWidget(editor, 1);
    Minimap *minimap = new Minimap(editor, container);
    h->addWidget(minimap);

    int idx = editorTab->addTab(container, fileInfo.fileName());
    editorTab->setTabToolTip(idx, filePath);
    if (editorTab->tabBar())
    {
        editorTab->tabBar()->setTabData(idx, filePath);
    }
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

void MainWindow::onTabCloseRequested(int index)
{
    if (!editorTab || index < 0 || index >= editorTab->count())
        return;

    QWidget *widget = editorTab->widget(index);
    editorTab->removeTab(index);
    if (widget)
        widget->deleteLater();

    // Update window title to reflect current tab
    int currentIndex = editorTab->currentIndex();
    if (currentIndex >= 0)
    {
        QVariant data;
        if (editorTab->tabBar())
        {
            data = editorTab->tabBar()->tabData(currentIndex);
        }
        QString path = data.toString();
        QFileInfo fi(path);
        QString name = fi.exists() ? fi.fileName() : editorTab->tabText(currentIndex);
        setWindowTitle(QString("Volt Editor - %1").arg(name));
    }
    else
    {
        setWindowTitle("Volt Editor");
    }
}

void MainWindow::onTabContextMenuRequested(const QPoint &pos)
{
    QTabBar *tb = editorTab ? editorTab->tabBar() : nullptr;
    if (!tb)
        return;

    int idx = tb->tabAt(pos);
    if (idx < 0)
        return;

    QMenu menu(this);
    QAction *closeAct = menu.addAction("Close");
    QAction *closeOthers = menu.addAction("Close Others");
    QAction *closeRight = menu.addAction("Close Tabs to the Right");
    QAction *copyPath = menu.addAction("Copy Path");

    QAction *selected = menu.exec(tb->mapToGlobal(pos));
    if (!selected)
        return;

    if (selected == closeAct)
    {
        onTabCloseRequested(idx);
    }
    else if (selected == closeOthers)
    {
        for (int i = editorTab->count() - 1; i >= 0; --i)
        {
            if (i != idx)
                onTabCloseRequested(i);
        }
    }
    else if (selected == closeRight)
    {
        for (int i = editorTab->count() - 1; i > idx; --i)
        {
            onTabCloseRequested(i);
        }
    }
    else if (selected == copyPath)
    {
        QVariant data;
        if (editorTab->tabBar())
            data = editorTab->tabBar()->tabData(idx);
        QString path = data.isValid() ? data.toString() : editorTab->tabToolTip(idx);
        QGuiApplication::clipboard()->setText(path);
    }
}

void MainWindow::onCurrentTabChanged(int index)
{
    // Refresh theme on all editors when tab changes to prevent color issues
    if (editorTab)
    {
        for (int i = 0; i < editorTab->count(); ++i)
        {
            CodeEditor *ed = qobject_cast<CodeEditor *>(editorTab->widget(i));
            if (ed)
            {
                ed->refreshTheme();
                StyleManager::setupWidgetScrollbars(ed);
            }
        }
    }

    // Update window title
    if (index >= 0 && index < editorTab->count())
    {
        QVariant data;
        if (editorTab->tabBar())
        {
            data = editorTab->tabBar()->tabData(index);
        }
        QString path = data.toString();
        QFileInfo fi(path);
        QString name = fi.exists() ? fi.fileName() : editorTab->tabText(index);
        setWindowTitle(QString("Volt Editor - %1").arg(name));
    }
    else
    {
        setWindowTitle("Volt Editor");
    }
}