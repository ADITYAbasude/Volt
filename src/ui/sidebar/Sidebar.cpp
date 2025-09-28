#include "Sidebar.h"
#include "CustomTreeView.h"
#include "../../themes/Theme.h"
#include "../../logging/VoltLogger.h"
#include "../utils/IconUtils.h"
#include "../components/CustomTabBar.h"
#include "../components/CustomTabWidget.h"

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
#include <QChar>
#include <QPainter>

Sidebar::Sidebar(QWidget *parent)
    : QDockWidget(parent),
      m_tabWidget(nullptr),
      m_centralWidget(nullptr),
      m_mainLayout(nullptr),
      m_toolbarLayout(nullptr),
      m_collapseButton(nullptr),
      m_createFileButton(nullptr),
      m_pathEdit(nullptr),
      m_stackedWidget(nullptr),
      m_explorerWidget(nullptr),
      m_treeView(new CustomTreeView(this)),
      m_fileSystemModel(nullptr),
      m_welcomeWidget(nullptr),
      m_welcomeLabel(nullptr),
      m_welcomeOpenFolderButton(nullptr),
      m_searchWidget(nullptr),
      m_sourceControlWidget(nullptr),
      m_searchLabel(nullptr),
      m_sourceControlLabel(nullptr),
      m_explorerTopBar(nullptr),
      m_font("icons-carbon")
{
    setTitleBarWidget(new QWidget());
    setupUI();
    setupTabs();
    setupFileSystemModel();
    applyTheme();

    // Connect theme changes
    connect(&Theme::instance(), &Theme::themeChanged, this, &Sidebar::applyTheme);

    VOLT_UI("Project Explorer created successfully");
}

void Sidebar::setupUI()
{

    m_tabWidget = new CustomTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setTabShape(QTabWidget::Rounded);
    m_tabWidget->setIconSize(QSize(20, 24));
    m_tabWidget->setLayoutDirection(Qt::LeftToRight);
    
    QTabBar *customBar = new CustomTabBar(m_tabWidget);
    m_tabWidget->setCustomTabBar(customBar);
    setWidget(m_tabWidget);
    
    // Ensure tabs are aligned left, not centered
    if (m_tabWidget->tabBar())
    {
        m_tabWidget->tabBar()->setLayoutDirection(Qt::LeftToRight);
        m_tabWidget->tabBar()->setExpanding(false);
    }
}

void Sidebar::setupTabs()
{
    createExplorerTab();
    createSearchTab();

    VOLT_UI("Project Explorer tabs setup completed");
}

void Sidebar::createExplorerTab()
{
    m_explorerWidget = new QWidget();

    m_mainLayout = new QVBoxLayout(m_explorerWidget);
    m_explorerTopBar = new QHBoxLayout();

    m_mainLayout->addStretch();

    qreal devicePixelRatio = this->devicePixelRatio();
    // Create high-DPI pixmap for crisp icons
    QPixmap newFileIcon(QSize(24, 24) * devicePixelRatio);
    newFileIcon.setDevicePixelRatio(devicePixelRatio);
    newFileIcon.fill(Qt::transparent);
    {
        QPainter painter(&newFileIcon);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QFont carbonFont = m_font;
        carbonFont.setPixelSize(18); 
        carbonFont.setHintingPreference(QFont::PreferNoHinting);

        painter.setFont(carbonFont);
        painter.setPen(QColor("#CCCCCC"));

        QChar newFileChar = Theme::instance().getCarbonIconChar("symbol-file");
        if (!newFileChar.isNull())
        {
            VOLT_DEBUG_F("Using Carbon icon for new file: U+%1 char: %2", QString::number(newFileChar.unicode(), 16).rightJustified(4, '0').toUpper().append(" ").append(newFileChar));
            QRect iconRect = newFileIcon.rect();
            if (devicePixelRatio > 1.0) {
                iconRect = QRect(0, 0, 24, 24);
            }
            painter.drawText(iconRect, Qt::AlignCenter | Qt::AlignVCenter, newFileChar);
        }
    }
    m_createFileButton = new IconButton("Create New File", this);
    m_collapseButton = new IconButton(":/icons/collapse_all.svg", "Collapse All", this);
    
    m_pathEdit = new QLabel("No folder opened", this);
    m_pathEdit->setWordWrap(true);
    m_pathEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_createFileButton->setIcon(QIcon(newFileIcon));

    m_explorerTopBar->addWidget(m_createFileButton);
    m_explorerTopBar->addWidget(m_collapseButton);
    m_explorerTopBar->addStretch();

    m_explorerTopBar->addWidget(m_pathEdit);

    // Stacked widget
    m_stackedWidget = new QStackedWidget(this);

    // Tree view
    m_treeView = new CustomTreeView(this);
    m_treeView->setHeaderHidden(true);
    m_treeView->setRootIsDecorated(true);
    m_treeView->setAlternatingRowColors(false);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_treeView->setExpandsOnDoubleClick(false);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

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

    m_mainLayout->addLayout(m_explorerTopBar);
    m_mainLayout->addWidget(m_stackedWidget);
    m_mainLayout->addStretch();

    // Connect signals
    connect(m_welcomeOpenFolderButton, &QPushButton::clicked, this, &Sidebar::openFolder);
    connect(m_collapseButton, &QPushButton::clicked, this, &Sidebar::collapseAll);
    connect(m_createFileButton, &QPushButton::clicked, this, &Sidebar::createNewFile);

    QPixmap folderIcon(QSize(24, 24) * devicePixelRatio);
    folderIcon.setDevicePixelRatio(devicePixelRatio);
    folderIcon.fill(Qt::transparent);
    {
        QPainter painter(&folderIcon);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QFont carbonFont = m_font;
        carbonFont.setPixelSize(18); 
        carbonFont.setHintingPreference(QFont::PreferNoHinting);
        painter.setFont(carbonFont);
        painter.setPen(QColor("#CCCCCC"));

        QChar folderChar = Theme::instance().getCarbonIconChar("symbol-folder");
        if (!folderChar.isNull())
        {
            QRect iconRect = folderIcon.rect();
            if (devicePixelRatio > 1.0) {
                iconRect = QRect(0, 0, 24, 24);
            }
            painter.drawText(iconRect, Qt::AlignCenter | Qt::AlignVCenter, folderChar);
        }
    }

    m_tabWidget->addTab(m_explorerWidget, QIcon(folderIcon), "");
    m_tabWidget->setTabToolTip(0, "Explorer");
    m_tabWidget->setCurrentIndex(0);
    showWelcomeScreen();
}

