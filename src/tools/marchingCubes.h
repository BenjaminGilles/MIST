#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include <tools/baseTool.h>
#include <QToolBar>
#include <QSpinBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMessageBox>

class marchingCubesTool:public baseTool
{
    Q_OBJECT

public:
    marchingCubesTool(MPRImageView* v,image<T>* i):baseTool(v,i),computeSpin(NULL)  {}
    ~marchingCubesTool()
    {
        if(computeSpin!=NULL) delete computeSpin;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QAction* computeAct = new QAction(tr("Compute isosurfaces"), parent);
        computeAct->setStatusTip(tr("Compute isosurfaces"));
        connect(computeAct, SIGNAL(triggered()),this, SLOT(ComputeMesh()));

        computeSpin=new QSpinBox(parent);
        computeSpin->setToolTip(tr("Resolution"));
        computeSpin->setStatusTip(tr("Marching cube resolution (0 for image resolution)"));
        computeSpin->setRange(0, 10000);
        computeSpin->setValue(0);

        QToolBar* ToolBar=new QToolBar(parent);
        ToolBar->addAction(computeAct);
        ToolBar->addWidget(computeSpin);
        ToolBar->addSeparator();

        QAction* saveAct = new QAction(/*QIcon(":save"),*/tr("Save isosurfaces"), parent);
        saveAct->setStatusTip(tr("Save isosurfaces"));
        connect(saveAct, SIGNAL(triggered()),this, SLOT(SaveMesh()));

        QToolBar* ToolBar2=new QToolBar(parent);
        ToolBar2->addAction(saveAct);
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

    void ComputeMesh()
    {
        unsigned int nbfaces=img->marchingCubes(computeSpin->value());
        emit statusChanged(tr("Generated %1 faces").arg(nbfaces));
    }

    void SaveMesh()
    {
        QString path = QFileDialog::getExistingDirectory(NULL,
                                                         tr("Select folder"), QString(img->currentPath.c_str()));
        if (path.isEmpty()) return;

        if(img->saveObjs(path.toStdString()))
        {
            emit statusChanged(tr("Mesh saved in '%1'").arg(path));
        }
        else  QMessageBox::warning(NULL, tr("QtSegmentation"), tr("Cannot save meshes in '%1'").arg(path));
    }



private:
    QSpinBox* computeSpin;

};

#endif
