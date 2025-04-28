
#include "FileMenu.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QKeySequence>

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
    addSeparator(); // Extra separator for better organization
    addAction(exitApplicationAction); // Exit action at the bottom
}

void FileMenu::newFile(){
    // Logic to create a new file
    QMessageBox::information(this, tr("New File"), tr("New file created."));
}

void FileMenu::openFile(){
    // Logic to open a file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, tr("Open File"), tr("Opened file: %1").arg(fileName));
    }
}
void FileMenu::saveFile(){
    // Logic to save the current file
    QMessageBox::information(this, tr("Save File"), tr("File saved."));
}

void FileMenu::saveAsFile(){
    // Logic to save the current file with a new name
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, tr("Save File As"), tr("Saved file as: %1").arg(fileName));
    }
}



void FileMenu::exitApplication(){
    // Logic to exit the application
    QApplication::quit();
}