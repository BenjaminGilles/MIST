#ifndef LABELTABLE_H
#define LABELTABLE_H

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

class labelTableTool:public baseTool
{
    Q_OBJECT

public:
    labelTableTool(MPRImageView* v,image<T>* i):baseTool(v,i)  {}
    ~labelTableTool()
    {
        delete table;
    }

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        table=new QTableWidget(0,3,parent);
        table->setColumnWidth ( 0, 30);
        table->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
        table->setColumnWidth ( 2, 30);
        table->horizontalHeader()->hide();
        connect(table, SIGNAL( cellChanged (int,int) ), this, SLOT( cellChanged(int,int) ) );
        connect(table, SIGNAL( cellClicked (int,int) ), this, SLOT( cellClicked(int,int) ) );
        connect(table, SIGNAL( cellDoubleClicked (int,int) ), this, SLOT( cellDoubleClicked(int,int) ) );
        updateTable();

        QAction* SelectLabelAct = new QAction(QIcon(":select"),tr("Selec Labels"), parent);
        SelectLabelAct->setStatusTip(tr("Select Labels"));
        connect(SelectLabelAct, SIGNAL(triggered()),this, SLOT(SelectLabel()));
        QAction* AddLabelAct = new QAction(QIcon(":add"),tr("Add ROI to Label"), parent);
        AddLabelAct->setShortcut(QKeySequence(Qt::Key_Return));
        AddLabelAct->setStatusTip(tr("Add ROI to Label (Return)"));
        connect(AddLabelAct, SIGNAL(triggered()),this, SLOT(AddLabel()));
        QAction* DelLabelAct = new QAction(QIcon(":remove"),tr("Remove ROI from Label"), parent);
        DelLabelAct->setShortcut(QKeySequence(Qt::Key_Backspace));
        DelLabelAct->setStatusTip(tr("Remove ROI from Label (Backspace)"));
        connect(DelLabelAct, SIGNAL(triggered()),this, SLOT(DelLabel()));
        QAction* ClearLabelAct = new QAction(QIcon(":clear"),tr("Clear ROI"), parent);
        ClearLabelAct->setShortcut(QKeySequence(Qt::Key_Space));
        ClearLabelAct->setStatusTip(tr("Clear ROI (Space)"));
        connect(ClearLabelAct, SIGNAL(triggered()),this, SLOT(ClearLabel()));

        QSlider* opacitySlider=new QSlider(Qt::Horizontal,parent);
        opacitySlider->setStatusTip(tr("Label Opacity"));
        opacitySlider->setPageStep ( 1 );
        opacitySlider->setRange(0,100);
        opacitySlider->setValue(50);
        connect(opacitySlider, SIGNAL( valueChanged(int) ), this, SLOT( changeOpacity(int) ) );

        QAction *borderAct = new QAction(QIcon(":plain"),tr("Plain/Boundary"), parent);
        borderAct->setCheckable(true);
        borderAct->setStatusTip(tr("Switch between plain/boundary visualization"));
        connect(borderAct, SIGNAL( toggled (bool) ), this, SLOT( changePlainVisu(bool) ) );

        QActionGroup* GeometryGroup = new QActionGroup(parent);
        GeometryGroup->addAction(SelectLabelAct);
        GeometryGroup->addAction(AddLabelAct);
        GeometryGroup->addAction(DelLabelAct);
        GeometryGroup->addAction(ClearLabelAct);

        QToolBar* ToolBar=new QToolBar(parent);
        ToolBar->addActions(GeometryGroup->actions());
        ToolBar->addWidget(opacitySlider);
        ToolBar->addAction(borderAct);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(table);
        layout->addWidget(ToolBar);

//        QGroupBox *w = new QGroupBox(tr("Labels"));
        QWidget *w = new QWidget(parent);
        w->setLayout(layout);

        return w;
    }


public slots:

    void changeOpacity (int val )
    {
        for(unsigned int i=0;i<3;++i) mprview->view[i]->graphView->labelOpacity = (double)val/100.;
        mprview->Render(true);
    }

    void changePlainVisu (bool val )
    {
        for(unsigned int i=0;i<3;++i) mprview->view[i]->graphView->labelBorderOnly = !val;
        mprview->Render(true);
    }

    void updateTable()
    {
        QStringList headerLabels;
        table->setRowCount(img->labelName.size());
        for(unsigned int i=0;i<img->labelName.size();i++)
        {
            QTableWidgetItem *text = new QTableWidgetItem(QString(img->labelName[i].c_str()));
            table->setItem(i, 1, text);
            QTableWidgetItem *color = new QTableWidgetItem();
            color->setBackground(QBrush(QColor(img->palette(i,0,0,0),img->palette(i,0,0,1),img->palette(i,0,0,2))));
            color->setFlags(Qt::ItemIsEnabled);
            table->setItem(i, 0, color);
            QTableWidgetItem *lockitem = new QTableWidgetItem();
            lockitem->setIcon(QIcon(img->labelLock[i]?":lock":""));
            lockitem->setFlags(Qt::ItemIsUserCheckable);
            lockitem->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, 2, lockitem);
            headerLabels << QString::number(i);
        }
        table->setVerticalHeaderLabels(headerLabels);
    }

    void cellChanged(int r,int c)
    {
        if(c==1)
        {
            img->labelName[r]=table->item(r,c)->text().toStdString();
        }
    }

    void cellClicked(int r,int c)
    {
        if(c==2)
        {
            img->labelLock[r]=!img->labelLock[r];
            table->item(r,c)->setIcon(QIcon(img->labelLock[r]?":lock":""));
        }
    }

    void cellDoubleClicked(int r,int c)
    {
        if(c==0)
        {
            QColor color= QColorDialog::getColor(QColor(img->palette(r,0,0,0),img->palette(r,0,0,1),img->palette(r,0,0,2)),NULL);
            table->item(r,c)->setBackground(QBrush(color));
            img->palette(r,0,0,0)=(unsigned char)color.red();
            img->palette(r,0,0,1)=(unsigned char)color.green();
            img->palette(r,0,0,2)=(unsigned char)color.blue();
            mprview->Render();
            //        QDebug()<<color;
        }
    }


    void SelectLabel()
    {
        std::vector<unsigned int> s=getSelectedLabels();
        img->clearRoi();
        for(unsigned int i=0;i<s.size();i++)        img->addLabelToRoi(s[i]);
//        for(unsigned int i=0;i<s.size();i++)        qDebug()<<s[i];
        mprview->Render();
    }

    void AddLabel()
    {
        std::vector<unsigned int> s=getSelectedLabels();
        if(s.size()!=1) { QMessageBox::warning(NULL, tr("qtSegmentation"),tr("Please select only one label")); return; }
        for(unsigned int i=0;i<s.size();i++)        img->addRoiToLabel(s[0]);
        img->clearRoi();
        mprview->Render();
    }

    void DelLabel()
    {
        std::vector<unsigned int> s=getSelectedLabels();
        if(s.size()!=1) { QMessageBox::warning(NULL, tr("qtSegmentation"),tr("Please select only one label")); return; }
        for(unsigned int i=0;i<s.size();i++)        img->delRoiFromLabel(s[0]);
        img->clearRoi();
        mprview->Render();
    }

    void ClearLabel()
    {
        img->clearRoi();
        mprview->Render();
    }




private:

    QTableWidget* table;
    std::vector<unsigned int> getSelectedLabels()
    {
        std::vector<unsigned int> s;
        foreach (QTableWidgetItem *item, table->selectedItems())        s.push_back(item->row());
        return s;
    }


};

#endif
