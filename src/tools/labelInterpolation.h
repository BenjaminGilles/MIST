#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <tools/baseTool.h>
#include <QToolBar>
#include <QComboBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMessageBox>

class labelInterpolationTool:public baseTool
{
    Q_OBJECT

public:
    labelInterpolationTool(MPRImageView* v,image<T>* i):baseTool(v,i),areaCombo(NULL) {}
    ~labelInterpolationTool()
    {
        if(areaCombo!=NULL) delete areaCombo;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QLabel* areaLabel=new QLabel(tr("Direction:"),parent);

        areaCombo=new QComboBox(parent);
        areaCombo->setToolTip(tr("Direction"));
        areaCombo->setStatusTip(tr("Direction along which the interpolation is performed"));
        areaCombo->addItem("x");
        areaCombo->addItem("y");
        areaCombo->addItem("z");
        areaCombo->setCurrentIndex(2);

        QToolBar* comboToolBar=new QToolBar(parent);
        comboToolBar->addWidget(areaLabel);
        comboToolBar->addSeparator();
        comboToolBar->addWidget(areaCombo);
        comboToolBar->addSeparator();

        QAction* computeAct = new QAction(tr("Interpolate selection"), parent);
        computeAct->setStatusTip(tr("Interpolate selection"));
        connect(computeAct, SIGNAL(triggered()),this, SLOT(InterpolateSelection()));

        QAction* computeAct2 = new QAction(tr("Interpolate labels"), parent);
        computeAct2->setStatusTip(tr("Interpolate all labels"));
        connect(computeAct2, SIGNAL(triggered()),this, SLOT(InterpolateLabels()));

        QToolBar* ToolBar=new QToolBar(parent);
        ToolBar->addAction(computeAct);
        ToolBar->addAction(computeAct2);
        ToolBar->addSeparator();

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(comboToolBar);
        layout->addWidget(ToolBar);
        layout->addStretch();

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);
        return w;
    }


public slots:

    void InterpolateSelection()
    {
        unsigned int nbslices= img->interpolateROI(areaCombo->currentIndex());
        emit statusChanged(tr("Interpolated %1 slices").arg(nbslices));
        mprview->Render(true);
    }

    void InterpolateLabels()
    {
        unsigned int nbslices= img->interpolateLabels(areaCombo->currentIndex());
        emit statusChanged(tr("Interpolated %1 slices").arg(nbslices));
        mprview->Render(true);
    }

private:
    QComboBox* areaCombo;
};

#endif
