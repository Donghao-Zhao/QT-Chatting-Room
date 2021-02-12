#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("Chat");
    w.setWindowIcon(QIcon("://image.png"));
    w.show();

    return a.exec();
}
