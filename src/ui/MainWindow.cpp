#include "MainWindow.h"
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    setWindowTitle("Volt Editor");
    resize(800, 600);
    setupMenuBar();
    setupStatusBar();
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