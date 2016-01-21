#include "qSlice.h"

#include <QGridLayout>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

QSlice::QSlice(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DoubleBuffer), parent)
{
    // valeurs par defaut
    _valueMin = -1000.;
    _valueMax = 1000.;
    _currentMin = -1000;
    _currentMax = 1000;
    _cursorSize = 12.;
    _mouseX	= 0.;
    _moved = 0;
    _onMin = false;
    _onMax = false;

    // reglage du rafraichissement
    _refreshTimer.setSingleShot(false);
    QObject::connect(
                &_refreshTimer, SIGNAL(timeout()),
                this, SLOT(update())
                );
    _refreshTimer.start(25);

    // suivi des deplacement de la souris
    this->setMouseTracking(true);
}

QSlice::~QSlice()
{
}

// taille par defaut
QSize QSlice::sizeHint() const
{
    return QSize(150, 40);
}

//setters
void QSlice::setCurrentMin(int val)
{
    if (val > _valueMax) _currentMin=_valueMax;
    else if (val < _valueMin) _currentMin=_valueMin;
    else _currentMin = val;
    if (_currentMin > _currentMax) _currentMax = _currentMin;
    this->update();
}

void QSlice::setCurrentMax(int val)
{
    if (val > _valueMax) _currentMax=_valueMax;
    else if (val < _valueMin) _currentMax=_valueMin;
    else _currentMax = val;
    if (_currentMin > _currentMax) _currentMin = _currentMax;
    this->update();
}

void QSlice::setValueMin(int val)
{
    if (val > _valueMax) return;
    _valueMin = val;
    if (_currentMin < _valueMin) _currentMin = _valueMin;
    if (_currentMax < _valueMin) _currentMax = _valueMin;
    this->update();
}

void QSlice::setValueMax(int val)
{
    if (val < _valueMin) return;
    _valueMax = val;
    if (_currentMin > _valueMax) _currentMin = _valueMax;
    if (_currentMax > _valueMax) _currentMax = _valueMax;
    this->update();
}

void QSlice::initializeGL()
{
}

void QSlice::paintGL()
{
    if (_onMax || _onMin) setCursor(Qt::SizeHorCursor);
    else setCursor(Qt::ArrowCursor);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    resizeGL(width(), height());

    glClearColor(0.75, 0.75, 0.75, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float range = (float)_valueMax-(float)_valueMin;
    float minX = 100.*((float)_currentMin-(float)_valueMin)/range;
    float maxX = 100.*((float)_currentMax-(float)_valueMin)/range;

    // fond
    QColor color = this->palette().button().color();
    QColor colorBG = color.dark(110);
    glColor3f(colorBG.redF(), colorBG.greenF(), colorBG.blueF());
    glBegin(GL_QUADS);
    glVertex3f(100,  0.5, -0.5);
    glVertex3f(100, -0.5, -0.5);
    glVertex3f(0  , -0.5, -0.5);
    glVertex3f(0  ,  0.5, -0.5);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(99.995,  0.495, -0.25);
    glVertex3f(99.995, -0.495, -0.25);
    glVertex3f( 0.005, -0.495, -0.25);
    glVertex3f( 0.005,  0.495, -0.25);
    glEnd();

    // dessin des zones non-selectionnables (bordures) avec la couleur de fond
    glColor3f(color.redF(), color.greenF(), color.blueF());
    glBegin(GL_QUADS);
    glVertex3f(- _cursorSize, 0.5, -0.3);
    glVertex3f(0, 0.5, -0.3);
    glVertex3f(0, -0.5, -0.3);
    glVertex3f(- _cursorSize, -0.5, -0.3);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(100, 0.5, -0.3);
    glVertex3f(100 + _cursorSize, 0.5, -0.3);
    glVertex3f(100 + _cursorSize, -0.5, -0.3);
    glVertex3f(100, -0.5, -0.3);
    glEnd();

    // dessin de la selection active
    QColor colorFG = color.dark(95);
    glColor3f(colorFG.redF(), colorFG.greenF(), colorFG.blueF());
    glBegin(GL_QUADS);
    glVertex3f(maxX, 0.5, -0.3);
    glVertex3f(maxX, -0.5, -0.3);
    glVertex3f(minX, -0.5, -0.3);
    glVertex3f(minX, 0.5, -0.3);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(maxX -0.005,  0.495, -0.2);
    glVertex3f(maxX -0.005, -0.495, -0.2);
    glVertex3f(minX -0.005, -0.495, -0.2);
    glVertex3f(minX -0.005,  0.495, -0.2);
    glEnd();

    // dessin des curseurs
    this->drawCursor(minX, 1);
    this->drawCursor(maxX, 2);

    glFlush();

    glPopAttrib();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void QSlice::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
                -(_cursorSize / 2.0), 100.0 + (_cursorSize / 2.0),	// X
                0.5, -0.5,	// Y
                -1.0, 1.0);	// Z
    glMatrixMode(GL_MODELVIEW);
}

