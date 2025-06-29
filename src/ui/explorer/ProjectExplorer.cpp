#include "ProjectExplorer.h"
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"

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

ProjectExplorer::ProjectExplorer(QWidget *parent)
    : QDockWidget("Explorer", parent), m_centralWidget(nullptr), m_mainLayout(nullptr), m_toolbarLayout(nullptr), m_openFolderButton(nullptr), m_collapseButton(nullptr), m_createFileButton(nullptr), m_pathEdit(nullptr), m_treeView(nullptr), m_fileSystemModel(nullptr)
{
    setupUI();
    setupFileSystemModel();
    applyTheme();

    // Connect theme changes
    connect(&Theme::instance(), &Theme::themeChanged, this, &ProjectExplorer::applyTheme);

    VOLT_UI("Project Explorer created successfully");
}

void ProjectExplorer::setupUI()
{
    // Create central widget
    m_centralWidget = new QWidget(this);
    setWidget(m_centralWidget);

    // Main layout
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(5);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);

    // Toolbar buttons
    m_toolbarLayout = new QHBoxLayout();
    m_openFolderButton = new QPushButton(QIcon::fromTheme("folder-open", QIcon(":/icons/folder-open.png")), QString(), this);
    m_collapseButton = new QPushButton(QIcon(":/icons/collapse_all.ico"), QString(), this);
    m_createFileButton = new QPushButton(QIcon(":/icons/new_file.ico"), QString(), this);

    m_openFolderButton->setToolTip("Open Folder");
    m_collapseButton->setToolTip("Collapse All");
    m_createFileButton->setToolTip("Create New File");

    m_openFolderButton->setFixedSize(24, 24);
    m_collapseButton->setFixedSize(24, 24);
    m_createFileButton->setFixedSize(24, 24);

    m_toolbarLayout->addStretch();
    m_toolbarLayout->addWidget(m_createFileButton);
    m_toolbarLayout->addWidget(m_openFolderButton);
    m_toolbarLayout->addWidget(m_collapseButton);

    // Path edit
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setPlaceholderText("No folder opened");
    m_pathEdit->setReadOnly(true);

    // Tree view
    m_treeView = new QTreeView(this);
    m_treeView->setHeaderHidden(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);

    // Add to layout
    m_mainLayout->addLayout(m_toolbarLayout);
    m_mainLayout->addWidget(m_pathEdit);
    m_mainLayout->addWidget(m_treeView);

    // Connect signals
    connect(m_openFolderButton, &QPushButton::clicked, this, &ProjectExplorer::openFolder);
    connect(m_collapseButton, &QPushButton::clicked, this, &ProjectExplorer::collapseAll);
    connect(m_createFileButton, &QPushButton::clicked, this, &ProjectExplorer::createNewFile);
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
        return;
    }

    QDir dir(path);
    if (!dir.exists())
    {
        VOLT_ERROR_F("Directory does not exist: %1", path);
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
    QColor bgColor = theme.getColor("explorer.background", QColor("#252526"));
    QColor fgColor = theme.getColor("explorer.foreground", QColor("#CCCCCC"));
    QColor borderColor = theme.getColor("explorer.border", QColor("#454545"));
    QColor selectionBg = theme.getColor("explorer.selectionBackground", QColor("#04395E"));
    QColor selectionFg = theme.getColor("explorer.selectionForeground", QColor("#FFFFFF"));
    QColor hoverBg = theme.getColor("explorer.hoverBackground", bgColor.lighter(110));

    // Get font
    QFont explorerFont = theme.getFont("explorer");
    if (explorerFont.family().isEmpty())
    {
        explorerFont = QFont("Segoe UI", 9);
    }

    // Apply to dock widget
    QString dockStyle = QString(R"(
        QDockWidget {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            titlebar-close-icon: url(close.png);
            titlebar-normal-icon: url(undock.png);
        }
        QDockWidget::title {
            background-color: %1;
            color: %2;
            padding: 5px;
            border-bottom: 1px solid %3;
            font-weight: bold;
        }
    )")
                            .arg(bgColor.name())
                            .arg(fgColor.name())
                            .arg(borderColor.name());

    setStyleSheet(dockStyle);

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
            padding: 3px;
            border: none;
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

    // Apply to buttons
    QString buttonStyle = QString(R"(
        QPushButton {
            background-color: transparent;
            color: %1;
            border: 1px solid transparent;
            padding: 2px;
            font-family: "%3";
            font-size: %4pt;
        }
        QPushButton:hover {
            background-color: %2;
            border: 1px solid %2;
        }
        QPushButton:pressed {
            background-color: %2;
        }
    )")
                              .arg(fgColor.name())
                              .arg(hoverBg.name())
                              .arg(explorerFont.family())
                              .arg(explorerFont.pointSize());

    m_openFolderButton->setStyleSheet(buttonStyle);
    m_collapseButton->setStyleSheet(buttonStyle);
    m_createFileButton->setStyleSheet(buttonStyle);

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