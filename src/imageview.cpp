#include "imageview.h"
#include <QtGui>




/*------------------------------------------------------------------------------------------------------------*/
GraphView::GraphView(QWidget * parent ,image<T>* v,const unsigned int a)
    :QGraphicsView(parent),autofitinview(true),aspectRatioMode(Qt::IgnoreAspectRatio),border(0),labelOpacity(0.5),roiOpacity(0.8),labelBorderOnly(true),mode(Navigation),pressed(false), img(v),area(a),onArea(false)
{
    //this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    this->setCacheMode(QGraphicsView::CacheBackground);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff) ;
    this->setFrameStyle(QFrame::NoFrame);
    this->setRenderHint(QPainter::Antialiasing);
    //    this->setTransformationAnchor(AnchorUnderMouse);
    //    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    //    this->scale(0.5,0.5);
    this->setMinimumSize(30, 30);
    this->setMouseTracking(true);


    QGraphicsScene* scene = new QGraphicsScene(this);
    this->setScene(scene);

    //        setItemIndexMethod(QGraphicsScene::NoIndex);
    //  graphscene->setBackgroundBrush(Qt::black);
    //  addRect ( 0,0, 420,420,QPen(QColor(255,0,0)) );

    showSlice[0]=true;
    showSlice[1]=true;

    reinit();
    Render(false);

    fitinview ();
}

void GraphView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:    { int dp[2]={0,-1}; img->moveViewBB(dp,area); emit selectionDone(); } break;
    case Qt::Key_Down:  { int dp[2]={0,1};  img->moveViewBB(dp,area); emit selectionDone(); } break;
    case Qt::Key_Left:  { int dp[2]={-1,0}; img->moveViewBB(dp,area); emit selectionDone(); } break;
    case Qt::Key_Right: { int dp[2]={1,0}; img->moveViewBB(dp,area);  emit selectionDone(); } break;

        //    case Qt::Key_Minus:    this->scale(1./1.2,1./1.2);       break;
        //    case Qt::Key_Equal:    fitinview ();         break;
    }
    QGraphicsView::keyPressEvent(event);
}




void GraphView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();

    // Fill
    if(img && !img->img.is_empty())
    {
        CImg<unsigned char> im = img->getCutplane(area);
        img->overlayLabels(im,area,labelOpacity,labelBorderOnly,roiOpacity);

        if(showSlice[0]) img->overlaySliceTrace(im,area,0);
        if(showSlice[1]) img->overlaySliceTrace(im,area,1);

        im.permute_axes("cxyz");
        QImage qimage = QImage(im.data(),im.height(),im.depth(),3*im.height(),QImage::Format_RGB888);
        painter->drawImage(sceneRect,qimage);
    }
    //    else
    //    {
    //        QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    //        gradient.setColorAt(0, Qt::white);
    //        gradient.setColorAt(1, Qt::lightGray);
    //        painter->fillRect(rect.intersects(sceneRect), gradient);
    //        painter->setBrush(Qt::NoBrush);
    //    }
    //    painter->drawRect(sceneRect);

    //    {
    //        double xy[2]={0,0};
    //        img-> getPlaneLine(xy,area);
    //        QRectF rect = this->sceneRect();
    //        xy[0]*=rect.width();         xy[1]*=rect.height();
    //        painter->setOpacity (0.5);
    //        QColor col[2]; if(area==2) {col[0]=Qt::red; col[1]=Qt::green;} else if(area==1) {col[0]=Qt::red; col[1]=Qt::blue;} else {col[0]=Qt::blue; col[1]=Qt::green;}
    //        if(showSlice[0])
    //        {
    //            painter->setPen ( QPen ( col[0], 1, Qt::SolidLine ) );
    //            painter->drawLine(rect.x()+xy[0],rect.y(),rect.x()+xy[0],rect.y()+rect.height());
    //        }
    //        if(showSlice[1])
    //        {
    //            painter->setPen ( QPen ( col[1], 1, Qt::SolidLine ) );
    //            painter->drawLine(rect.x(),rect.y()+xy[1],rect.x()+rect.width(),rect.y()+xy[1]);
    //        }
    //    }


    qDebug()<<"drawBackground "<<area;
}



void GraphView::drawForeground(QPainter *painter, const QRectF &/*rect*/)
{
    QRectF sceneRect = this->sceneRect();

    if(mode==Brush && onArea==true)
    {
        unsigned int dimz, dimxy[2];
        img-> getPlaneDim(dimz,dimxy,area);
        CImg<unsigned char> im(dimxy[0],dimxy[1],1,4); im.fill(0);
        img->overlayBrush(im,area);
        cimg_forXYC(im,x,y,c) if(c!=3) if(im(x,y,0,c)!=0) im(x,y,0,3)=255;
        im.permute_axes("cxyz");
        QImage qimage = QImage(im.data(),im.height(),im.depth(),4*im.height(),QImage::Format_ARGB32);
        painter->drawImage(sceneRect,qimage);
    }


    if(mode==Zoom && pressed)
    {
        // QRectF r(pressedPos,currentPos);
        QRectF r(pressedPos,currentPos);
        painter->setPen ( Qt::NoPen);
        painter->setBrush(Qt::SolidPattern);
        painter->setBrush(Qt::cyan);
        painter->setOpacity (0.3);
        painter->drawRect(r);

        painter->setPen ( QPen ( Qt::darkCyan, 2, Qt::DotLine ) );
        painter->setBrush(Qt::NoBrush);
        painter->setOpacity (1);
        painter->drawRect(r);
    }

    //    qDebug()<<"drawForeground "<<area;
}


void GraphView::mousePressEvent(QMouseEvent *mouseEvent)
{
    QGraphicsView::mousePressEvent(mouseEvent);
    if(!img) return;

    if (mouseEvent->button() == Qt::LeftButton)
    {
        pressedPos=currentPos=mapToScene(mouseEvent->pos());
        pressed=true;
    }
}

//void GraphView::mouseDoubleClickEvent(QMouseEvent *mouseEvent)
//{
//    QGraphicsView::mouseDoubleClickEvent(mouseEvent);
//    emit mouseDoubleClicked(mouseEvent);
//}

void GraphView::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    QGraphicsView::mouseReleaseEvent(mouseEvent);

    if(!img) return;

    if(mode==Zoom && pressed)
    {
        //         QRectF r(pressedPos,currentPos);
        QRectF rect = this->sceneRect();
        QRectF r(pressedPos,currentPos);
        img->selectViewBB((double)r.left()/(double)rect.width(),(double)r.bottom()/(double)rect.height(),(double)r.right()/(double)rect.width(),(double)r.top()/(double)rect.height(),area);
        emit selectionDone();
    }

    pressed=false;
    Render(true);
}

void GraphView::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    QGraphicsView::mouseMoveEvent(mouseEvent);
    if(!img) return;

    currentPos=mapToScene(mouseEvent->pos());

    QRectF rect = this->sceneRect();

    img->updateCoord((double)currentPos.x()/(double)rect.width(),(double)currentPos.y()/(double)rect.height(),area);
    std::string l = img->getLabelAtCoord();
    if(l.size())    emit statusChanged(tr("(%1,%2,%3) : %4").arg(img->coord[0]).arg(img->coord[1]).arg(img->coord[2]).arg(l.c_str()));
    else            emit statusChanged(tr("(%1,%2,%3)").arg(img->coord[0]).arg(img->coord[1]).arg(img->coord[2]));

    if(mode==Brush) emit areaChanged(area);

    Render(false);
}
