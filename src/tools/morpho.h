#ifndef MORPHO_H
#define MORPHO_H

#include <tools/baseTool.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>
#include <QSpinBox>

class morphoTool:public baseTool
{
    Q_OBJECT

public:
    morphoTool(MPRImageView* v,image<T>* i):baseTool(v,i)  {}
    ~morphoTool()
    {
        delete morphoSizespin;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QAction* DilateBut = new QAction(tr("Dilate"), parent);
        DilateBut->setStatusTip(tr("Dilate selection"));
        connect(DilateBut, SIGNAL(triggered()),this, SLOT(Dilate()));

        QAction* ErodeBut = new QAction(tr("Erode"), parent);
        ErodeBut->setStatusTip(tr("Erode selection"));
        connect(ErodeBut, SIGNAL(triggered()),this, SLOT(Erode()));

        morphoSizespin=new QSpinBox(parent);
        morphoSizespin->setToolTip(tr("Size of the structuring element"));
        morphoSizespin->setStatusTip(tr("Size of the structuring element"));
        morphoSizespin->setRange(0, 100);
        morphoSizespin->setValue(1);

        QToolBar* ToolBar1=new QToolBar(parent);
        ToolBar1->addAction(DilateBut);
        ToolBar1->addSeparator();
        ToolBar1->addAction(ErodeBut);
        ToolBar1->addSeparator();
        ToolBar1->addWidget(morphoSizespin);
        ToolBar1->addSeparator();

        QAction* FillHolesBut = new QAction(tr("FillHoles"), parent);
        FillHolesBut->setStatusTip(tr("Fill holes in selection"));
        connect(FillHolesBut, SIGNAL(triggered()),this, SLOT(FillHoles()));

        QAction* InvertBut = new QAction(tr("Invert"), parent);
        InvertBut->setStatusTip(tr("Invert selection"));
        connect(InvertBut, SIGNAL(triggered()),this, SLOT(Invert()));

        QToolBar* ToolBar2=new QToolBar(parent);
        ToolBar2->addAction(FillHolesBut);
        ToolBar2->addSeparator();
        ToolBar2->addAction(InvertBut);
        ToolBar2->addSeparator();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(ToolBar1);
        layout->addWidget(ToolBar2);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);

        return w;
    }


public slots:

    void Dilate()
    {
        img->roi.dilate(2*morphoSizespin->value()+1);
        mprview->Render(true);
    }

    void Erode()
    {
        img->roi.erode(2*morphoSizespin->value()+1);
        mprview->Render(true);
    }

    void FillHoles()
    {
        img->FillHoles3D();
        mprview->Render(true);
    }

    void Invert()
    {
        img->InvertROI();
        mprview->Render(true);
    }

private:

    QSpinBox* morphoSizespin;

};

#endif
