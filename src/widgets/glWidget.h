#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QTimer>
#include <QGLWidget>
#include <float.h>
#include <QDebug>
#include <vector>
#include "image.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(std::vector<MESH>* m, CImg<unsigned char>* p, QWidget *parent = 0);
    ~GLWidget();

//    QSize minimumSizeHint() const;
//    QSize sizeHint() const;

public slots:
    void Render();

    void setXRotation(double angle);     void setYRotation(double angle);     void setZRotation(double angle);
    void setXTranslation(double val);    void setYTranslation(double val);    void setZTranslation(double angle);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void setupView();

    void Select(); int selected[2]; double moveP[3]; double selectedP[3];

//    std::vector<MESH> mesh;
    void DrawMeshes();
    void DrawMesh(const int index,const GLfloat alpha=1.0);
    void IniVisual();

    unsigned int wireframe;
    GLdouble center[3];

    double xRot;   double yRot;    double zRot;
    double xTr;    double yTr;     double zTr;
    QPoint lastPos;

    QTimer redrawTimer;

    std::vector<MESH>* meshes;
    CImg<unsigned char>* palette;
};




#endif
