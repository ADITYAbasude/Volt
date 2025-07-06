#include "ProjectExplorer.h"
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"
#include "../utils/IconUtils.h"

#include <QFileDialog>
#include <QHeaderView>
#include <QApplication>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QTabWidget>

ProjectExplorer::ProjectExplorer(QWidget *parent)
    : QDockWidget("Sidebar", parent), 
    m_tabWidget(nullptr), 
    m_centralWidget(nullptr), 
    m_mainLayout(nullptr), 
    m_toolbarLayout(nullptr), 
    m_collapseButton(nullptr), 
    m_createFileButton(nullptr), 
    m_pathEdit(nullptr), 
    m_stackedWidget(nullptr), 
    m_explorerWidget(nullptr), 
    m_treeView(nullptr), 
    m_fileSystemModel(nullptr), 
    m_welcomeWidget(nullptr), 
    m_welcomeLabel(nullptr), 
    m_welcomeOpenFolderButton(nullptr), 
    m_searchWidget(nullptr), 
    m_sourceControlWidget(nullptr), 
    m_searchLabel(nullptr), 
    m_sourceControlLabel(nullptr),
    m_explorerTopBar(nullptr)
{
    setupUI();
    setupTabs();
    setupFileSystemModel();
    applyTheme();

    // Connect theme changes
    connect(&Theme::instance(), &Theme::themeChanged, this, &ProjectExplorer::applyTheme);

    VOLT_UI("Project Explorer created successfully");
}

void ProjectExplorer::setupUI()
{
    // Create the main tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setTabShape(QTabWidget::Rounded);
    m_tabWidget->setIconSize(QSize(20, 20));
    m_tabWidget->setLayoutDirection(Qt::LeftToRight);
    m_tabWidget->setDocumentMode(true);       // Remove borders around tabs
    m_tabWidget->setUsesScrollButtons(false); // Disable scroll buttons
    setWidget(m_tabWidget);

    // Ensure tabs are aligned left, not centered
    if (m_tabWidget->tabBar())
    {
        m_tabWidget->tabBar()->setLayoutDirection(Qt::LeftToRight);
        m_tabWidget->tabBar()->setExpanding(false);
    }

    VOLT_UI("Project Explorer UI setup completed");
}

void ProjectExplorer::setupTabs()
{
    // Create tabs
    createExplorerTab();
    createSearchTab();
    createSourceControlTab();

    VOLT_UI("Project Explorer tabs setup completed");
}

void ProjectExplorer::createExplorerTab()
{
    // Create the explorer widget
    m_explorerWidget = new QWidget();

    // Main layout for explorer
    m_mainLayout = new QVBoxLayout(m_explorerWidget);
    m_mainLayout->setSpacing(5);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);


    // Top bar layout: icon buttons left, stretch, path label right
    m_explorerTopBar = new QHBoxLayout();
    m_explorerTopBar->setSpacing(5);

    // Create icon buttons using the new IconButton component
    m_createFileButton = new IconButton(":/icons/new_file.svg", "Create New File", this);
    m_collapseButton = new IconButton(":/icons/collapse_all.svg", "Collapse All", this);

    m_explorerTopBar->addWidget(m_createFileButton);
    m_explorerTopBar->addWidget(m_collapseButton);
    m_explorerTopBar->addStretch();

    // Path label (right side)
    m_pathEdit = new QLabel("No folder opened", this);
    m_pathEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_explorerTopBar->addWidget(m_pathEdit);


    // Stacked widget
    m_stackedWidget = new QStackedWidget(this);

    // Tree view
    m_treeView = new QTreeView(this);
    m_treeView->setHeaderHidden(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);

    // Welcome screen
    m_welcomeWidget = new QWidget(this);
    QVBoxLayout *welcomeLayout = new QVBoxLayout(m_welcomeWidget);
    m_welcomeLabel = new QLabel("No Folder Opened", this);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_welcomeOpenFolderButton = new FilledColorButton("Open Folder", FilledColorButton::Primary, this);
    welcomeLayout->addStretch();
    welcomeLayout->addWidget(m_welcomeLabel);
    welcomeLayout->addWidget(m_welcomeOpenFolderButton);
    welcomeLayout->addStretch();

    m_stackedWidget->addWidget(m_welcomeWidget);
    m_stackedWidget->addWidget(m_treeView);


    // Add to layout: top bar, then stacked widget
    m_mainLayout->addLayout(m_explorerTopBar);
    m_mainLayout->addWidget(m_stackedWidget);

    // Connect signals
    connect(m_welcomeOpenFolderButton, &QPushButton::clicked, this, &ProjectExplorer::openFolder);
    connect(m_collapseButton, &QPushButton::clicked, this, &ProjectExplorer::collapseAll);
    connect(m_createFileButton, &QPushButton::clicked, this, &ProjectExplorer::createNewFile);

    // Add to tab widget

    m_tabWidget->addTab(m_explorerWidget, IconUtils::loadSvgIcon(":/icons/folder.svg", 20), "");
    m_tabWidget->setTabToolTip(0, "Explorer");
    m_tabWidget->setCurrentIndex(0);
    showWelcomeScreen();
}

