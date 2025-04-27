#include <QApplication>
#include <QLabel>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    /*
        set application icon for both window and taskbar
    */
    QIcon appIcon(":/icons/app.ico");
    app.setWindowIcon(appIcon);
    
    // Set the taskbar icon explicitly
    QApplication::setWindowIcon(appIcon);
    
    // Set application metadata
    QApplication::setApplicationName("Volt");
    QApplication::setApplicationDisplayName("Volt Editor");

    QLabel label("Hello World");
    label.setAlignment(Qt::AlignCenter);
    label.setWindowTitle("");
    label.setStyleSheet("font-size: 24px; color: blue; background-color: cyan;");
    label.resize(200, 100);
    label.show();

    return app.exec();
}