void QSlice::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter;
    painter.begin(this);

    this->paintGL();
    painter.setRenderHint(QPainter::Antialiasing);

    // overpainting: c'est possible ici
    // (? mettre des HUD sur les curseurs lorsqu'on les deplacent ?)

    painter.end();
}

void QSlice::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        _mouseX = (float)(event->x()) /
                (float)this->width() *
                (100.0 + _cursorSize);
    _moved = 0;
}

void QSlice::mouseReleaseEvent(QMouseEvent *event)
{
    _moved = 0;
    this->mouseMoveEvent(event);
}

void QSlice::mouseMoveEvent(QMouseEvent *event)
{
    float mouseX =	(float)(event->x()) /
            (float)this->width() *
            (100.0 + _cursorSize);
    float range = (float)_valueMax-(float)_valueMin;
    float minX = 100.*((float)_currentMin-(float)_valueMin)/range;
    float maxX = 100.*((float)_currentMax-(float)_valueMin)/range;
    _onMin = ((((mouseX > minX) &&
                (mouseX < minX + _cursorSize)) ||
               ((_mouseX > minX) &&
                (_mouseX < minX + _cursorSize)))    // curseur au dessus
              && (_moved != 2)    // pas en train de bouger l'autre
              && !_onMax);            // pas au-dessus de l'autre
    _onMax = ((((mouseX > maxX) &&
                (mouseX < maxX + _cursorSize)) ||
               ((_mouseX > maxX) &&
                (_mouseX < maxX + _cursorSize)))    // curseur au dessus
              && (_moved != 1)    // pas en train de bouger l'autre
              && !_onMin);            // pas au dessus de l'autre

    if (event->buttons() & Qt::LeftButton)
    {
        if ((_moved != 2) && _onMin)
        {
            _moved = 1;
            minX += mouseX - _mouseX;
            if (minX < 0)	minX = 0;
            if (minX > 100) minX = 100;
            _currentMin = qRound(minX * range / 100.)+_valueMin;
            emit minChanged(this->currentMin());
            if (_currentMax < _currentMin)
            {
                _currentMax = _currentMin;
                emit maxChanged(this->currentMax());
            }
        }
        if ((_moved != 1) && _onMax)
        {
            _moved = 2;
            maxX += mouseX - _mouseX;
            if (maxX < 0)	maxX = 0;
            if (maxX > 100) maxX = 100;
            _currentMax = qRound(maxX * range / 100.)+_valueMin;
            if (_currentMin > _currentMax)
            {
                _currentMin = _currentMax;
                emit minChanged(this->currentMin());
            }
            emit maxChanged(this->currentMax());
        }
        //        this->printVals();
    }
    else
        _moved = 0;

    _mouseX = mouseX;
}

