#ifndef VIEW_H
#define VIEW_H

#include <tools/baseTool.h>
#include <QGroupBox>
#include <QToolBar>
#include <QHBoxLayout>
#include <widgets/qSlice.h>

class imageTool:public baseTool
{
    Q_OBJECT

public:
    imageTool(MPRImageView* v,image<T>* i):baseTool(v,i),zoomAct(NULL),oldViewMode(GraphView::Navigation),rangew(NULL)
    {
        for(unsigned int i=0;i<3;i++)         vsize_edit[i] =NULL;
        for(unsigned int i=0;i<3;i++)         dim_edit[i] =NULL;
    }
    ~imageTool()
    {
        for(unsigned int i=0;i<3;i++) if(vsize_edit[i]!=NULL) delete vsize_edit[i];
        for(unsigned int i=0;i<3;i++) if(dim_edit[i]!=NULL) delete dim_edit[i];
        if(zoomAct!=NULL) delete zoomAct;
        if(rangew!=NULL) delete rangew;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
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

        QToolBar *dim_bar = new QToolBar();
        QLabel* dim_txt=new QLabel(tr("Dimensions:"),parent);
        dim_bar->addWidget(dim_txt);
        dim_bar->addSeparator();
        for(unsigned int i=0;i<3;i++)
        {
            dim_edit[i] = new QLineEdit(parent);
//            dim_edit[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
            dim_edit[i]->setAlignment(Qt::AlignCenter);
            dim_edit[i]->setFixedWidth(50);
            dim_bar->addWidget(dim_edit[i]);
            dim_bar->addSeparator();
        }
        QAction* dim_Act = new QAction(QIcon(":resample"),tr("Resample image"), parent);
        dim_Act->setStatusTip(tr("Resample image"));
        connect(dim_Act, SIGNAL(triggered()),this, SLOT(resampleDim()));
        dim_bar->setIconSize(QSize(20,20));
        dim_bar->addAction(dim_Act);

        QToolBar *vsize_bar = new QToolBar();
        QLabel* vsize_txt=new QLabel(tr("Voxelsize:"),parent);
        vsize_bar->addWidget(vsize_txt);
        vsize_bar->addSeparator();
        for(unsigned int i=0;i<3;i++)
        {
            vsize_edit[i] = new QLineEdit(parent);
//            vsize_edit[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
            vsize_edit[i]->setAlignment(Qt::AlignCenter);
            vsize_edit[i]->setFixedWidth(50);
            vsize_bar->addWidget(vsize_edit[i]);
            vsize_bar->addSeparator();
        }
        QAction* vsize_Act = new QAction(QIcon(":resample"),tr("Resample image"), parent);
        vsize_Act->setStatusTip(tr("Resample image"));
        connect(vsize_Act, SIGNAL(triggered()),this, SLOT(resampleVoxelSize()));
        vsize_bar->setIconSize(QSize(20,20));
        vsize_bar->addAction(vsize_Act);


        QToolBar *mirror_bar = new QToolBar();
        QLabel* mirror_txt=new QLabel(tr("Mirror:"),parent);
        mirror_bar->addWidget(mirror_txt);
        mirror_bar->addSeparator();
        QAction* mirrorX_Act = new QAction(tr("X"), parent);
        mirrorX_Act->setStatusTip(tr("Mirror image along X"));
        connect(mirrorX_Act, SIGNAL(triggered()),this, SLOT(mirrorX()));
        mirror_bar->addAction(mirrorX_Act);
        mirror_bar->addSeparator();
        QAction* mirrorY_Act = new QAction(tr("Y"), parent);
        mirrorY_Act->setStatusTip(tr("Mirror image along Y"));
        connect(mirrorY_Act, SIGNAL(triggered()),this, SLOT(mirrorY()));
        mirror_bar->addAction(mirrorY_Act);
        mirror_bar->addSeparator();
        QAction* mirrorZ_Act = new QAction(tr("Z"), parent);
        mirrorZ_Act->setStatusTip(tr("Mirror image along Z"));
        connect(mirrorZ_Act, SIGNAL(triggered()),this, SLOT(mirrorZ()));
        mirror_bar->addAction(mirrorZ_Act);
        mirror_bar->addSeparator();


        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(rangew);
        layout->addWidget(ToolBar);
        layout->addWidget(dim_bar);
        layout->addWidget(vsize_bar);
        layout->addWidget(mirror_bar);
        layout->addStretch();

        //        QGroupBox *w = new QGroupBox(tr("View"));
        QWidget *w = new QWidget(parent);
        w->setLayout(layout);

        return w;
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
        if(img->crop())
        {
            reinit();
            mprview->reinit();
            // todo: reinit of regiongrowing min/max
        }
    }


    void mirrorX()
    {
        if(img->mirror('x')) mprview->reinit();
    }
    void mirrorY()
    {
        if(img->mirror('y')) mprview->reinit();
    }
    void mirrorZ()
    {
        if(img->mirror('z')) mprview->reinit();
    }


    void resampleDim()
    {
        bool ok;
        unsigned int newDim[3];
        for(unsigned int i=0;i<3;i++)
        {
            newDim[i]=dim_edit[i]->text().toUInt(&ok);
            if(!ok) {newDim[i]=img->dim[i]; dim_edit[i]->setText(QString::number(img->dim[i])); }
        }
        if(img->resampleDim(newDim))
        {
            reinit();
            mprview->reinit();
            // todo: reinit of regiongrowing min/max
        }
    }

    void resampleVoxelSize()
    {
        bool ok;
        real newVSize[3];
        for(unsigned int i=0;i<3;i++)
        {
            newVSize[i]=(real)vsize_edit[i]->text().toDouble(&ok);
            if(!ok) {newVSize[i]=img->voxelSize[i]; vsize_edit[i]->setText(QString::number(img->voxelSize[i])); }
        }
        if(img->resampleVoxelSize(newVSize))
        {
            reinit();
            mprview->reinit();
            // todo: reinit of regiongrowing min/max
        }
    }

    void reinit()
    {
        T _min=0,_max=0;
        img->getIntensityRangeLimits(_min,_max);
        rangew->setRangeLimits((int)floor((double)_min), (int)ceil((double)_max));
        for(unsigned int i=0;i<3;i++)         dim_edit[i]->setText(QString::number(img->dim[i]));
        for(unsigned int i=0;i<3;i++)         vsize_edit[i]->setText(QString::number(img->voxelSize[i]));
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
    QLineEdit* vsize_edit[3];
    QLineEdit* dim_edit[3];

};

#endif