void ProjectExplorer::createSearchTab()
{
    // Create the search widget
    m_searchWidget = new QWidget();
    QVBoxLayout *searchLayout = new QVBoxLayout(m_searchWidget);

    m_searchLabel = new QLabel("Search functionality coming soon...", m_searchWidget);
    m_searchLabel->setAlignment(Qt::AlignCenter);
    m_searchLabel->setWordWrap(true);

    searchLayout->addStretch();
    searchLayout->addWidget(m_searchLabel);
    searchLayout->addStretch();

    // Add to tab widget
    m_tabWidget->addTab(m_searchWidget, IconUtils::loadSvgIcon(":/icons/search.svg", 20), "");
    m_tabWidget->setTabToolTip(1, "Search");
}

void ProjectExplorer::createSourceControlTab()
{
    // Create the source control widget
    m_sourceControlWidget = new QWidget();
    QVBoxLayout *sourceControlLayout = new QVBoxLayout(m_sourceControlWidget);

    m_sourceControlLabel = new QLabel("Source Control functionality coming soon...", m_sourceControlWidget);
    m_sourceControlLabel->setAlignment(Qt::AlignCenter);
    m_sourceControlLabel->setWordWrap(true);

    sourceControlLayout->addStretch();
    sourceControlLayout->addWidget(m_sourceControlLabel);
    sourceControlLayout->addStretch();

    // Add to tab widget
    // m_tabWidget->addTab(m_sourceControlWidget, QIcon(":/icons/git.svg"), "");
    // m_tabWidget->setTabToolTip(2, "Source Control");
}

void ProjectExplorer::setupFileSystemModel()
{
    m_fileSystemModel = new QFileSystemModel(this);
    m_fileSystemModel->setRootPath("");

    // Set filters to show all files and folders
    m_fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);

    // Connect model to tree view
    m_treeView->setModel(m_fileSystemModel);

    // Hide extra columns (only show name)
    m_treeView->hideColumn(1); // Size
    m_treeView->hideColumn(2); // Type
    m_treeView->hideColumn(3); // Date Modified

    // Connect tree view signals
    connect(m_treeView, &QTreeView::doubleClicked, this, &ProjectExplorer::onItemDoubleClicked);
    connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ProjectExplorer::onSelectionChanged);
}

void ProjectExplorer::setRootPath(const QString &path)
{
    if (path.isEmpty())
    {
        showWelcomeScreen();
        return;
    }

    QDir dir(path);
    if (!dir.exists())
    {
        VOLT_ERROR_F("Directory does not exist: %1", path);
        showWelcomeScreen();
        return;
    }

    m_currentRootPath = dir.absolutePath();

    // Set the model root path
    QModelIndex rootIndex = m_fileSystemModel->setRootPath(m_currentRootPath);
    m_treeView->setRootIndex(rootIndex);

    // Update path display
    m_pathEdit->setText(m_currentRootPath);

    // Expand the root
    m_treeView->expand(rootIndex);

    VoltLogger::instance().info("Project Explorer root set to: %1", m_currentRootPath);
    emit folderChanged(m_currentRootPath);

    showTreeView();
}

void ProjectExplorer::showWelcomeScreen()
{
    m_stackedWidget->setCurrentWidget(m_welcomeWidget);
    m_createFileButton->setEnabled(false);
    m_collapseButton->setEnabled(false);
    m_pathEdit->setText("No folder opened");
}

void ProjectExplorer::showTreeView()
{
    m_stackedWidget->setCurrentWidget(m_treeView);
    m_createFileButton->setEnabled(true);
    m_collapseButton->setEnabled(true);
}

QString ProjectExplorer::currentRootPath() const
{
    return m_currentRootPath;
}

void ProjectExplorer::openFolder()
{
    QString folderPath = QFileDialog::getExistingDirectory(
        this,
        "Open Folder",
        m_currentRootPath.isEmpty() ? QDir::homePath() : m_currentRootPath);

    if (!folderPath.isEmpty())
    {
        setRootPath(folderPath);
    }
}

void ProjectExplorer::collapseAll()
{
    m_treeView->collapseAll();
    VOLT_UI("Project Explorer: Collapsed all folders");
}

