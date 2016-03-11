#include "qSlice.h"

#include <QGridLayout>
#include <QDebug>

QSlice::QSlice(QWidget *parent)
    : QWidget(parent)
{
    _before = new QWidget(this);
    _before->setPalette(QPalette(QColor(191, 191, 255)));
    _before->setAutoFillBackground(true);

    _selection = new QWidget(this);
    _selection->setPalette(QPalette(QColor(191, 255, 191)));
    _selection->setAutoFillBackground(true);

    _after = new QWidget(this);
    _after->setPalette(QPalette(QColor(191, 191, 255)));
    _after->setAutoFillBackground(true);

    _splitter = new QSplitter(Qt::Horizontal, this);
    _splitter->addWidget(_before);
    _splitter->addWidget(_selection);
    _splitter->addWidget(_after);
    _splitter->setHandleWidth(12);

    QGridLayout *grid = new QGridLayout();
    grid->setMargin(0);
    grid->addWidget(_splitter, 0, 0);
    this->setLayout(grid);

    QObject::connect(_splitter, SIGNAL(splitterMoved(int, int)),this, SLOT(updateValues(int, int))	);

    _valueMin = std::numeric_limits<int>::min();
    _valueMax = std::numeric_limits<int>::max();
    _currentMin = _valueMin;
    _currentMax = _valueMax;
}

QSlice::~QSlice()
{
    delete _before;
    delete _selection;
    delete _after;
    delete _splitter;
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
    this->updateSizes();
}

void QSlice::setCurrentMax(int val)
{
    if (val > _valueMax) _currentMax=_valueMax;
    else if (val < _valueMin) _currentMax=_valueMin;
    else _currentMax = val;
    if (_currentMin > _currentMax) _currentMin = _currentMax;
    this->updateSizes();
}

void QSlice::setValueMin(int val)
{
    if (val > _valueMax) return;
    _valueMin = val;
    if (_currentMin < _valueMin) _currentMin = _valueMin;
    if (_currentMax < _valueMin) _currentMax = _valueMin;
    this->updateSizes();
}

void QSlice::setValueMax(int val)
{
    if (val < _valueMin) return;
    _valueMax = val;
    if (_currentMin > _valueMax) _currentMin = _valueMax;
    if (_currentMax > _valueMax) _currentMax = _valueMax;
    this->updateSizes();
}

void QSlice::updateSizes()
{
    float factor = ((float)_valueMax-(float)_valueMin) / ((float)this->width() - (float)_splitter->handleWidth()*2.) ;
    QList<int> sizes;
    sizes<< qRound( ((float)_currentMin - (float)_valueMin) /factor );
    sizes<< qRound( ((float)_currentMax - (float)_currentMin) /factor );
    sizes<< qRound( ((float)_valueMax - (float)_currentMax) /factor );
    _splitter->setSizes(sizes);
}

void QSlice::updateValues(int pos, int index)
{
    float factor = ((float)_valueMax-(float)_valueMin) / ((float)this->width() - (float)_splitter->handleWidth()*2.) ;
    if (index == 1)
    {
        _currentMin = qRound( (float)pos * factor  + (float)_valueMin );
        if (_currentMin > _currentMax)
        {
            _currentMax = _currentMin;
            emit maxChanged(_currentMax);
        }
        emit minChanged(_currentMin);
    }
    if (index == 2)
    {
        _currentMax = qRound( ((float)pos-(float)_splitter->handleWidth()) * factor + (float)_valueMin );
        if (_currentMax < _currentMin)
        {
            _currentMin = _currentMax;
            emit minChanged(_currentMin);
        }
        emit maxChanged(_currentMax);
    }
//    printVals();
}


void QSlice::printVals()
{
    qDebug() << "Slice [" << _currentMin << "-" << _currentMax <<"] \t"
             << "Total [" << _valueMin << "-" << _valueMax << "]";
}

