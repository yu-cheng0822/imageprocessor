#include "imageprocessor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    imageprocessor w;
    w.show();
    return a.exec();
}
