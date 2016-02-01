#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QDebug>
#include <QGraphicsView>
#include <QSplitter>
#include <QHBoxLayout>
#include <QObject>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QResizeEvent>

#include <widgets/mprlayout.h>
#include <image.h>

typedef int T;

class MPRImageView;
class ImageView;
class GraphView;





//-----------------------------------------------------------------------------------------------//
//	image widget
//-----------------------------------------------------------------------------------------------//

class GraphView : public QGraphicsView
{
    Q_OBJECT

public:
    GraphView(QWidget * parent = 0,image<T>* v=NULL,const unsigned int a=0);
    ~GraphView() { }

    bool autofitinview;
    Qt::AspectRatioMode aspectRatioMode;
    unsigned int border;

    QSize	minimumSizeHint () const
    {
        return QSize(this->sceneRect().width(),this->sceneRect().height());
    }

    QSize	sizeHint () const
    {
        return minimumSizeHint();
    }

    void updateImage();

    bool showSlice[2];
    double labelOpacity;
    double roiOpacity;
    bool labelBorderOnly;
    enum Mode {Navigation,Zoom,Brush,RegionGrowing,Morpho,Landmarks,MarchingCubes};
    Mode mode;
    enum Qt::MouseButton pressed;
    Qt::KeyboardModifiers pressedModifiers;

    image<T>* img;
    const unsigned int area;

public slots:

    void fitinview ()
    {
        //                QRectF rect = this->sceneRect();
        //                QRectF unity = this->matrix().mapRect(QRectF(0, 0, 1, 1));
        //                scale(1 / unity.width(), 1 / unity.height());
        //                QRectF scenerect = this->matrix().mapRect(rect);
        //                scale(viewport()->rect().width() / scenerect.width(), viewport()->rect().height() / scenerect.height());
        //                centerOn(rect.center());
        this->fitInView(this->sceneRect().x()-border,this->sceneRect().y()-border,this->sceneRect().width()+border,this->sceneRect().height()+border,aspectRatioMode);
    }

    void reinit()
    {
        double z=0,xy[2]={0,0};
        img->getPlaneSize(z,xy,area);
        setSceneRect(0,0,xy[0],xy[1]);
    }

    void Render(bool updateBackground=true)
    {
        if(updateBackground) scene()->invalidate(this->sceneRect(), QGraphicsScene::BackgroundLayer);
        scene()->invalidate(this->sceneRect(), QGraphicsScene::ForegroundLayer);
    }

    bool setSlice(int val) // when slice is changed outside
    {
        return img->setSlice( val , area);
        //        Render(true);
    }

signals:
    void statusChanged(const QString);
    void sliceChanged(int val);
    void allSlicesChanged();
    void selectionDone();
    void seedSelected();
    void LandmarkSet();

    //void mousePressed(QMouseEvent *mouseEvent);
    //void mouseReleased(QMouseEvent *mouseEvent);
    //    void mouseDoubleClicked(QMouseEvent *mouseEvent);


protected:
    void resizeEvent ( QResizeEvent * /*event*/ )
    {
        if(autofitinview) fitinview ();
    }

    void keyPressEvent(QKeyEvent *event);


private:
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mousePressEvent(QMouseEvent *mouseEvent);
    void wheelEvent(QWheelEvent *event);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    //    void mouseDoubleClickEvent(QMouseEvent *mouseEvent);
    void focusOutEvent ( QFocusEvent * event );

    QPointF pressedPos;
    QPointF currentPos;
};

//-----------------------------------------------------------------------------------------------//
//	slider + image widget
//-----------------------------------------------------------------------------------------------//

class ImageView: public QWidget
{
    Q_OBJECT

public:

    ImageView(QWidget *parent, image<T>* v, const unsigned int a, const unsigned int sliderHeight=15)
        : QWidget(parent) , img(v), area(a)
    {
        graphView=new GraphView(this,img,area);
        connect(graphView, SIGNAL(statusChanged(const QString)), parent, SLOT(changeStatus(const QString)));

        slider=new QSlider(Qt::Horizontal,this);
        slider->setPageStep ( 1 );
        slider->setFixedHeight ( sliderHeight );
        connect(slider, SIGNAL( valueChanged(int) ), this, SLOT( changeSlider(int) ) );

        connect(this, SIGNAL( sliceChanged(int) ), graphView, SLOT( setSlice(int) ) );
        connect(this, SIGNAL( sliceChanged(int) ), parent, SLOT( Render() ) );

        connect(graphView, SIGNAL( allSlicesChanged() ), parent, SLOT( updateSliders() ) );
        connect(graphView, SIGNAL( allSlicesChanged() ), parent, SLOT( Render() ) );

        connect(graphView, SIGNAL( sliceChanged(int) ), this , SLOT( setSlider(int) ) );
        connect(graphView, SIGNAL( sliceChanged(int) ), parent, SLOT( Render() ) );

        connect(graphView, SIGNAL( seedSelected() ), parent, SLOT( selectSeed() ) );
        connect(graphView, SIGNAL( LandmarkSet() ), parent, SLOT( Render() ) );

        connect(graphView, SIGNAL( selectionDone() ), parent, SLOT( reinit() ) );

        label=new QLabel(this);
        label->setFixedHeight ( sliderHeight );
        label->setFixedWidth ( 30 );

        hlayout = new QHBoxLayout(this);
        hlayout->setMargin(0);
        hlayout->setSpacing(10);
        hlayout->addWidget(slider);
        hlayout->addWidget(label);
        QWidget *w = new QWidget(this);
        w->setLayout(hlayout);

        vlayout = new QVBoxLayout(this);
        vlayout->setMargin(0);
        vlayout->setSpacing(0);
        vlayout->addWidget(graphView);
        vlayout->addWidget(w);
        this->setLayout(vlayout);

        reinit();
    }

