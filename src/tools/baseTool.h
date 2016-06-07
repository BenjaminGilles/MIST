#ifndef BASETOOL_H
#define BASETOOL_H

#include <QWidget>
#include <imageview.h>
#include <image.h>

class baseTool:public QObject
{
    Q_OBJECT

public:
    baseTool(MPRImageView* v,image<T>* i):mprview(v),img(i)
    {
        connect(this, SIGNAL(statusChanged(const QString)), mprview, SLOT(changeStatus(const QString)));
    }
    ~baseTool() {}

    virtual QWidget* getMenu(QWidget *parent=NULL)=0;

signals:
    void statusChanged(const QString);

public slots:

protected:
    MPRImageView* mprview;
    image<T>* img;
};

#endif
