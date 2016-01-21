#ifndef __QSLICE_H
#define __QSLICE_H

#include <QObject>
#include <QGLWidget>
#include <QTimer>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDebug>

class QMouseEvent ;

// double slider: from http://forum.qtfr.org/discussion/3830/qt4-bi-slider-avec-qsplitter-probleme-de-deplacement-du-qsplitter

class QSlice : public QGLWidget
{
    Q_OBJECT

    private:
        int		_currentMin;	// valeur minimale courante
        int		_currentMax;	// valeur maximale courante
        int		_valueMin;		// valeur minimale limite
        int		_valueMax;		// valeur maximale limite
        double		_cursorSize;	// taille du curseur
        QTimer		_refreshTimer;	// timer de maj
        double 		_mouseX;		// derni?re position de la souris
        int 		_moved;			// indice de l'?l?ment d?plac?
        bool 		_onMin;			// sur le curseur min
        bool 		_onMax;			// sur le curseur max

    public:
        QSlice(QWidget *parent = NULL);	// constructeur
        ~QSlice();						// destructeur

        QSize sizeHint() const;			// taille par defaut

        //getters
        int currentMin() const {return _currentMin;}
        int currentMax() const {return _currentMax;}
        int valueMin() const {return _valueMin;}
        int valueMax() const {return _valueMax;}

        //setters
        void setValueMin(int valueMin);
        void setValueMax(int valueMax);

    public slots:
        void setCurrentMin(int currentMin);
        void setCurrentMax(int currentMax);
        void printVals();

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);
        void paintEvent(QPaintEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

        void drawCursor(double pos, int id);// id = 1 -> min
                                            // id = 2 -> max

    signals:
        void minChanged(int val);
        void maxChanged(int val);
};

//-----------------------------------------------------------------------------------------------//
//	slider + labels
//-----------------------------------------------------------------------------------------------//


class QRangeWidget: public QWidget
{
    Q_OBJECT

public:

    QRangeWidget(QWidget *parent=NULL)
        : QWidget(parent)
    {
        rangeMin = new QLineEdit(tr("-1000"),this);
        rangeMin->setAlignment(Qt::AlignCenter);
        rangeMin->setFixedWidth ( 50 );
        connect(rangeMin, SIGNAL( textChanged(const QString&) ), this, SLOT( minTextChanged(const QString&) ) );

        rangeMax = new QLineEdit(tr("1000"),this);
        rangeMax->setAlignment(Qt::AlignCenter);
        rangeMax->setFixedWidth ( 50 );
        connect(rangeMax, SIGNAL( textChanged(const QString&) ), this, SLOT( maxTextChanged(const QString&) ) );

        slider = new QSlice(this);
        connect(slider, SIGNAL( minChanged(int) ), this, SLOT( minSliderChanged(int) ) );
        connect(slider, SIGNAL( maxChanged(int) ), this, SLOT( maxSliderChanged(int) ) );

        QHBoxLayout* hlayout = new QHBoxLayout(this);
        hlayout->setMargin(0);
        hlayout->setSpacing(10);
        hlayout->addWidget(rangeMin);
        hlayout->addWidget(slider);
        hlayout->addWidget(rangeMax);

        this->setLayout(hlayout);
    }

    ~QRangeWidget()
    {
        delete rangeMin;
        delete rangeMax;
        delete slider;
    }

signals:
    void rangeChanged(int,int);

public slots:

    void setRangeLimits(int _min,int _max)
    {
        slider->setValueMin(_min);
        slider->setValueMax(_max);
        slider->setCurrentMin(_min);
        slider->setCurrentMax(_max);
        updateText();
    }

    void updateText()
    {
        rangeMin->blockSignals(true);
        rangeMax->blockSignals(true);
        rangeMin->setText(QString::number(slider->currentMin()));
        rangeMax->setText(QString::number(slider->currentMax()));
        rangeMin->blockSignals(false);
        rangeMax->blockSignals(false);
    }

    void minTextChanged(const QString& txt) // when lineedit changes
    {
        bool ok;
        int val=txt.toInt(&ok);
        if(!ok)
        {
            rangeMin->setText(QString::number(slider->currentMin()));
            return;
        }
        slider->setCurrentMin(val);
        updateText();
        emit rangeChanged(slider->currentMin(),slider->currentMax());
    }

    void maxTextChanged(const QString& txt) // when lineedit changes
    {
        bool ok;
        int val=txt.toInt(&ok);
        if(!ok)
        {
            rangeMax->setText(QString::number(slider->currentMax()));
            return;
        }
        slider->setCurrentMax(val);
        updateText();
        emit rangeChanged(slider->currentMin(),slider->currentMax());
    }

    void minSliderChanged(int /*val*/) // when slider moves
    {
        updateText();
        emit rangeChanged(slider->currentMin(),slider->currentMax());
    }

    void maxSliderChanged(int /*val*/) // when slider moves
    {
        updateText();
        emit rangeChanged(slider->currentMin(),slider->currentMax());
    }

protected:
    QLineEdit *rangeMin;
    QLineEdit *rangeMax;
    QSlice *slider;
};




#endif
