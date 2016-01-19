#ifndef REGIONGROWING_H
#define REGIONGROWING_H

#include <tools/baseTool.h>
#include <QPushButton>
#include <QVBoxLayout>

class regionGrowingTool:public baseTool
{
    Q_OBJECT

public:
    regionGrowingTool(MPRImageView* v,image<T>* i):baseTool(v,i)  {}
    ~regionGrowingTool()
    {
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QPushButton* switchDimAct = new QPushButton(tr("2D"), parent);
        switchDimAct->setStatusTip(tr("Restrict growing to 2D"));
        switchDimAct->setCheckable(true);
        switchDimAct->setChecked(true);
        connect(switchDimAct, SIGNAL(toggled(bool)),this, SLOT(switchDim(bool)));
        QPushButton* InLabelAct = new QPushButton(tr("Inside labels"), parent);
        InLabelAct->setStatusTip(tr("Restrict growing to labels"));
        InLabelAct->setCheckable(true);
        InLabelAct->setChecked(true);
        connect(InLabelAct, SIGNAL(toggled(bool)),this, SLOT(InLabel(bool)));
        QPushButton* ConnectedAct = new QPushButton(tr("Connected"), parent);
        ConnectedAct->setStatusTip(tr("Enforce connectivity"));
        ConnectedAct->setCheckable(true);
        ConnectedAct->setChecked(true);
        connect(ConnectedAct, SIGNAL(toggled(bool)),this, SLOT(Connected(bool)));

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(switchDimAct);
        layout->addWidget(InLabelAct);
        layout->addWidget(ConnectedAct);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);
        return w;

        //action.push_back(ACTION(18,"Control+left/right button -- Add/Remove separation",0,CTRL,LEFT,false));
        //action.push_back(ACTION(19,""                                                  ,0,CTRL,RIGHT,false));
        //action.push_back(ACTION(40,"Control+backspace -- Remove all separations",cimg::keyBACKSPACE,CTRL));
    }


public slots:

    void switchDim(bool val)
    {
    }

    void InLabel(bool val)
    {
    }

    void Connected(bool val)
    {
    }



private:


};

#endif
