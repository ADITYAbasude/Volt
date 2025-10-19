
#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include "statusbar/StatusBar.h"
#include "sidebar/Sidebar.h"
#include "components/CustomTabWidget.h"
#include "../editor/CodeEditor.h"

class FileMenu;

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

signals:
    void fileModified();

private slots:
    void onThemeChanged();
    void onTabCloseRequested(int index);
    void onCurrentTabChanged(int index);
    void onTabContextMenuRequested(const QPoint &pos);
    void onFileModificationChanged(bool hasChanges);


private:
    void updateTabModified(int tabIndex, bool hasUnsavedChanges);
    int getModifiedFileCount() const;
    
    // Setup functions
    void setupMenuBar();
    void setupStatusBar();
    void setupEditor();
    void setupSidebar();
    void applyTheme();
    
    // UI elements
    StatusBar *statusBar;
    FileMenu *fileMenu;
    CustomTabWidget *editorTab;
    Sidebar *sidebar;
};

