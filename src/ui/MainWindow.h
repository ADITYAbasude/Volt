
#pragma once
#include <QMainWindow>
#include "menubar/FileMenu.h"
#include "statusbar/StatusBar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    // Constructor
    explicit MainWindow(QWidget *parent = nullptr);

    // Destructor
    ~MainWindow() = default;

private:
    // Setup functions
    void setupMenuBar();
    void setupStatusBar();
    
    // UI elements
    StatusBar *statusBar;
    FileMenu *fileMenu;
};

