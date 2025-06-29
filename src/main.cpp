#include <QApplication>
#include <QDir>
#include "ui/MainWindow.h"
#include "themes/Theme.h"
#include "logging/VoltLogger.h"
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QIcon appIcon(":/icons/app.ico");
    app.setWindowIcon(appIcon);
    // Set application metadata
    QApplication::setApplicationName("Volt");
    QApplication::setApplicationDisplayName("Volt Editor");

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

    // Load the dark theme
    VOLT_THEME("Loading dark theme...");
    Theme::instance().loadTheme("dark");
    VOLT_THEME("Dark theme loaded successfully");

    // Create main window
    VOLT_UI("Creating main window...");
    MainWindow window;
    window.show();
    VOLT_UI("Main window created and shown successfully");

    VOLT_SYSTEM("Entering application event loop...");
    int result = app.exec();

    VOLT_SYSTEM_F("Application event loop finished with code: %1", result);

    // Cleanup logging before exit
    VoltLogger::instance().shutdown();

    return result;
}
