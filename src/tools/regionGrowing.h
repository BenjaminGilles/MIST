#ifndef REGIONGROWING_H
#define REGIONGROWING_H

#include <tools/baseTool.h>
#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <widgets/qSlice.h>

class regionGrowingTool:public baseTool
{
    Q_OBJECT

public:
    regionGrowingTool(MPRImageView* v,image<T>* i):baseTool(v,i),rangew(NULL),is2D(true),inLabel(true),connected(true)
    {
        range[0]=range[1]=0;
        connect(mprview, SIGNAL(seedSelected()), this, SLOT(update()));
    }
    ~regionGrowingTool()
    {
            if(rangew!=NULL) delete rangew;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        rangew = new QRangeWidget(parent);
        connect(rangew, SIGNAL( rangeChanged(int,int) ), this, SLOT( changeRange(int,int) ) );

        QAction* is2DAct = new QAction(tr("2D"), parent);
        is2DAct->setStatusTip(tr("Restrict growing to 2D ?"));
        is2DAct->setCheckable(true);
        is2DAct->setChecked(is2D);
        connect(is2DAct, SIGNAL(toggled(bool)),this, SLOT(Is2D(bool)));

        QAction* InLabelAct = new QAction(tr("Inside label"), parent);
        InLabelAct->setStatusTip(tr("Restrict growing to label ?"));
        InLabelAct->setCheckable(true);
        InLabelAct->setChecked(inLabel);
        connect(InLabelAct, SIGNAL(toggled(bool)),this, SLOT(InLabel(bool)));

        QAction* ConnectedAct = new QAction(tr("Connected"), parent);
        ConnectedAct->setStatusTip(tr("Enforce connectivity ?"));
        ConnectedAct->setCheckable(true);
        ConnectedAct->setChecked(connected);
        connect(ConnectedAct, SIGNAL(toggled(bool)),this, SLOT(Connected(bool)));
        QToolBar* ToolBar=new QToolBar(parent);
        ToolBar->addAction(is2DAct);
        ToolBar->addAction(InLabelAct);
        ToolBar->addAction(ConnectedAct);
        ToolBar->addSeparator();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(rangew);
        layout->addWidget(ToolBar);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);
        return w;

        //action.push_back(ACTION(18,"Control+left/right button -- Add/Remove separation",0,CTRL,LEFT,false));
        //action.push_back(ACTION(19,""                                                  ,0,CTRL,RIGHT,false));
        //action.push_back(ACTION(40,"Control+backspace -- Remove all separations",cimg::keyBACKSPACE,CTRL));
    }


public slots:

    void reinit()
    {
        img->getIntensityRangeLimits(range[0],range[1]);
        rangew->setRangeLimits((int)floor((double)range[0]), (int)ceil((double)range[1]));
    }

    void changeRange(int _min, int _max)
    {
        range[0]=(T)_min;
        range[1]=(T)_max;
        update();
    }

    void Is2D(bool val)
    {
        is2D=val;
        update();
    }

    void InLabel(bool val)
    {
        inLabel=val;
        update();
    }

    void Connected(bool val)
    {
        connected=val;
        update();
    }


    void update()
    {
        img->regionGrowing(range,inLabel,connected,is2D);
        mprview->Render(true);
    }

private:
    QRangeWidget *rangew ;

    T range[2];
    bool is2D;
    bool inLabel;
    bool connected;

};

#endif
