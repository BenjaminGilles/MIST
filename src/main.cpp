#include "mainwindow.h"
#include <QApplication>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFileInfo fileInfo;
    if(argc>=2)  fileInfo.setFile(QString(argv[1]));

    MainWindow w(fileInfo.absoluteFilePath());
    w.show();

    return a.exec(); // useless comment
}
