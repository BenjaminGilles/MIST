#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <tools/baseTool.h>
#include <QToolBar>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMessageBox>

class labelInterpolationTool:public baseTool
{
    Q_OBJECT

public:
    labelInterpolationTool(MPRImageView* v,image<T>* i):baseTool(v,i) {}
    ~labelInterpolationTool()
    {
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QAction* computeAct = new QAction(tr("Interpolate selection"), parent);
        computeAct->setStatusTip(tr("Interpolate selection"));
        connect(computeAct, SIGNAL(triggered()),this, SLOT(InterpolateSelection()));

        QToolBar* ToolBar=new QToolBar(parent);
        ToolBar->addAction(computeAct);
        ToolBar->addSeparator();

        QAction* computeAct2 = new QAction(tr("Interpolate labels"), parent);
        computeAct2->setStatusTip(tr("Interpolate all labels"));
        connect(computeAct2, SIGNAL(triggered()),this, SLOT(InterpolateLabels()));

        QToolBar* ToolBar2=new QToolBar(parent);
        ToolBar2->addAction(computeAct2);
        ToolBar2->addSeparator();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(ToolBar);
        layout->addWidget(ToolBar2);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);
        return w;
    }


public slots:

    void InterpolateSelection()
    {
        unsigned int nbslices= img->interpolateROI(0);
        nbslices+= img->interpolateROI(1);
        nbslices+= img->interpolateROI(2);
        emit statusChanged(tr("Interpolated %1 slices").arg(nbslices));
        mprview->Render(true);
    }

    void InterpolateLabels()
    {
        unsigned int nbslices= img->interpolateLabels(0);
        nbslices+= img->interpolateLabels(1);
        nbslices+= img->interpolateLabels(2);
        emit statusChanged(tr("Interpolated %1 slices").arg(nbslices));
        mprview->Render(true);
    }

private:

};

#endif