// dessin des curseurs
void QSlice::drawCursor(double pos, int id)
{
    if ((id != 1) && (id != 2))
        return;

    // profondeur (z-index)
    double z;
    if (_onMax != (id == 1))	z = 0.2;
    else					z = 0.0;

    // curseur du maximum
    glColor3f(0.625, 0.625, 0.625);
    glBegin(GL_QUADS);
    glVertex3f(pos - (_cursorSize / 2.0),	 0.4, -0.1 + z);
    glVertex3f(pos + (_cursorSize / 2.0),	 0.4, -0.1 + z);
    glVertex3f(pos + (_cursorSize / 2.0),	 0.2, -0.1 + z);
    glVertex3f(pos - (_cursorSize / 2.0),	 0.2, -0.1 + z);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(pos - (_cursorSize / 2.0),	-0.2, -0.1 + z);
    glVertex3f(pos + (_cursorSize / 2.0),	-0.2, -0.1 + z);
    glVertex3f(pos + (_cursorSize / 2.0),	-0.4, -0.1 + z);
    glVertex3f(pos - (_cursorSize / 2.0),	-0.4, -0.1 + z);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(pos - (_cursorSize / 2.0),	 0.2, -0.1 + z);
    glVertex3f(pos - (_cursorSize / 4.0),	 0.2, -0.1 + z);
    glVertex3f(pos - (_cursorSize / 4.0),	-0.2, -0.1 + z);
    glVertex3f(pos - (_cursorSize / 2.0),	-0.2, -0.1 + z);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(pos + (_cursorSize / 4.0),	 0.2, -0.1 + z);
    glVertex3f(pos + (_cursorSize / 2.0),	 0.2, -0.1 + z);
    glVertex3f(pos + (_cursorSize / 2.0),	-0.2, -0.1 + z);
    glVertex3f(pos + (_cursorSize / 4.0),	-0.2, -0.1 + z);
    glEnd();
    //~ if ((_onMax && (id == 2)) || (_onMin && (id == 1)))
    //~ {
    //~ if (_moved == id)
    //~ glColor3f(0.25, 0.25, 1.0);
    //~ else
    //~ glColor3f(0.25, 0.25, 0.75);
    //~ glBegin(GL_QUADS);
    //~ glVertex3f(pos - (_cursorSize / 4.0),	 0.2, -0.05 + z);
    //~ glVertex3f(pos + (_cursorSize / 4.0),	 0.2, -0.05 + z);
    //~ glVertex3f(pos + (_cursorSize / 4.0),	-0.2, -0.05 + z);
    //~ glVertex3f(pos - (_cursorSize / 4.0),	-0.2, -0.05 + z);
    //~ glEnd();
    //~ }
    if ((_onMax || _onMin) && (_moved == id))
    {
        glColor3f(0.25, 0.25, 0.75);
        glBegin(GL_QUADS);
        glVertex3f(pos - (_cursorSize / 4.0),	 0.2, -0.05 + z);
        glVertex3f(pos + (_cursorSize / 4.0),	 0.2, -0.05 + z);
        glVertex3f(pos + (_cursorSize / 4.0),	-0.2, -0.05 + z);
        glVertex3f(pos - (_cursorSize / 4.0),	-0.2, -0.05 + z);
        glEnd();
    }
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(pos - (_cursorSize / 2.0) +0.05,  0.4, 0.0 + z);
    glVertex3f(pos + (_cursorSize / 2.0) -0.05,  0.4, 0.0 + z);
    glVertex3f(pos + (_cursorSize / 2.0) -0.05, -0.4, 0.0 + z);
    glVertex3f(pos - (_cursorSize / 2.0) +0.05, -0.4, 0.0 + z);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(pos - (_cursorSize / 4.0) +0.05,  0.2, 0.0 + z);
    glVertex3f(pos + (_cursorSize / 4.0) -0.05,  0.2, 0.0 + z);
    glVertex3f(pos + (_cursorSize / 4.0) -0.05, -0.2, 0.0 + z);
    glVertex3f(pos - (_cursorSize / 4.0) +0.05, -0.2, 0.0 + z);
    glEnd();
    glColor3f(0.75, 0.75, 0.75);
    glBegin(GL_LINES);
    glVertex3f(pos - (_cursorSize / 4.0), -0.23, 0 + z);
    glVertex3f(pos - (_cursorSize / 4.0), -0.37, 0 + z);
    glVertex3f(pos - (_cursorSize / 4.0), +0.23, 0 + z);
    glVertex3f(pos - (_cursorSize / 4.0), +0.37, 0 + z);
    glVertex3f(pos - (_cursorSize / 8.0), -0.23, 0 + z);
    glVertex3f(pos - (_cursorSize / 8.0), -0.37, 0 + z);
    glVertex3f(pos - (_cursorSize / 8.0), +0.23, 0 + z);
    glVertex3f(pos - (_cursorSize / 8.0), +0.37, 0 + z);
    glVertex3f(pos						 , -0.23, 0 + z);
    glVertex3f(pos						 , -0.37, 0 + z);
    glVertex3f(pos						 , +0.23, 0 + z);
    glVertex3f(pos						 , +0.37, 0 + z);
    glVertex3f(pos + (_cursorSize / 8.0), -0.23, 0 + z);
    glVertex3f(pos + (_cursorSize / 8.0), -0.37, 0 + z);
    glVertex3f(pos + (_cursorSize / 8.0), +0.23, 0 + z);
    glVertex3f(pos + (_cursorSize / 8.0), +0.37, 0 + z);
    glVertex3f(pos + (_cursorSize / 4.0), -0.23, 0 + z);
    glVertex3f(pos + (_cursorSize / 4.0), -0.37, 0 + z);
    glVertex3f(pos + (_cursorSize / 4.0), +0.23, 0 + z);
    glVertex3f(pos + (_cursorSize / 4.0), +0.37, 0 + z);
    glEnd();
}

// affichage des changements
void QSlice::printVals()
{
    qDebug() << "Slice [" << _currentMin
             << "-" << _currentMax <<"] \t"
             << "Total [" << _valueMin
             << "-" << _valueMax << "]";
}