void ProjectExplorer::onItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }

    QString filePath = m_fileSystemModel->filePath(index);
    QFileInfo fileInfo(filePath);

    if (fileInfo.isFile())
    {
        VoltLogger::instance().info("Project Explorer: File double-clicked: %1", filePath);
        emit fileDoubleClicked(filePath);
    }
    else if (fileInfo.isDir())
    {
        // Toggle expand/collapse for folders
        if (m_treeView->isExpanded(index))
        {
            m_treeView->collapse(index);
        }
        else
        {
            m_treeView->expand(index);
        }
        VoltLogger::instance().info("Project Explorer: Folder toggled: %1", filePath);
    }
}

void ProjectExplorer::onSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)

    if (!current.isValid())
    {
        return;
    }

    QString filePath = m_fileSystemModel->filePath(current);
    QFileInfo fileInfo(filePath);

    VOLT_TRACE_F("Project Explorer: Selection changed to: %1", filePath);
}

void ProjectExplorer::applyTheme()
{
    Theme &theme = Theme::instance();

    // Get colors from theme
    QColor bgColor = theme.getColor("explorer.background");
    QColor fgColor = theme.getColor("explorer.foreground");
    QColor borderColor = theme.getColor("explorer.border");
    QColor selectionBg = theme.getColor("explorer.selectionBackground");
    QColor selectionFg = theme.getColor("explorer.selectionForeground");
    QColor hoverBg = theme.getColor("explorer.hoverBackground");

    // primary color for selection
    QColor primaryColor = theme.getColor("primary");

    if (primaryColor.isValid() && primaryColor.name().isEmpty())
    {
        primaryColor = QColor("#007ACC"); // Default primary color if not set
    }

    // Get font
    QFont explorerFont = theme.getFont("explorer");
    if (explorerFont.family().isEmpty())
    {
        explorerFont = QFont("Segoe UI", 9);
    }

    // Apply to dock widget
    QString tabStyle = QString(R"(
    QTabWidget::pane {
        border: none;
        background-color: %1;
        border-top: 1px solid %9;
    }
    QTabWidget::tab-bar {
        alignment: left;
        align-items: left;
        left: 0px;
    }
    QTabBar {
        background: %1;
        border: none;
        alignment: left;
        align-items: left;
        qproperty-drawBase: 0;
        subcontrol-position: top;
        qproperty-expanding: false;
        text-align: left;
    }
    QTabBar::tab {
        background: %1;
        color: %2;
        border: none;
        min-width: 26px;
        max-width: 26px;
        min-height: 26px;
        max-height: 26px;
        margin: 0;
        padding: 0 0 0 2px;
        border-radius: 0;
    }
    QTabBar::tab:selected {
        background: %1;
        color: #FFFFFF;
        border-bottom: 1px solid %9;
    }
    QTabBar::tab:hover:!selected {
        background: %1;
        color: %2;
    }
    QTabBar::tab:!selected {
        opacity: 0.6;
        background: %1;
        color: %2;
    }
    QTabBar::tab:first {
        margin-left: 0;
    }
    QTabBar::tab:last {
        margin-right: 0;
    }
    QTabBar::close-button {
        image: none;
        subcontrol-position: right;
    }
)")
                           .arg(bgColor.name())                 // %1 - content background
                           .arg(fgColor.name())                 // %2 - icon color
                           .arg(borderColor.name())             // %3 - border (not used)
                           .arg(selectionBg.name())             // %4 - selection background (not used)
                           .arg(selectionFg.name())             // %5 - selection icon color
                           .arg(hoverBg.name())                 // %6 - hover background
                           .arg(borderColor.darker(250).name()) // %7 - tab bar background (darker for sidebar)
                           .arg(borderColor.name())             // %8 - border color
                           .arg(primaryColor.name());           // %9 - primary color for selection

    m_tabWidget->setStyleSheet(tabStyle);

    // Apply to tree view
    QString treeStyle = QString(R"(
        QTreeView {
            background-color: %1;
            color: %2;
            border: none;
            outline: none;
            font-family: "%6";
            font-size: %7pt;
        }
        QTreeView::item {
            padding: 5px;
            border-radius: 3px;
        }
        QTreeView::item:hover {
            background-color: %5;
        }
        QTreeView::item:selected {
            background-color: %3;
            color: %4;
        }
        QTreeView::branch:closed:has-children {
            image: url(:/icons/folder-closed.png);
        }
        QTreeView::branch:open:has-children {
            image: url(:/icons/folder-open.png);
        }
    )")
                            .arg(bgColor.name())            // %1 - background
                            .arg(fgColor.name())            // %2 - foreground
                            .arg(selectionBg.name())        // %3 - selection background
                            .arg(selectionFg.name())        // %4 - selection foreground
                            .arg(hoverBg.name())            // %5 - hover background
                            .arg(explorerFont.family())     // %6 - font family
                            .arg(explorerFont.pointSize()); // %7 - font size

    m_treeView->setStyleSheet(treeStyle);
    m_treeView->setIndentation(15);

    // Apply to path edit
    QString pathStyle = QString(R"(
        QLineEdit {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            padding: 3px;
            font-family: "%4";
            font-size: %5pt;
        }
    )")
                            .arg(bgColor.darker(105).name())
                            .arg(fgColor.name())
                            .arg(borderColor.name())
                            .arg(explorerFont.family())
                            .arg(explorerFont.pointSize());

    m_pathEdit->setStyleSheet(pathStyle);

    // Apply to welcome label
    m_welcomeLabel->setFont(explorerFont);
    m_welcomeLabel->setStyleSheet(QString("color: %1;").arg(fgColor.name()));

    // Apply to other tab labels
    if (m_searchLabel)
    {
        m_searchLabel->setFont(explorerFont);
        m_searchLabel->setStyleSheet(QString("color: %1;").arg(fgColor.name()));
    }

    if (m_sourceControlLabel)
    {
        m_sourceControlLabel->setFont(explorerFont);
        m_sourceControlLabel->setStyleSheet(QString("color: %1;").arg(fgColor.name()));
    }

    // The IconButton and FilledColorButton components handle their own styling

    VOLT_UI("Project Explorer theme applied successfully");
}

