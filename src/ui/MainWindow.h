
#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include "menubar/FileMenu.h"
#include "statusbar/StatusBar.h"
#include "Sidebar/Sidebar.h"
#include "../editor/CodeEditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    // Constructor
    explicit MainWindow(QWidget *parent = nullptr);

    // Destructor
    ~MainWindow() = default;
    
    // Public methods
    void openFile(const QString &filePath);
    void openFolder(const QString &folderPath);

private slots:
    void onThemeChanged();
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);
    void onTabContextMenuRequested(const QPoint &pos);
    

private:
    // Setup functions
    void setupMenuBar();
    void setupStatusBar();
    void setupEditor();
    void setupSidebar();
    void applyTheme();
    
    // UI elements
    StatusBar *statusBar;
    FileMenu *fileMenu;
    QTabWidget *editorTab;
    Sidebar *sidebar;
};

