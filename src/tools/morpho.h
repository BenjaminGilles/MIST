#ifndef MORPHO_H
#define MORPHO_H

#include <tools/baseTool.h>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
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
        QLabel* sizelab=new QLabel(tr("Size: "),parent);
        morphoSizespin=new QSpinBox(parent);
        morphoSizespin->setRange(0, 100);
        morphoSizespin->setValue(1);
        QHBoxLayout *sizelayout = new QHBoxLayout();
        sizelayout ->setMargin(0);
        sizelayout ->addWidget(sizelab);
        sizelayout ->addWidget(morphoSizespin);
        sizelayout ->addStretch();
        QWidget *sizew = new QWidget(parent);
        sizew->setLayout(sizelayout);

        QPushButton* DilateBut = new QPushButton(tr("Dilate"), parent);
        DilateBut->setStatusTip(tr("Dilate"));
        connect(DilateBut, SIGNAL(pressed()),this, SLOT(Dilate()));
        QPushButton* ErodeBut = new QPushButton(tr("Erode"), parent);
        ErodeBut->setStatusTip(tr("Erode"));
        connect(ErodeBut, SIGNAL(pressed()),this, SLOT(Erode()));
        QPushButton* FillHolesBut = new QPushButton(tr("FillHoles"), parent);
        FillHolesBut->setStatusTip(tr("FillHoles"));
        connect(FillHolesBut, SIGNAL(pressed()),this, SLOT(FillHoles()));
        QPushButton* InvertBut = new QPushButton(tr("Invert"), parent);
        InvertBut->setStatusTip(tr("Invert"));
        connect(InvertBut, SIGNAL(pressed()),this, SLOT(Invert()));

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(sizew);
        layout->addWidget(DilateBut);
        layout->addWidget(ErodeBut);
        layout->addWidget(FillHolesBut);
        layout->addWidget(InvertBut);
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