void Sidebar::createSearchTab()
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

    // Create high-DPI pixmap for crisp icons
    qreal devicePixelRatio = this->devicePixelRatio();
    QPixmap searchIcon(QSize(24, 24) * devicePixelRatio);
    searchIcon.setDevicePixelRatio(devicePixelRatio);
    searchIcon.fill(Qt::transparent);
    QPainter painter(&searchIcon);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QFont carbonFont = m_font;
    carbonFont.setPixelSize(18); 
    carbonFont.setHintingPreference(QFont::PreferNoHinting);
    painter.setFont(carbonFont);
    painter.setPen(QColor("#CCCCCC"));

    QChar searchChar = Theme::instance().getCarbonIconChar("search");
    if (!searchChar.isNull())
    {
        VOLT_DEBUG_F("Using Carbon icon for search: U+%1 char: %2", QString::number(searchChar.unicode(), 16).rightJustified(4, '0').toUpper().append(" ").append(searchChar));
        // Use proper bounding rectangle for centering
        QRect iconRect = searchIcon.rect();
        if (devicePixelRatio > 1.0) {
            iconRect = QRect(0, 0, 24, 24);
        }
        painter.drawText(iconRect, Qt::AlignCenter | Qt::AlignVCenter, searchChar);
    }

    // Add to tab widget
    m_tabWidget->addTab(m_searchWidget, QIcon(searchIcon), "");
    m_tabWidget->setTabToolTip(1, "Search");
}

void Sidebar::setupFileSystemModel()
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
    connect(m_treeView, &QTreeView::doubleClicked, this, &Sidebar::onItemDoubleClicked);
    connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &Sidebar::onSelectionChanged);
}

