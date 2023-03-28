#include "cvtools.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
 

    CVToolsWindow w;

    w.show();
    return a.exec();
}
