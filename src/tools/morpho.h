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
        DilateBut->setStatusTip(tr("Dilate"));
        connect(DilateBut, SIGNAL(triggered()),this, SLOT(Dilate()));
        QAction* ErodeBut = new QAction(tr("Erode"), parent);
        ErodeBut->setStatusTip(tr("Erode"));
        connect(ErodeBut, SIGNAL(triggered()),this, SLOT(Erode()));
        QLabel* sizelab=new QLabel(tr("Size: "),parent);
        morphoSizespin=new QSpinBox(parent);
        morphoSizespin->setRange(0, 100);
        morphoSizespin->setValue(1);
        QToolBar* ToolBar1=new QToolBar(parent);
        ToolBar1->addAction(DilateBut);
        ToolBar1->addSeparator();
        ToolBar1->addAction(ErodeBut);
        ToolBar1->addSeparator();
        ToolBar1->addWidget(sizelab);
        ToolBar1->addWidget(morphoSizespin);
        ToolBar1->addSeparator();

        QAction* FillHolesBut = new QAction(tr("FillHoles"), parent);
        FillHolesBut->setStatusTip(tr("FillHoles"));
        connect(FillHolesBut, SIGNAL(triggered()),this, SLOT(FillHoles()));
        QToolBar* ToolBar2=new QToolBar(parent);
        ToolBar2->addAction(FillHolesBut);
        ToolBar2->addSeparator();

        QAction* InvertBut = new QAction(tr("Invert"), parent);
        InvertBut->setStatusTip(tr("Invert"));
        connect(InvertBut, SIGNAL(triggered()),this, SLOT(Invert()));
        QToolBar* ToolBar3=new QToolBar(parent);
        ToolBar3->addAction(InvertBut);
        ToolBar3->addSeparator();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(ToolBar1);
        layout->addWidget(ToolBar2);
        layout->addWidget(ToolBar3);
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
