#ifndef BRUSH_H
#define BRUSH_H

#include <tools/baseTool.h>
#include <QLabel>
#include <QSpinBox>
#include <QToolBar>

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
        QSpinBox* sizespin=new QSpinBox(parent);
        sizespin->setToolTip(tr("Brush size"));
        sizespin->setStatusTip(tr("Brush size"));
        sizespin->setRange(0, 100);
        sizespin->setValue(img->brushSize);
        connect(sizespin, SIGNAL( valueChanged(int) ), this , SLOT( setBrushSize(int) ) );
        QToolBar *toolbar = new QToolBar(parent);
        toolbar ->addWidget(sizespin);
        toolbar ->addSeparator();

        //>>>
        QLabel *PosLabel = new QLabel(tr("Layer number "
                 "%1 :").arg(0));
        toolbar ->addWidget(PosLabel);
        toolbar ->addSeparator();
        //<<<

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


};

#endif
