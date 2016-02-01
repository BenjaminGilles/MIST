#ifndef LANDMARKS_H
#define LANDMARKS_H

#include <tools/baseTool.h>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QHeaderView>
#include <QColorDialog>
#include <QGroupBox>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QActionGroup>
#include <QToolBar>
#include <QMessageBox>

class landmarksTool:public baseTool
{
    Q_OBJECT

public:
    landmarksTool(MPRImageView* v,image<T>* i):baseTool(v,i)  {}
    ~landmarksTool()
    {
        delete table;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        table=new QTableWidget(0,1,parent);
        table->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
        table->horizontalHeader()->hide();
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        connect(table, SIGNAL( cellChanged (int,int) ), this, SLOT( cellChanged(int,int) ) );
        connect(table, SIGNAL( cellClicked (int,int) ), this, SLOT( cellClicked(int,int) ) );
//        connect(table, SIGNAL( cellDoubleClicked (int,int) ), this, SLOT( cellDoubleClicked(int,int) ) );
        updateTable();

        QAction* AddAct = new QAction(QIcon(":add"),tr("Add new landmark"), parent);
        AddAct->setStatusTip(tr("Add new landmark"));
        connect(AddAct, SIGNAL(triggered()),this, SLOT(Add()));
        QAction* DelAct = new QAction(QIcon(":remove"),tr("Remove selected landmark"), parent);
        DelAct->setStatusTip(tr("Remove selected landmark"));
        connect(DelAct, SIGNAL(triggered()),this, SLOT(Del()));

        QAction* LoadAct = new QAction(QIcon(":download"),tr("Load landmarks"), parent);
        LoadAct->setStatusTip(tr("Load landmarks"));
        connect(LoadAct, SIGNAL(triggered()),this, SLOT(Load()));
        QAction* SaveAct = new QAction(QIcon(":upload"),tr("Save landmarks"), parent);
        SaveAct->setStatusTip(tr("Save landmarks"));
        connect(SaveAct, SIGNAL(triggered()),this, SLOT(Save()));

        QActionGroup* GeometryGroup = new QActionGroup(parent);
        GeometryGroup->addAction(AddAct);
        GeometryGroup->addAction(DelAct);
        GeometryGroup->addAction(LoadAct);
        GeometryGroup->addAction(SaveAct);

        QToolBar* ToolBar=new QToolBar(parent);
        ToolBar->addActions(GeometryGroup->actions());

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(table);
        layout->addWidget(ToolBar);
        layout->addStretch();

//        QGroupBox *w = new QGroupBox(tr("Landmarks"));
        QWidget *w = new QWidget(parent);
        w->setLayout(layout);

        return w;
    }


public slots:


    void updateTable()
    {
        QStringList headerLabels;
        table->setRowCount(img->landmarks.size());
        for(unsigned int i=0;i<img->landmarks.size();i++)
        {
            QTableWidgetItem *text = new QTableWidgetItem(QString(img->landmarks[i].name.c_str()));
            table->setItem(i, 0, text);
            headerLabels << QString::number(i);
        }
        table->setVerticalHeaderLabels(headerLabels);
    }

    void cellChanged(int r,int c)
    {
        if(c==0)
        {
            img->landmarks[r].name=table->item(r,c)->text().toStdString();
        }
    }

    void cellClicked(int r,int c)
    {
        img->selectLandmark(r);
    }

//    void cellDoubleClicked(int r,int c)
//    {
//    }

    void Load()
    {

    }

    void Save()
    {

    }

    void Add()
    {
        img->addLandmark();
        updateTable();
        table->clearSelection();
        table->selectRow(table->rowCount()-1); // todo fix
        img->selectLandmark(table->rowCount()-1);
        mprview->Render();
    }

    void Del()
    {
        std::vector<unsigned int> s=getSelected();
        if(s.size()==0) return;
        for(unsigned int i=0;i<s.size();i++)        img->delLandmark(s[0]);
        table->clearSelection();
        img->selectLandmark();
        updateTable();
        mprview->Render();
    }




private:

    QTableWidget* table;
    std::vector<unsigned int> getSelected()
    {
        std::vector<unsigned int> s;
        foreach (QTableWidgetItem *item, table->selectedItems())        s.push_back(item->row());
        return s;
    }


};

#endif
