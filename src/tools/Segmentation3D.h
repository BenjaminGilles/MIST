// Patotskaya
// Interpolation tool

//#ifndef BRUSH_H
//#define BRUSH_H

#include <tools/baseTool.h>
#include <widgets/qSlice.h>
#include <QLabel>
#include <QSpinBox>
#include <QToolBar>
#include <QRadioButton>
#include <QPushButton>
#include <QLineEdit>

class Segment3DTool:public baseTool
{
    Q_OBJECT

public:
    int ar_fixed = 2; //First window by default
    bool interp_type = false; //Linear interp by def

    Segment3DTool(MPRImageView* v,image<T>* i):baseTool(v,i),rangew(NULL)
    {}
    ~Segment3DTool()
    {}

    virtual QWidget* getMenu(QWidget *parent=NULL)
    {
        QRadioButton *button1 = new QRadioButton("Direction 1 ");
        QRadioButton *button2 = new QRadioButton("Direction 2 ");
        QRadioButton *button3 = new QRadioButton("Direction 3 ");

        connect(button1, SIGNAL (released()), this, SLOT (handleCheckButton1() ) );
        connect(button2, SIGNAL (released()), this, SLOT (handleCheckButton2() ) );
        connect(button3, SIGNAL (released()), this, SLOT (handleCheckButton3() ) );

        QToolBar *toolbar = new QToolBar(parent);
        toolbar->addWidget(button1);
        toolbar->addWidget(button2);
        toolbar->addWidget(button3);

        toolbar ->addSeparator();

        QToolBar *slice_bar = new QToolBar();

        //QLabel *PosLabel1 = new QLabel(tr("Start Layer"));

        //slice_bar ->addWidget(PosLabel1);

        /*QSpinBox* start_sl=new QSpinBox(parent);
        start_sl->setStatusTip(tr("Start layer"));
        start_sl->setToolTip(tr("Start layer"));
        start_sl->setRange(0, 1000);*/
        //start_sl->setValue(img->brushSize);
        //connect(start_sl, SIGNAL( valueChanged(int) ), this , SLOT( setBrushSize(int) ) );

        QToolBar *slice_bar1 = new QToolBar();

        QRadioButton *button4 = new QRadioButton("Linear");
        QRadioButton *button5 = new QRadioButton("Spline");

        connect(button4, SIGNAL (released()), this, SLOT (handleCheckButton4() ) );
        connect(button5, SIGNAL (released()), this, SLOT (handleCheckButton5() ) );

        slice_bar1->addWidget(button4);
        slice_bar1->addWidget(button5);


        /*QLineEdit *echoLineEdit = new QLineEdit();
        QLineEdit *validatorLineEdit= new QLineEdit();;

        connect(echoLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(customSlotF(const QString &)));
        connect(validatorLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(customSlotL(const QString &)));

        slice_bar1 ->addWidget(echoLineEdit);
        slice_bar1 ->addWidget(validatorLineEdit);*/




        /*slice_bar1 ->addSeparator();
        QLabel *PosLabel2 = new QLabel(tr("End Layer"));

        slice_bar1 ->addWidget(PosLabel2);
*/
        /*QSpinBox* end_sl=new QSpinBox(parent);
        end_sl->setStatusTip(tr("End layer"));
        end_sl->setToolTip(tr("End layer"));
        end_sl->setRange(0, 1000);
        //start_sl->setValue(img->brushSize);
        //connect(start_sl, SIGNAL( valueChanged(int) ), this , SLOT( setBrushSize(int) ) );

        slice_bar1 ->addWidget(end_sl);
*/
        /*for(unsigned int i=0;i<2;i++)
        {
            slice_bar->addWidget(PosLabel);
            vsize_edit[i] = new QLineEdit(parent);
//            vsize_edit[i]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
            vsize_edit[i]->setAlignment(Qt::AlignCenter);
            vsize_edit[i]->setFixedWidth(50);
            slice_bar->addWidget(vsize_edit[i]);
            slice_bar->addSeparator();
        }*/

        QPushButton *m_button;
        m_button= new QPushButton("Interpolate");
           // set size and location of the button
        m_button->setGeometry(QRect(QPoint(100, 100),
        QSize(200, 50)));

        // Connect button signal to appropriate slot
        connect(m_button, SIGNAL (released()), this, SLOT (handleButton() ) );

        slice_bar1 ->addWidget(m_button);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(toolbar);
        layout->addStretch();
        layout->addWidget(slice_bar);
        layout->addWidget(slice_bar1);

        QWidget *w = new QWidget(parent);
        w->setLayout(layout);
        return w;
    }

signals:
    void textChanged(const QString &);

public slots:

    void customSlotF(const QString & s){
        bool tru = true;
        img->first_slice = s.toInt(&tru,10);
    }

    void customSlotL(const QString & s){
        bool tru = true;
        img->last_slice = s.toInt(&tru,10);
    }

    void handleCheckButton1(){
        ar_fixed = 2;
        qDebug()<<">>>Radio  Button Press"<<ar_fixed;
    }

    void handleCheckButton2(){
        ar_fixed = 0;
        qDebug()<<">>>Radio  Button Press"<<ar_fixed;
    }

    void handleCheckButton3(){
        ar_fixed = 1;
        qDebug()<<">>>Radio  Button Press"<<ar_fixed;
    }

    void handleCheckButton4(){
        interp_type = false;
        qDebug()<<">>>linear Interpolation"<<ar_fixed;
    }

    void handleCheckButton5(){
        interp_type = true;
        qDebug()<<">>>Spline Interpolation"<<ar_fixed;
    }



    void handleButton(){
        bool tru = true;

        int pos,area;
        int dir[2],dimz;
        area = ar_fixed;

        img->getPlaneDirections(dir,pos);
        /*for (int d = 0;d<3;d++){
            if ((d != dir[0]) and (d != dir[1]) )
                area = d;
        }*/

        img->interpolation_type = interp_type;

        qDebug()<<">>> Button Press"<<area << dir[0]<<dir[1];
        img->interpolateLayers(area);
        qDebug()<<">>> Done"<<area << dir[0]<<dir[1]<<img->first_slice<<img->last_slice;

        //img->selectBrushMulLayers(area,true);
        //mprview->Render(false);

    }

    void setBrushSize(int size)
    {
        img->brushSize=size;
        mprview->Render(false);
    }

    void changeRangeSlices(const QString & f_slice, const QString & l_slice)
    {
        bool tru = true;
        img->first_slice = f_slice.toInt(&tru,10);
        img->last_slice = l_slice.toInt(&tru,10);
        qDebug()<<"Key Press last"<<img->last_slice;
    }


private:

    QRangeWidget *rangew ;
    T range[2];
    QLineEdit* vsize_edit[3];


};

//#endif
