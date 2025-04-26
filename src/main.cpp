#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel label("Hello World");
    label.setAlignment(Qt::AlignCenter);
    label.setStyleSheet("font-size: 24px; color: blue; background-color: cyan;");
    label.setWindowTitle("My First Qt App");
    label.resize(200, 100);
    label.show();

    return app.exec();
}
