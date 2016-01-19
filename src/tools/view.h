#ifndef VIEW_H
#define VIEW_H

#include <tools/baseTool.h>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <widgets/qSlice.h>

class viewTool:public baseTool
{
    Q_OBJECT

public:
    viewTool(MPRImageView* v,image<T>* i):baseTool(v,i),zoom(NULL),oldViewMode(GraphView::Navigation),rangew(NULL)  {}
    ~viewTool()
    {
        if(zoom!=NULL) delete zoom;
        if(rangew!=NULL) delete rangew;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QGroupBox *groupBox = new QGroupBox(tr("View"));

        rangew = new QRangeWidget(parent);
        connect(rangew, SIGNAL( rangeChanged(int,int) ), this, SLOT( changeRange(int,int) ) );

        zoom = new QPushButton("Zoom");
        zoom->setCheckable(true);
        connect(zoom, SIGNAL( toggled (bool) ), this, SLOT( setZoom(bool) ) );
        for(unsigned int i=0;i<3;++i) connect(mprview->view[i]->graphView, SIGNAL( selectionDone () ), this, SLOT( switchOffzoom() ) );
        QPushButton* zoombut = new QPushButton("R");
        zoombut->setMaximumWidth(20);
        connect(zoombut, SIGNAL( pressed () ), this, SLOT( zoomReset() ) );
        QHBoxLayout *zooml = new QHBoxLayout();
        zooml->setMargin(0);
        zooml->setSpacing(2);
        zooml->addWidget(zoom);
        zooml->addWidget(zoombut);
        QWidget *zoomw = new QWidget(parent);
        zoomw->setLayout(zooml);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(rangew);
        layout->addWidget(zoomw);

        groupBox->setLayout(layout);
        return groupBox;
    }


public slots:

    void switchOffzoom()    { zoom->setChecked(false);    }


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

    QPushButton *zoom;
    GraphView::Mode oldViewMode;
    QRangeWidget *rangew ;

};

#endif
