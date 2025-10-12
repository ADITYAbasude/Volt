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
        tb->setExpanding(true);
        tb->setUsesScrollButtons(true);
        tb->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tb, &QTabBar::customContextMenuRequested, this, &MainWindow::onTabContextMenuRequested);
    }

    connect(editorTab, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);
    connect(editorTab, &QTabWidget::currentChanged, this, &MainWindow::onCurrentTabChanged);

    StyleManager::setupWidgetScrollbars(editor);
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

    if (menuBar())
    {
        QColor menuBg = theme.getColor("menu.background");
        QColor menuFg = theme.getColor("menu.foreground");
        QColor menuBorder = theme.getColor("menu.border");
        QColor menuSelectionBg = theme.getColor("menu.selectionBackground");
        QColor menuSelectionFg = theme.getColor("menu.selectionForeground");

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
                                     .arg(menuBg.name())
                                     .arg(menuFg.name())
                                     .arg(menuBorder.name())
                                     .arg(menuPadding.top())
                                     .arg(menuPadding.right())
                                     .arg(menuPadding.bottom())
                                     .arg(menuFont.family())
                                     .arg(menuFont.pointSize())
                                     .arg(menuPadding.left())
                                     .arg(menuItemHeight)
                                     .arg(menuSelectionBg.name())
                                     .arg(menuSelectionFg.name());

        menuBar()->setStyleSheet(menuStyleSheet);

        qDebug() << "Applied menu theme - Background:" << menuBg.name() << "Height:" << menuItemHeight;
    }

    if (editorTab && editorTab->tabBar())
    {
        QColor tabBg = theme.getColor("editor.background");
        QColor tabFg = theme.getColor("editor.foreground");
        QColor primary = theme.getColor("primary");
    }

    if (statusBar)
    {
        statusBar->applyTheme();
    }
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

    for (int i = 0; i < tabWidget->count(); ++i)
    {
        VOLT_DEBUG_F("Checking tab %1", i);

        QVariant data;
        if (tabWidget->tabBar())
        {
            data = tabWidget->tabBar()->tabData(i);
        }
        if (data.isValid() && data.toString() == filePath)
        {
            return i;
        }

        QString tabPath = tabWidget->tabToolTip(i);
        if (tabPath == filePath)
        {
            return i;
        }

        QString tabText = tabWidget->tabText(i);
        if (tabText == QFileInfo(filePath).fileName())
        {
            return i;
        }
    }

    return -1;
}

void MainWindow::openFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile())
    {
        QMessageBox::warning(this, "Error", "File does not exist: " + filePath);
        return;
    }

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

    CodeEditor *editor = new CodeEditor(this);
    editor->setText(content);

    StyleManager::setupWidgetScrollbars(editor);
    editor->refreshTheme();
    editor->setStyleSheet("QsciScintilla { background-color: #1e1e1e, border: none; outline: none; }");

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
    editorTab->setCurrentIndex(idx);

    setWindowTitle(QString(" - %1").arg(fileInfo.fileName()));
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