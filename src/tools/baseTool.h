#ifndef BASETOOL_H
#define BASETOOL_H

#include <QWidget>
#include <imageview.h>
#include <image.h>

class baseTool:public QObject
{
    Q_OBJECT

public:
    baseTool(MPRImageView* v,image<T>* i):mprview(v),img(i)  {}
    ~baseTool() {}

    virtual QWidget* getMenu(QWidget *parent=NULL)=0;

public slots:

protected:
    MPRImageView* mprview;
    image<T>* img;
};

#endif
