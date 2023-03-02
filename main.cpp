#include "mafs.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mafs w;
    w.show();
    return a.exec();
}
