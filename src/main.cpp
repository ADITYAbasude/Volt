#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QCommandLineParser>
#include "ui/MainWindow.h"
#include "themes/Theme.h"
#include "logging/VoltLogger.h"
#include <QIcon>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QIcon appIcon(":/icons/app.ico");
    app.setWindowIcon(appIcon);
    // Set application metadata
    QApplication::setApplicationName("Volt");
    QApplication::setApplicationDisplayName("Volt Editor");
    QApplication::setApplicationVersion("1.0.0");

    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("Volt Editor - A modern text editor");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "File or folder to open", "[file/folder]");
    parser.process(app);

    // Initialize the centralized logging system
    VoltLogger::instance().initialize("logs/volt.log", VoltLogger::DEBUG, true, true);

    /*
     * Main entry point for the Volt Editor application.
     * Initializes the application, sets up logging, loads themes, and creates the main window.
     *
     * @param argc Argument count from command line
     * @param argv Argument vector from command line
     * @return Exit code of the application
     */

    VOLT_SYSTEM("=== VOLT EDITOR APPLICATION STARTING ===");
    VOLT_INFO_F("Application started with %1 arguments", argc);
    VOLT_DEBUG_F("Working directory: %1", QDir::currentPath());
    VOLT_DEBUG_F("Application directory: %1", app.applicationDirPath());

    // Initialize the resource system
    VOLT_SYSTEM("Initializing resource system...");
    int fontId = QFontDatabase::addApplicationFont(":/fonts/icons-carbon.ttf");
    if (fontId == -1) {
        VOLT_ERROR("Failed to load icons-carbon.ttf font!");
        return EXIT_FAILURE;
    } else {
        VOLT_DEBUG_F("icons-carbon.ttf loaded, fontId: %1", fontId);
    }

    QStringList loadedFamilies = QFontDatabase::applicationFontFamilies(fontId);
    VOLT_DEBUG_F("Loaded font families: %1", loadedFamilies.join(", "));

    QFont iconFont("icons-carbon");
    iconFont.setPointSize(16);

    // Check for file/folder argument from context menu
    const QStringList positionalArgs = parser.positionalArguments();
    QString fileToOpen;
    if (!positionalArgs.isEmpty()) {
        fileToOpen = positionalArgs.first();
        VOLT_INFO_F("File/folder to open from context menu: %1", fileToOpen);
    }

    // Load the dark theme
    VOLT_THEME("Loading dark theme...");
    Theme::instance().loadTheme("dark");
    VOLT_THEME("Dark theme loaded successfully");

    // Create main window
    VOLT_UI("Creating main window...");
    MainWindow window;
    
    // Open the file or folder if specified from context menu
    if (!fileToOpen.isEmpty()) {
        QFileInfo fileInfo(fileToOpen);
        if (fileInfo.isFile()) {
            window.openFile(fileToOpen);
            VOLT_INFO_F("Opening file from context menu: %1", fileToOpen);
        } else if (fileInfo.isDir()) {
            window.openFolder(fileToOpen);
            VOLT_INFO_F("Opening folder from context menu: %1", fileToOpen);
        } else {
            VOLT_WARN_F("Invalid file/folder path from context menu: %1", fileToOpen);
        }
    }
    
    window.show();
    VOLT_UI("Main window created and shown successfully");

    VOLT_SYSTEM("Entering application event loop...");
    int result = app.exec();

    VOLT_SYSTEM_F("Application event loop finished with code: %1", result);

    // Cleanup logging before exit
    VoltLogger::instance().shutdown();

    return result;
}