    ~ImageView()
    {
        delete hlayout;
        delete vlayout;
        delete graphView;
        delete slider;
        delete label;
    }

    GraphView* graphView;

    QSize	sizeHint () const
    {
        return QSize(graphView->sizeHint().width()+2*vlayout->margin(),graphView->sizeHint().height()+2*vlayout->margin()+hlayout->sizeHint().height());
    }

signals:
    void sliceChanged(int val);

public slots:

    void reinit() // when image has changed
    {
        graphView->reinit();
        graphView->Render(true);
        unsigned int dimz, dimxy[2];
        img-> getPlaneDim(dimz,dimxy,area);
        slider->blockSignals(true);
        slider->setRange(0,dimz-1);
        slider->blockSignals(false);
        setSlider(img->slice[area]);
    }

    void changeSlider(int val) // when slider moves
    {
        label->setNum(val+(int)img->viewBB[0][area]);
        emit sliceChanged(val+img->viewBB[0][area]);
    }

    void setSlider ( int slice ) // when slice is changed outside
    {
        slider->blockSignals(true);
        slider->setValue(slice-img->viewBB[0][area]);
        slider->blockSignals(false);
        label->setNum(slice);
    }

protected:
    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout;
    QSlider *slider;
    QLabel *label;
    image<T>* img;
    const unsigned int area;
};



//-----------------------------------------------------------------------------------------------//
//	3 imageViews
//-----------------------------------------------------------------------------------------------//

class MPRImageView : public QWidget
{
    Q_OBJECT

public:
    MPRImageView(QWidget * parent = 0,image<T>* v=NULL)
        :QWidget(parent), img(v), viewMode(GraphView::Navigation)
    {
        mprlayout = new MPRLayout(this);
        for(unsigned int i=0;i<3;++i)    view[i] = setupImageView(i);
        mprlayout->addWidget(view[2], MPRLayout::XY);
        mprlayout->addWidget(view[0], MPRLayout::ZY);
        mprlayout->addWidget(view[1], MPRLayout::XZ);
        setLayout(mprlayout);

        showSlices = new QAction(tr("&Show slice trace"), this);
        showSlices->setStatusTip(tr("Show slice trace"));
        showSlices->setCheckable (true);
        showSlices->setChecked (true);
        connect(showSlices, SIGNAL(triggered(bool)), this, SLOT(updateSliceVisibility()) );
    }

    ~MPRImageView()
    {
        for(unsigned int i=0;i<3;++i) delete view[i];
        delete mprlayout;
    }

    QAction *showViewAct[3];
    QAction *showSlices;
    ImageView* view[3];


signals:
    void statusChanged(const QString);
    void seedSelected();

public slots:
    void changeStatus(const QString s) {emit statusChanged(s);}
    void selectSeed() {emit seedSelected();}

    void updateSliceVisibility()
    {
        if(!showSlices->isChecked())
        {
            for(unsigned int i=0;i<3;++i) for(unsigned int j=0;j<2;++j) view[i]->graphView->showSlice[j]=false;
        }
        else
        {
            view[0]->graphView->showSlice[0]=view[2]->isVisible();
            view[1]->graphView->showSlice[1]=view[2]->isVisible();
            view[0]->graphView->showSlice[1]=view[1]->isVisible();
            view[2]->graphView->showSlice[1]=view[1]->isVisible();
            view[2]->graphView->showSlice[0]=view[0]->isVisible();
            view[1]->graphView->showSlice[0]=view[0]->isVisible();
        }
        Render(true);
    }


    void reinit()
    {
        updateGeometry();
        for(unsigned int i=0;i<3;++i) view[i]->reinit();
        mprlayout->invalidate();
        updateGeometry();
    }

    void updateSliders() // render all sliders
    {
        for(unsigned int i=0;i<3;++i) view[i]->setSlider(img->slice[i]);
    }

    void Render(bool updateBackground=true)
    {
        for(unsigned int i=0;i<3;++i) view[i]->graphView->Render(updateBackground);
    }

    void updateLayout() { mprlayout->invalidate(); }

    void setViewMode(GraphView::Mode mode)
    {
        viewMode=mode;
        for(unsigned int i=0;i<3;++i) view[i]->graphView->mode=mode;
        Render(true);
    }

    GraphView::Mode getViewMode()    { return viewMode; }

private:
    ImageView* setupImageView(const unsigned int area)
    {
        ImageView* iv=new ImageView(this,img,area);

        QString areatext[3]={QString("ZY"),QString("XZ"),QString("XY")};
        showViewAct[area] = new QAction(tr("&Show ")+areatext[area], this);
        showViewAct[area]->setStatusTip(tr("Show ")+areatext[area]);
        showViewAct[area]->setCheckable (true);
        showViewAct[area]->setChecked (true);
        connect(showViewAct[area], SIGNAL(triggered(bool)), iv, SLOT(setVisible (bool)) );
        connect(showViewAct[area], SIGNAL(triggered(bool)), this, SLOT(updateLayout()) );
        connect(showViewAct[area], SIGNAL(triggered(bool)), this, SLOT(updateSliceVisibility()) );
        return iv;
    }

    image<T>* img;
    MPRLayout *mprlayout;
    GraphView::Mode viewMode;
};








#endif // IMAGEVIEW_H
