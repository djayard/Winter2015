#include "cfortedit.h"
#include <QApplication>
#include <QDir>
#include <iostream>


int main(int argc, char *argv[])
{
    QApplication A(argc, argv);
#ifdef Q_OS_MAC
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    QDir Dir = QDir::current();
    Dir.cdUp();
    Dir.cdUp();
    Dir.cdUp();
    QDir::setCurrent(Dir.absolutePath());
#endif


    CFortEdit W;
    W.show();

    return A.exec();
}
