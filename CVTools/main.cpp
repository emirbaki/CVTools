#include "cvtools.h"
#include <QtWidgets/QApplication>
#include "GraphWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
 

    CVToolsWindow w;

    w.show();
    return a.exec();
}
