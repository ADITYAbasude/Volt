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

class ProjectExplorer : public QDockWidget
{
    Q_OBJECT

public:
    explicit ProjectExplorer(QWidget *parent = nullptr);
    ~ProjectExplorer() = default;

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
    
    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_toolbarLayout;
    
    QPushButton *m_openFolderButton;
    QPushButton *m_collapseButton;
    QPushButton *m_createFileButton;
    QLineEdit *m_pathEdit;
    
    QTreeView *m_treeView;
    QFileSystemModel *m_fileSystemModel;
    
    QString m_currentRootPath;
};