void Sidebar::setRootPath(const QString &path)
{
    if (path.isEmpty())
    {
        showWelcomeScreen();
        return;
    }

    QDir dir(path);
    if (!dir.exists())
    {
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

void Sidebar::showWelcomeScreen()
{
    m_stackedWidget->setCurrentWidget(m_welcomeWidget);
    m_createFileButton->setEnabled(false);
    m_collapseButton->setEnabled(false);
    m_createFileButton->setVisible(false);
    m_collapseButton->setVisible(false);
    m_pathEdit->setVisible(false);

}

void Sidebar::showTreeView()
{
    m_stackedWidget->setCurrentWidget(m_treeView);
    m_createFileButton->setEnabled(true);
    m_collapseButton->setEnabled(true);
    m_createFileButton->setVisible(true);
    m_collapseButton->setVisible(true);
}

QString Sidebar::currentRootPath() const
{
    return m_currentRootPath;
}

void Sidebar::openFolder()
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

void Sidebar::collapseAll()
{
    m_treeView->collapseAll();
}

void Sidebar::onItemDoubleClicked(const QModelIndex &index)
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

void Sidebar::onSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
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

void Sidebar::applyTheme()
{
    Theme &theme = Theme::instance();

    QColor bgColor = theme.getColor("editor.background");

    QColor fgColor = theme.getColor("editor.foreground");

    QColor borderColor = theme.getColor("menubar.border");

    QColor selectionBg = theme.getColor("explorer.selectionBackground");

    QColor selectionFg = theme.getColor("explorer.selectionForeground");

    QColor hoverBg = theme.getColor("explorer.hoverBackground");

    QColor secondaryColor = theme.getColor("secondary");

    QColor primaryColor = theme.getColor("primary");

    QFont explorerFont = theme.getFont("explorer");

    QFont treeFont("Segoe UI", 9);
    m_treeView->setFont(treeFont);

    QString tabStyle = QString(R"(
    QTabWidget::pane {
        border: none;
        background-color: %1;
    }
    
    QTabWidget::tab-bar {
        left: 0px;
        alignment: left;
    }
    
    QTabBar {
        background-color: %1;
        border: none;
        qproperty-drawBase: false;
        qproperty-expanding: false;
        outline: none;
    }
    
    QTabBar::tab {
        border: none;
        background-color: %1;
        color: %2;
        margin: 0;
        padding: 0;
        min-width: 32px;
        min-height: 32px;
        max-width: 32px;
        max-height: 32px;
    }
    QTabBar::tab:not(:last) {
        margin-right: 6px;
    }
    
    QTabBar::tab:selected {
        color: #FFFFFF;
        border: 1px solid #ffca2c;
    }
    
    QTabBar::tab:hover:!selected {
        background-color: %5;
    }
    
    QTabBar::tab:first {
        margin-left: 2px;
    }
    )").arg(bgColor.name())           // %1 - background
        .arg(fgColor.name())           // %2 - foreground
        .arg(selectionBg.name())       // %3 - selection background (inactive)
        .arg(selectionFg.name())       // %4 - selection foreground
        .arg(hoverBg.name())           // %5 - hover background
        .arg(explorerFont.family())    // %6 - font family
        .arg(explorerFont.pointSize()) // %7 - font size
        .arg(primaryColor.name());     // %8 - active selection background
    
        if (m_tabWidget->tabBar()){

            m_tabWidget->tabBar()->setStyleSheet(tabStyle);
            m_tabWidget->tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);
            m_tabWidget->tabBar()->setAttribute(Qt::WA_SetStyle, true);
            m_tabWidget->tabBar()->update();
        }

    this->setStyleSheet(QString(R"(
        QDockWidget {
            background-color: %1;
            border: none;
        }
    )")
                            .arg(bgColor.name()));

    QString treeStyle = QString(R"(
        QTreeView {
            background-color: %1;
            color: %2;
            border: none;
            outline: none;
            font-family: "%6";
            font-size: %7pt;
            show-decoration-selected: 1;
        }
        QTreeView::item {
            padding: 2px 4px;
            border: none;
            min-height: 22px;
        }
        QTreeView::item:hover {
            background-color: %5;
        }
        QTreeView::item:selected {
            background-color: %8;
            color: %4;
        }
        QTreeView::item:selected:active {
            background-color: %8;
        }
        QTreeView::item:selected:!active {
            background-color: %3;
        }
        QTreeView::branch {
            background-color: %1;
        }
        QTreeView::branch:has-siblings:!adjoins-item {
            border-image: none;
        }
        QTreeView::branch:has-siblings:adjoins-item {
            border-image: none;
        }
        QTreeView::branch:!has-children:!has-siblings:adjoins-item {
            border-image: none;
        }
        QTreeView::branch:has-children:!has-siblings:closed,
        QTreeView::branch:closed:has-children:has-siblings {
            border-image: none;
            image: none;
        }
        QTreeView::branch:open:has-children:!has-siblings,
        QTreeView::branch:open:has-children:has-siblings {
            border-image: none;
            image: none;
        }
    )")
                            .arg(bgColor.name())           // %1 - background
                            .arg(fgColor.name())           // %2 - foreground
                            .arg(selectionBg.name())       // %3 - selection background (inactive)
                            .arg(selectionFg.name())       // %4 - selection foreground
                            .arg(hoverBg.name())           // %5 - hover background
                            .arg(explorerFont.family())    // %6 - font family
                            .arg(explorerFont.pointSize()) // %7 - font size
                            .arg(primaryColor.name());     // %8 - active selection background

    m_treeView->setStyleSheet(treeStyle);
    m_treeView->setIndentation(20);         // VSCode-like indentation
    m_treeView->setAnimated(false);         // Disable animations for snappier feel
    m_treeView->setUniformRowHeights(true); // Better performance

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
}

void Sidebar::createNewFile()
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