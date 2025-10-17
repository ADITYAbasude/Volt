
#pragma once
#include <QMenu>
#include <QAction>

class MainWindow;

class FileMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FileMenu(QWidget *parent = nullptr);
    void setMainWindow(MainWindow *mw);
    ~FileMenu() = default;

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void exitApplication();

private:
    QAction *newFileAction;
    QAction *openFileAction;
    QAction *saveFileAction;
    QAction *saveAsFileAction;
    QAction *exitApplicationAction;

    MainWindow *mainWindow;
};