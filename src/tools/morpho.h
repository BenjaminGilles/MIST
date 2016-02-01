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
    morphoTool(MPRImageView* v,image<T>* i):baseTool(v,i),morphoSizespin(NULL),is2D(true)  {}
    ~morphoTool()
    {
        if(morphoSizespin!=NULL) delete morphoSizespin;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QAction* is2DAct = new QAction(tr("2D"), parent);
        is2DAct->setStatusTip(tr("Restrict operations to 2D planes ?"));
        is2DAct->setCheckable(true);
        is2DAct->setChecked(is2D);
        connect(is2DAct, SIGNAL(toggled(bool)),this, SLOT(Is2D(bool)));

        QToolBar* ToolBar1=new QToolBar(parent);
        ToolBar1->addAction(is2DAct);
        ToolBar1->addSeparator();

        QAction* DilateBut = new QAction(tr("Dilate"), parent);
        DilateBut->setStatusTip(tr("Dilate selection"));
        connect(DilateBut, SIGNAL(triggered()),this, SLOT(Dilate()));

        QAction* ErodeBut = new QAction(tr("Erode"), parent);
        ErodeBut->setStatusTip(tr("Erode selection"));
        connect(ErodeBut, SIGNAL(triggered()),this, SLOT(Erode()));

        QAction* OpenBut = new QAction(tr("Open"), parent);
        OpenBut->setStatusTip(tr("Open selection"));
        connect(OpenBut, SIGNAL(triggered()),this, SLOT(Open()));

        QAction* CloseBut = new QAction(tr("Close"), parent);
        CloseBut->setStatusTip(tr("Close selection"));
        connect(CloseBut, SIGNAL(triggered()),this, SLOT(Close()));

        morphoSizespin=new QSpinBox(parent);
        morphoSizespin->setToolTip(tr("Size of the structuring element"));
        morphoSizespin->setStatusTip(tr("Size of the structuring element"));
        morphoSizespin->setRange(0, 100);
        morphoSizespin->setValue(1);

        QToolBar* ToolBar2=new QToolBar(parent);
        ToolBar2->addAction(DilateBut);
        ToolBar2->addSeparator();
        ToolBar2->addAction(ErodeBut);
        ToolBar2->addSeparator();
        ToolBar2->addAction(OpenBut);
        ToolBar2->addSeparator();
        ToolBar2->addAction(CloseBut);
        ToolBar2->addSeparator();
        ToolBar2->addWidget(morphoSizespin);
        ToolBar2->addSeparator();

        QAction* FillHolesBut = new QAction(tr("FillHoles"), parent);
        FillHolesBut->setStatusTip(tr("Fill holes in selection"));
        connect(FillHolesBut, SIGNAL(triggered()),this, SLOT(FillHoles()));

        QToolBar* ToolBar3=new QToolBar(parent);
        ToolBar3->addAction(FillHolesBut);
        ToolBar3->addSeparator();

        QAction* InvertBut = new QAction(tr("Invert"), parent);
        InvertBut->setStatusTip(tr("Invert selection"));
        connect(InvertBut, SIGNAL(triggered()),this, SLOT(Invert()));

        QToolBar* ToolBar4=new QToolBar(parent);
        ToolBar4->addAction(InvertBut);
        ToolBar4->addSeparator();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(ToolBar1);
        layout->addWidget(ToolBar2);
        layout->addWidget(ToolBar3);
        layout->addWidget(ToolBar4);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);

        return w;
    }


public slots:

    void Is2D(bool val)
    {
        is2D=val;
    }

    void Open()
    {
        Dilate();
        Erode();
    }

    void Close()
    {
        Erode();
        Dilate();
    }

    void Dilate()
    {
        img->dilate(2*morphoSizespin->value()+1);
        mprview->Render(true);
    }

    void Erode()
    {
        img->erode(2*morphoSizespin->value()+1);
        mprview->Render(true);
    }

    void FillHoles()
    {
        if(is2D) {  for(unsigned int i=0;i<3;i++) if(mprview->view[i]->isVisible()) img->FillHoles2D(i); }
        else img->FillHoles();
        mprview->Render(true);
    }

    void Invert()
    {
        if(is2D) {  for(unsigned int i=0;i<3;i++) if(mprview->view[i]->isVisible()) img->InvertROI2D(i); }
        else img->InvertROI();
        mprview->Render(true);
    }

private:

    QSpinBox* morphoSizespin;
    bool is2D;

};

#endif
