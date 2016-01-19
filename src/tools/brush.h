#ifndef BRUSH_H
#define BRUSH_H

#include <tools/baseTool.h>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>

class brushTool:public baseTool
{
    Q_OBJECT

public:
    brushTool(MPRImageView* v,image<T>* i):baseTool(v,i)  {}
    ~brushTool()
    {
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QLabel* sizelab=new QLabel(tr("Size: "),parent);
        QSpinBox* sizespin=new QSpinBox(parent);
        sizespin->setRange(0, 100);
        sizespin->setValue(img->brushSize);
        connect(sizespin, SIGNAL( valueChanged(int) ), this , SLOT( setBrushSize(int) ) );
        QHBoxLayout *sizelayout = new QHBoxLayout();
        sizelayout ->setMargin(0);
        sizelayout ->addWidget(sizelab);
        sizelayout ->addWidget(sizespin);
        sizelayout ->addStretch();
        QWidget *sizew = new QWidget(parent);
        sizew->setLayout(sizelayout);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(sizew);
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


};

#endif
