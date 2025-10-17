
#include "FileMenu.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTabWidget>
#include <QKeySequence>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include "./logging/VoltLogger.h"
#include "../MainWindow.h"
#include "../../editor/CodeEditor.h"

FileMenu::FileMenu(QWidget *parent) : QMenu("File", parent)
{
    // Create actions
    newFileAction = new QAction("New", this);
    openFileAction = new QAction("Open", this);
    saveFileAction = new QAction("Save", this);
    saveAsFileAction = new QAction("Save As", this);
    exitApplicationAction = new QAction("Exit", this);

    // Set shortcuts
    newFileAction->setShortcut(QKeySequence::New);
    openFileAction->setShortcut(QKeySequence::Open);
    saveFileAction->setShortcut(QKeySequence::Save);
    saveAsFileAction->setShortcut(QKeySequence::SaveAs);
    exitApplicationAction->setShortcut(QKeySequence::Quit);

    // Connect actions to slots
    connect(newFileAction, &QAction::triggered, this, &FileMenu::newFile);
    connect(openFileAction, &QAction::triggered, this, &FileMenu::openFile);
    connect(saveFileAction, &QAction::triggered, this, &FileMenu::saveFile);
    connect(saveAsFileAction, &QAction::triggered, this, &FileMenu::saveAsFile);
    connect(exitApplicationAction, &QAction::triggered, this, &FileMenu::exitApplication);

    // set status tip for actions
    newFileAction->setStatusTip("Create a new file");
    openFileAction->setStatusTip("Open an existing file");
    saveFileAction->setStatusTip("Save the current file");
    saveAsFileAction->setStatusTip("Save the current file with a new name");
    exitApplicationAction->setStatusTip("Exit the application");

    // Add actions to the menu
    addAction(newFileAction);
    addSeparator();
    addAction(openFileAction);
    addSeparator();
    addAction(saveFileAction);
    addSeparator();
    addAction(saveAsFileAction);
    addSeparator();
    addSeparator();
    addAction(exitApplicationAction);
}

void FileMenu::newFile()
{
    QMessageBox::information(this, tr("New File"), tr("New file created."));
}

void FileMenu::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty() && mainWindow)
    {
        mainWindow->openFile(fileName);
    }
}

/*
 * Sets the main window pointer for the FileMenu to interact with the main application window.
 * This allows the FileMenu to access and manipulate elements of the main window, such as
 * the central widget or other UI components.
 */
void FileMenu::setMainWindow(MainWindow *mw)
{
    mainWindow = mw;
}

/*
 * Saves the current file.
 * If no file is open, prompts the user to use "Save As" instead.
 */
void FileMenu::saveFile()
{
    if (!mainWindow)
    {
        VOLT_INFO("Main window pointer is null in FileMenu::saveFile");
        return;
    }
    QTabWidget *tabWidget = mainWindow->findChild<QTabWidget *>();
    if (!tabWidget || tabWidget->count() == 0)
    {
        QMessageBox::warning(this, tr("Save File"), tr("No file is currently open to save."));
        return;
    }

    int currentIndex = tabWidget->currentIndex();
    QWidget *widget = tabWidget->widget(currentIndex);

    QString currentPath;
    if (tabWidget->tabBar())
    {
        QVariant data = tabWidget->tabBar()->tabData(currentIndex);
        currentPath = data.toString();
    }

    if (currentPath.isEmpty())
    {
        // No file path associated, prompt Save As
        saveAsFile();
        return;
    }

    CodeEditor *editor = nullptr;
    if (widget)
    {
        editor = widget->findChild<CodeEditor *>();
    }

    if (!editor)
    {
        VOLT_INFO("No CodeEditor found in the current tab during Save operation.");
        return;
    }

    QString fileContent = editor->text();

    /*
     * save the changed file content to the existing file path.
     * If the file cannot be opened for writing, an error message is displayed
     * to the user and an error is logged.
     */
    QFile file(currentPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString errorMsg = QString("Failed to save file: %1\nError: %2").arg(currentPath, file.errorString());
        QMessageBox::critical(this, "Error", errorMsg);
        VOLT_ERROR_F("FileMenu: %1", errorMsg);
        return;
    }

    QTextStream out(&file);
    out << fileContent;
    file.close();
}

/*
 * Saves the current file with a new name specified by the user.
 * Opens a file dialog to get the new file name and writes the content
 * of the current editor to the specified file.
 */

void FileMenu::saveAsFile()
{

    if (!mainWindow)
    {
        VOLT_INFO("Main window pointer is null in FileMenu::saveAsFile");
        return;
    }
    QTabWidget *tabWidget = mainWindow->findChild<QTabWidget *>();
    if (!tabWidget || tabWidget->count() == 0)
    {
        QMessageBox::warning(this, tr("Save File As"), tr("No file is currently open to save."));
        return;
    }

    int currentIndex = tabWidget->currentIndex();
    QWidget *widget = tabWidget->widget(currentIndex);

    QString currentPath;
    if (tabWidget->tabBar())
    {
        QVariant data = tabWidget->tabBar()->tabData(currentIndex);
        currentPath = data.toString();
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"), currentPath, tr("Text Files (*.txt);;All Files (*);;Cpp Files (*.cpp *.h)"));
    if (fileName.isEmpty())
    {
        return;
    }

    CodeEditor *editor = nullptr;
    if (widget)
    {
        editor = widget->findChild<CodeEditor *>();
    }

    if (!editor)
    {
        VOLT_INFO("No CodeEditor found in the current tab during Save As operation.");
        return;
    }

    QString fileContent = editor->text();

    /*
     * save the changed file content to the specified file path.
     * If the file cannot be opened for writing, an error message is displayed
     * to the user and an error is logged.
     */
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString errorMsg = QString("Failed to save file: %1\nError: %2").arg(fileName, file.errorString());
        QMessageBox::critical(this, "Error", errorMsg);
        VOLT_ERROR_F("FileMenu: %1", errorMsg);
        return;
    }

    QTextStream out(&file);
    out << fileContent;
    file.close();

    /*
     * Update the tab title and data to reflect the new file name and path.
     */
    QFileInfo fileInfo(fileName);
    tabWidget->tabBar()->setTabText(currentIndex, fileInfo.fileName());
    tabWidget->tabBar()->setTabData(currentIndex, fileName);
    // if (tabWidget->tabBar())
    // {
    //     tabWidget->tabBar()->setTabData(currentIndex, fileName);
    // }
}

void FileMenu::exitApplication()
{
    QApplication::quit();
}