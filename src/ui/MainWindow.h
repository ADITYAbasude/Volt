
#pragma once
#include <QMainWindow>
#include "menubar/FileMenu.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private:
    void setupMenuBar();
    FileMenu *fileMenu;
};

