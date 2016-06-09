#ifndef SEGMENTATION3D_H
#define SEGMENTATION3D_H


#include <tools/baseTool.h>
#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <widgets/qSlice.h>
#include <QLabel>
#include <QSpinBox>

class segmentation3DTool:public baseTool
{
    Q_OBJECT

public:
    segmentation3DTool(MPRImageView* v,image<T>* i):baseTool(v,i)//,rangew(NULL)
    {}
    ~segmentation3DTool()
    {}

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QSpinBox* sizespin=new QSpinBox(parent);
        sizespin->setStatusTip(tr("Brush size"));
        sizespin->setToolTip(tr("Brush size"));
        sizespin->setRange(0, 100);
        sizespin->setValue(img->brushSize);
        connect(sizespin, SIGNAL( valueChanged(int) ), this , SLOT( setBrushSize(int) ) );
        QToolBar *toolbar = new QToolBar(parent);
        toolbar ->addWidget(sizespin);
        toolbar ->addSeparator();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(toolbar);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);
        return w;
    }


public slots:

    void setBrushSize(int size)
    {
        img->brushSize=size;
        mprview->Render(false);
    }


private:
//    QRangeWidget *rangew ;

//    T range[2];


};

#endif
