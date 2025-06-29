
#pragma once
#include <QMainWindow>
#include "menubar/FileMenu.h"
#include "statusbar/StatusBar.h"
#include "../editor/CodeEditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    // Constructor
    explicit MainWindow(QWidget *parent = nullptr);

    // Destructor
    ~MainWindow() = default;

private slots:
    void onThemeChanged();

private:
    // Setup functions
    void setupMenuBar();
    void setupStatusBar();
    void setupEditor();
    void applyTheme();
    
    // UI elements
    StatusBar *statusBar;
    FileMenu *fileMenu;
    CodeEditor *editor;
};

