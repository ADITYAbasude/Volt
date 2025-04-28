#include "MainWindow.h"
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){

    setWindowTitle("Volt Editor");
    resize(800, 600);
    setupMenuBar();
}

void MainWindow::setupMenuBar() {
    fileMenu = new FileMenu(this);
    menuBar()->addMenu(fileMenu);
}