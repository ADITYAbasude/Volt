#include "ui/MainWindow.h"
#include <QApplication>
#include <QLabel>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QIcon appIcon(":/icons/app.ico");
    app.setWindowIcon(appIcon);
    
    // Set application metadata
    QApplication::setApplicationName("Volt");
    QApplication::setApplicationDisplayName("Volt Editor");

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
