#pragma once

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDir>
#include <QStackedWidget>
#include <QTabWidget>
#include <QFont>
#include "../components/IconButton.h"
#include "../components/FilledColorButton.h"

class Sidebar : public QDockWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);
    ~Sidebar() = default;

    void setRootPath(const QString &path);
    QString currentRootPath() const;

public slots:
    void applyTheme();
    void openFolder();
    void collapseAll();
    void createNewFile();

signals:
    void fileDoubleClicked(const QString &filePath);
    void folderChanged(const QString &folderPath);
    void fileCreated(const QString &filePath);

private slots:
    void onItemDoubleClicked(const QModelIndex &index);
    void onSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void setupUI();
    void setupFileSystemModel();
    void setupTabs();
    void createExplorerTab();
    void createSearchTab();
    void createSourceControlTab();
    void showWelcomeScreen();
    void showTreeView();

    // UI Components
    QTabWidget *m_tabWidget;
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_explorerTopBar;
    QHBoxLayout *m_toolbarLayout;

    IconButton *m_collapseButton;
    IconButton *m_createFileButton;
    QLabel *m_pathEdit;
    
    QStackedWidget *m_stackedWidget;

    // Explorer Tab Components
    QWidget *m_explorerWidget;

    // Tree View
    QTreeView *m_treeView;
    QFileSystemModel *m_fileSystemModel;

    // Welcome Screen
    QWidget *m_welcomeWidget;
    QLabel *m_welcomeLabel;
    FilledColorButton *m_welcomeOpenFolderButton;

    // Other Tab Components
    QWidget *m_searchWidget;
    QWidget *m_sourceControlWidget;
    QLabel *m_searchLabel;
    QLabel *m_sourceControlLabel;

    QString m_currentRootPath;

    QFont m_font;
};
