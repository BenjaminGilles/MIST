#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include <tools/baseTool.h>
#include <QPushButton>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

class marchingCubesTool:public baseTool
{
    Q_OBJECT

public:
    marchingCubesTool(MPRImageView* v,image<T>* i):baseTool(v,i)  {}
    ~marchingCubesTool()
    {
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QPushButton* ComputeBut = new QPushButton(tr("Compute"), parent);
        ComputeBut->setStatusTip(tr("Compute Mesh"));
        connect(ComputeBut, SIGNAL(pressed()),this, SLOT(ComputeMesh()));
        QSpinBox* Computespin=new QSpinBox(parent);
        Computespin->setRange(1, 1000);
        Computespin->setValue(100);
        QHBoxLayout *Computelayout = new QHBoxLayout();
        Computelayout ->setMargin(0);
        Computelayout ->addWidget(ComputeBut);
        Computelayout ->addWidget(Computespin);
        Computelayout ->addStretch();
        QWidget *Computew = new QWidget(parent);
        Computew->setLayout(Computelayout);

        QPushButton* SaveBut = new QPushButton(tr("Save"), parent);
        SaveBut->setStatusTip(tr("Save Mesh"));
        connect(SaveBut, SIGNAL(pressed()),this, SLOT(SaveMesh()));

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(Computew);
        layout->addWidget(SaveBut);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);
        return w;
    }


public slots:

    void ComputeMesh()
    {
    }

    void SaveMesh()
    {
    }



private:


};

#endif
