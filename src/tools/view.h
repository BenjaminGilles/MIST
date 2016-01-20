#ifndef VIEW_H
#define VIEW_H

#include <tools/baseTool.h>
#include <QGroupBox>
#include <QToolBar>
#include <QHBoxLayout>
#include <widgets/qSlice.h>

class viewTool:public baseTool
{
    Q_OBJECT

public:
    viewTool(MPRImageView* v,image<T>* i):baseTool(v,i),zoomAct(NULL),oldViewMode(GraphView::Navigation),rangew(NULL)  {}
    ~viewTool()
    {
        if(zoomAct!=NULL) delete zoomAct;
        if(rangew!=NULL) delete rangew;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QGroupBox *groupBox = new QGroupBox(tr("View"));

        rangew = new QRangeWidget(parent);
        connect(rangew, SIGNAL( rangeChanged(int,int) ), this, SLOT( changeRange(int,int) ) );

        zoomAct = new QAction(QIcon(":resize"),tr("Zoom in"), parent);
        zoomAct->setCheckable(true);
        zoomAct->setStatusTip(tr("Select a region to zoom in"));
        connect(zoomAct, SIGNAL( toggled (bool) ), this, SLOT( setZoom(bool) ) );
        for(unsigned int i=0;i<3;++i) connect(mprview->view[i]->graphView, SIGNAL( selectionDone () ), this, SLOT( switchOffzoom() ) );

        QAction* resetAct = new QAction(QIcon(":resetZoom"),tr("Reset zoom"), parent);
        resetAct->setStatusTip(tr("Reset zoom"));
        connect(resetAct, SIGNAL(triggered()),this, SLOT(zoomReset()));

        QAction* cropAct = new QAction(QIcon(":crop"),tr("Crop image"), parent);
        cropAct->setStatusTip(tr("Crop image according to zoomed region"));
        connect(cropAct, SIGNAL(triggered()),this, SLOT(crop()));

        QActionGroup* GeometryGroup = new QActionGroup(parent);
        GeometryGroup->addAction(zoomAct);
        GeometryGroup->addAction(resetAct);
        GeometryGroup->addAction(cropAct);

        QToolBar* ToolBar=new QToolBar(parent);
        ToolBar->addActions(GeometryGroup->actions());

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(rangew);
        layout->addWidget(ToolBar);

        groupBox->setLayout(layout);
        return groupBox;
    }


public slots:

    void switchOffzoom()    { zoomAct->setChecked(false);    }

    void setZoom(bool s)
    {
        if(s) { oldViewMode=mprview->getViewMode(); mprview->setViewMode(GraphView::Zoom);}
        else mprview->setViewMode(oldViewMode);
    }

    void zoomReset()
    {
        img->resetViewBB();
        mprview->reinit();
    }

    void crop()
    {
        img->crop();
        mprview->reinit();
    }

    void reinit()
    {
        T _min=0,_max=0;
        img->getIntensityRangeLimits(_min,_max);
        rangew->setRangeLimits((int)floor((double)_min), (int)ceil((double)_max));
    }

    void changeRange(int _min, int _max)
    {
        img->intensityRange[0]=(T)_min;
        img->intensityRange[1]=(T)_max;
        mprview->Render(true);
    }

private:

    QAction *zoomAct;
    GraphView::Mode oldViewMode;
    QRangeWidget *rangew ;

};

#endif