void ProjectExplorer::createNewFile()
{
    if (m_currentRootPath.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Please open a folder first before creating a new file.");
        return;
    }

    // Get file name from user
    bool ok;
    QString fileName = QInputDialog::getText(this, "Create New File",
                                             "Enter file name:", QLineEdit::Normal,
                                             "untitled.txt", &ok);

    if (!ok || fileName.isEmpty())
    {
        return;
    }

    // Create full file path
    QString filePath = QDir(m_currentRootPath).absoluteFilePath(fileName);

    // Check if file already exists
    if (QFile::exists(filePath))
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "File Exists",
            QString("File '%1' already exists. Do you want to overwrite it?").arg(fileName),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (reply != QMessageBox::Yes)
        {
            return;
        }
    }

    // Create the file
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Write a basic template based on file extension
        QTextStream stream(&file);
        QString extension = QFileInfo(fileName).suffix().toLower();

        if (extension == "cpp" || extension == "cxx" || extension == "cc")
        {
            stream << "#include <iostream>\n\n";
            stream << "int main() {\n";
            stream << "    std::cout << \"Hello, World!\" << std::endl;\n";
            stream << "    return 0;\n";
            stream << "}\n";
        }
        else if (extension == "h" || extension == "hpp")
        {
            QString headerGuard = fileName.toUpper().replace(".", "_").replace("-", "_");
            stream << "#pragma once\n\n";
            stream << "#ifndef " << headerGuard << "\n";
            stream << "#define " << headerGuard << "\n\n";
            stream << "// Your code here\n\n";
            stream << "#endif // " << headerGuard << "\n";
        }
        else if (extension == "html")
        {
            stream << "<!DOCTYPE html>\n";
            stream << "<html>\n<head>\n    <title>Document</title>\n</head>\n";
            stream << "<body>\n    <h1>Hello, World!</h1>\n</body>\n</html>\n";
        }
        else if (extension == "css")
        {
            stream << "/* CSS Stylesheet */\n\n";
            stream << "body {\n";
            stream << "    font-family: Arial, sans-serif;\n";
            stream << "    margin: 0;\n";
            stream << "    padding: 0;\n";
            stream << "}\n";
        }
        else if (extension == "js")
        {
            stream << "// JavaScript\n\n";
            stream << "console.log('Hello, World!');\n";
        }
        else if (extension == "py")
        {
            stream << "#!/usr/bin/env python3\n\n";
            stream << "def main():\n";
            stream << "    print('Hello, World!')\n\n";
            stream << "if __name__ == '__main__':\n";
            stream << "    main()\n";
        }
        else if (extension == "json")
        {
            stream << "{\n";
            stream << "    \"name\": \"value\"\n";
            stream << "}\n";
        }
        else if (extension == "md")
        {
            stream << "# " << QFileInfo(fileName).baseName() << "\n\n";
            stream << "This is a markdown file.\n";
        }
        else
        {
            // Plain text file - create empty
            stream << "";
        }

        file.close();

        // Emit signal that file was created
        emit fileCreated(filePath);

        // Log success
        VOLT_INFO_F("Project Explorer: Created new file: %1", filePath);

        // Show success message
        QMessageBox::information(this, "Success",
                                 QString("File '%1' created successfully!").arg(fileName));

        // Refresh the tree view to show the new file
        m_fileSystemModel->dataChanged(m_fileSystemModel->index(0, 0), m_fileSystemModel->index(m_fileSystemModel->rowCount() - 1, 0));
    }
    else
    {
        // Log and show error
        QString errorMsg = QString("Failed to create file: %1").arg(file.errorString());
        VOLT_ERROR_F("Project Explorer: %1", errorMsg);
        QMessageBox::critical(this, "Error", errorMsg);
    }
}