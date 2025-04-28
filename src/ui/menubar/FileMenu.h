
#ifndef FILEMENU_H
#define FILEMENU_H

#include <QMenu>
#include <QAction>

class FileMenu : public QMenu
{
    Q_OBJECT
    /* data */
public:
    explicit FileMenu(QWidget *parent = nullptr);
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
};

#endif


