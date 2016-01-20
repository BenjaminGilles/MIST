#include "imageview.h"
#include <QtGui>




/*------------------------------------------------------------------------------------------------------------*/
GraphView::GraphView(QWidget * parent ,image<T>* v,const unsigned int a)
    :QGraphicsView(parent),autofitinview(true),aspectRatioMode(Qt::IgnoreAspectRatio),border(0),labelOpacity(0.5),roiOpacity(0.8),labelBorderOnly(true),mode(Navigation),pressed(Qt::NoButton), img(v),area(a)
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
    this->setFocusPolicy(Qt::NoFocus);


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
    event->modifiers();

    QGraphicsView::keyPressEvent(event);

    if(!event->modifiers())
        switch (event->key())
        {
        case Qt::Key_Up:    { int dp[2]={0,-1}; img->moveViewBB(dp,area); emit selectionDone(); } break;
        case Qt::Key_Down:  { int dp[2]={0,1};  img->moveViewBB(dp,area); emit selectionDone(); } break;
        case Qt::Key_Left:  { int dp[2]={-1,0}; img->moveViewBB(dp,area); emit selectionDone(); } break;
        case Qt::Key_Right: { int dp[2]={1,0}; img->moveViewBB(dp,area);  emit selectionDone(); } break;
        }

    if(event->key()==Qt::Key_PageUp || (event->key()==Qt::Key_Up && event->modifiers().testFlag(Qt::ShiftModifier)))
        if(setSlice(img->slice[area]+1))
        {
            emit sliceChanged(img->slice[area]);
        }
    if(event->key()==Qt::Key_PageDown || (event->key()==Qt::Key_Down && event->modifiers().testFlag(Qt::ShiftModifier)))
        if(setSlice(img->slice[area]+1))
        {
            emit sliceChanged(img->slice[area]);
        }

    //    case Qt::Key_Minus:    this->scale(1./1.2,1./1.2);       break;
    //    case Qt::Key_Equal:    fitinview ();         break;
}



void GraphView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();

    if(!img) return;
    if(img->img.is_empty()) return;


    CImg<unsigned char> im = img->getCutplane(area);
    img->overlayLabels(im,area,labelOpacity,labelBorderOnly,roiOpacity);

    if(showSlice[0]) img->overlaySliceTrace(im,area,0);
    if(showSlice[1]) img->overlaySliceTrace(im,area,1);
    if(mode==RegionGrowing) img->overlaySeed(im,area);

    im.permute_axes("cxyz");
    QImage qimage = QImage(im.data(),im.height(),im.depth(),3*im.height(),QImage::Format_RGB888);
    painter->drawImage(sceneRect,qimage);


    //    else
    //    {
    //        QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    //        gradient.setColorAt(0, Qt::white);
    //        gradient.setColorAt(1, Qt::lightGray);
    //        painter->fillRect(rect.intersects(sceneRect), gradient);
    //        painter->setBrush(Qt::NoBrush);
    //    }
    //    painter->drawRect(sceneRect);

    //qDebug()<<"drawBackground "<<area;
}


// paint RGBA image (foreground)
void overlayRGBACImage(QPainter *painter, CImg<unsigned char>& im, const QRectF &rect, const qreal opacity=1.)
{
    im.permute_axes("cxyz");
    QImage qimage = QImage(im.data(),im.height(),im.depth(),4*im.height(),QImage::Format_ARGB32);
    painter->setOpacity(opacity);
    painter->drawImage(rect,qimage);
}



void GraphView::drawForeground(QPainter *painter, const QRectF &/*rect*/)
{
    if(mode==Brush && this->hasFocus())
    {
        CImg<unsigned char> im = img->getEmptyCutplane(area);
        img->overlayBrush(im,area);
        overlayRGBACImage(painter,im,this->sceneRect(),1);
    }
    else if(mode==Zoom && pressed==Qt::LeftButton && !pressedModifiers)
    {
        // QRectF r(pressedPos,currentPos);
        QRectF r(pressedPos,currentPos);
        painter->setPen ( Qt::NoPen);
        painter->setBrush(Qt::SolidPattern);
        painter->setBrush(Qt::cyan);
        painter->setOpacity (0.3);
        painter->drawRect(r);

        painter->setPen ( QPen ( Qt::darkCyan, 1, Qt::DotLine ) );
        painter->setBrush(Qt::NoBrush);
        painter->setOpacity (1);
        painter->drawRect(r);
    }

    //    qDebug()<<"drawForeground "<<area;
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    QGraphicsView::wheelEvent(event);
    if(event->delta()==0) return;
    int incr = (int)event->delta() / (int)std::abs(event->delta());
    if(setSlice(img->slice[area]+incr))
    {
        if(mode==Brush && pressed==Qt::LeftButton) {img->selectBrush(area,!pressedModifiers.testFlag(Qt::ShiftModifier)); Render(true);}
        emit sliceChanged(img->slice[area]);
    }
    // todo change brush size with Qt::ControlModifier
}

void GraphView::mousePressEvent(QMouseEvent *mouseEvent)
{
    QGraphicsView::mousePressEvent(mouseEvent);
    if(!img) return;

    pressed=mouseEvent->button();
    pressedModifiers=mouseEvent->modifiers();
    if (pressed != Qt::NoButton)
    {
        pressedPos=currentPos=mapToScene(mouseEvent->pos());
    }
    if(mode==Brush && pressed==Qt::LeftButton)
    {
        img->selectBrush(area,!pressedModifiers.testFlag(Qt::ShiftModifier));
        Render(true);
    }
    else if(mode==RegionGrowing && pressed==Qt::LeftButton)
    {
        img->selectSeed(area);
        emit seedSelected();
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

    if(mode==Zoom && pressed==Qt::LeftButton)
    {
        //         QRectF r(pressedPos,currentPos);
        QRectF rect = this->sceneRect();
        QRectF r(pressedPos,currentPos);
        img->selectViewBB((double)r.left()/(double)rect.width(),(double)r.bottom()/(double)rect.height(),(double)r.right()/(double)rect.width(),(double)r.top()/(double)rect.height(),area);
        emit selectionDone();
    }

    pressed=Qt::NoButton;
    pressedModifiers=Qt::KeyboardModifiers();

    //    Render(true);
}

void GraphView::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    QGraphicsView::mouseMoveEvent(mouseEvent);
    this->setFocus(Qt::MouseFocusReason);

    if(!img) return;
    bool updateBackground=false;
    currentPos=mapToScene(mouseEvent->pos());

    QRectF rect = this->sceneRect();

    img->updateCoord((double)currentPos.x()/(double)rect.width(),(double)currentPos.y()/(double)rect.height(),area);
    std::string l = img->getLabelAtCoord();
    if(l.size())    emit statusChanged(tr("(%1,%2,%3) : %4").arg(img->coord[0]).arg(img->coord[1]).arg(img->coord[2]).arg(l.c_str()));
    else            emit statusChanged(tr("(%1,%2,%3)").arg(img->coord[0]).arg(img->coord[1]).arg(img->coord[2]));

    if(mode==Brush && pressed==Qt::LeftButton) {img->selectBrush(area,!pressedModifiers.testFlag(Qt::ShiftModifier)); updateBackground=true;}

    Render(updateBackground);
}

void GraphView::focusOutEvent ( QFocusEvent * event )
{
    QGraphicsView::focusOutEvent(event);
    Render(false); // remove brush visualization
}
