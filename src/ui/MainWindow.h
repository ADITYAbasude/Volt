
#pragma once
#include <QMainWindow>
#include "menubar/FileMenu.h"
#include "statusbar/StatusBar.h"
#include "explorer/ProjectExplorer.h"
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

private:
    // Setup functions
    void setupMenuBar();
    void setupStatusBar();
    void setupEditor();
    void setupProjectExplorer();
    void applyTheme();
    
    // UI elements
    StatusBar *statusBar;
    FileMenu *fileMenu;
    CodeEditor *editor;
    ProjectExplorer *projectExplorer;
};

