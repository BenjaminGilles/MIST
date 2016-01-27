#include <QtGui>
#include <QtOpenGL>
#include <QtDebug>

#include <math.h>

#include "glWidget.h"

#define PICK_REGION_SIZE 10
#define TIMERREDRAW_PERIOD 5
#define P_SPHERE 1
#define P_DOF 2
#define P_CUBE 3


const GLdouble PI = 3.14159265358979323846264338327950288419717;
const GLfloat GREEN[4]={.2f,1,.2f,1};
const GLfloat BLUE[4]={.2f,.2f,1,1};
const GLfloat RED[4]={1,.2f,.2f,1};
const GLfloat BLACK[4]={0,0,0,1};
const GLfloat GREYTRANSP[4]={0.8f,0.8f,0.8f,0.3f};
const GLfloat GREY[4]={0.8f,0.8f,0.8f,1};
const GLfloat LIGHTGREY[4]={0.5,0.5,0.5,1};

void Primitive_Dof_rigid(const double size,const double linewidth)
{
    glLineWidth(linewidth);
    glBegin(GL_LINES);
    glColor4fv(RED);  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,RED); glVertex3d(0,0,0);glVertex3d(size,0,0);
    glColor4fv(GREEN); glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,GREEN); glVertex3d(0,0,0);glVertex3d(0,size,0);
    glColor4fv(BLUE);  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,BLUE); glVertex3d(0,0,0);glVertex3d(0,0,size);
    glEnd();
}


void Primitive_Cube(const double s=1,const bool wire=false)
{
    double ss2=s/2.;
    if(!wire) glBegin(GL_QUADS);

    if(wire) glBegin(GL_LINE_LOOP);
    glNormal3f(1,0,0);   glVertex3d(ss2,-ss2,-ss2);         glNormal3f(1,0,0);   glVertex3d(ss2,-ss2,ss2);         glNormal3f(1,0,0);   glVertex3d(ss2,ss2,ss2);         glNormal3f(1,0,0);   glVertex3d(ss2,ss2,-ss2);
    if(wire) glEnd ();

    if(wire) glBegin(GL_LINE_LOOP);
    glNormal3f(-1,0,0);   glVertex3d(-ss2,-ss2,-ss2);         glNormal3f(-1,0,0);   glVertex3d(-ss2,-ss2,ss2);         glNormal3f(-1,0,0);   glVertex3d(-ss2,ss2,ss2);         glNormal3f(-1,0,0);   glVertex3d(-ss2,ss2,-ss2);
    if(wire) glEnd ();

    if(wire) glBegin(GL_LINE_LOOP);
    glNormal3f(0,1,0);   glVertex3d(-ss2,ss2,-ss2);         glNormal3f(0,1,0);   glVertex3d(ss2,ss2,-ss2);         glNormal3f(0,1,0);   glVertex3d(ss2,ss2,ss2);         glNormal3f(0,1,0);   glVertex3d(-ss2,ss2,ss2);
    if(wire) glEnd ();

    if(wire) glBegin(GL_LINE_LOOP);
    glNormal3f(0,-1,0);   glVertex3d(-ss2,-ss2,-ss2);         glNormal3f(0,-1,0);   glVertex3d(ss2,-ss2,-ss2);         glNormal3f(0,-1,0);   glVertex3d(ss2,-ss2,ss2);         glNormal3f(0,-1,0);   glVertex3d(-ss2,-ss2,ss2);
    if(wire) glEnd ();

    if(wire) glBegin(GL_LINE_LOOP);
    glNormal3f(0,0,1);   glVertex3d(-ss2,-ss2,ss2);         glNormal3f(0,0,1);   glVertex3d(-ss2,ss2,ss2);         glNormal3f(0,0,1);   glVertex3d(ss2,ss2,ss2);         glNormal3f(0,0,1);   glVertex3d(ss2,-ss2,ss2);
    if(wire) glEnd ();

    if(wire) glBegin(GL_LINE_LOOP);
    glNormal3f(0,0,-1);   glVertex3d(-ss2,-ss2,-ss2);         glNormal3f(0,0,-1);   glVertex3d(-ss2,ss2,-ss2);         glNormal3f(0,0,-1);   glVertex3d(ss2,ss2,-ss2);         glNormal3f(0,0,-1);   glVertex3d(ss2,-ss2,-ss2);
    glEnd ();
}

void Primitive_Sphere(const int a=10,const int b=10,const double r=1,const bool wire=false)
{
    int i,j;
    double da = ( PI / a ),db = ( 2.0f * PI / b ),r0,y0,r1,y1,x0,z0,x1,z1,x2,z2,x3,z3;
    if(!wire) glBegin(GL_QUADS);

    for( i = 0; i < a + 1 ; i++ )
    {
        r0 = r * sin ( i * da );
        y0 = r * cos ( i * da );
        r1 = r * sin ( (i+1) * da );
        y1 = r * cos ( (i+1) * da );

        for( j = 0; j < b + 1 ; j++ )
        {
            x0 = r0 * sin( j * db );
            z0 = r0 * cos( j * db );
            x1 = r0 * sin( (j+1) * db );
            z1 = r0 * cos( (j+1) * db );

            x2 = r1 * sin( j * db );
            z2 = r1 * cos( j * db );
            x3 = r1 * sin( (j+1) * db );
            z3 = r1 * cos( (j+1) * db );

            if(wire) glBegin(GL_LINE_LOOP);
            glNormal3f(x0/r,y0/r,z0/r);   glVertex3d(x0,y0,z0);
            glNormal3f(x1/r,y0/r,z1/r);   glVertex3d(x1,y0,z1);
            glNormal3f(x3/r,y1/r,z3/r);   glVertex3d(x3,y1,z3);
            glNormal3f(x2/r,y1/r,z2/r);   glVertex3d(x2,y1,z2);
            if(wire) glEnd ();
        }
    }
    if(!wire) glEnd ();
}


GLWidget::GLWidget(std::vector<MESH>* m, CImg<unsigned char>* p, QWidget *parent)
    : QGLWidget(parent),meshes(m),palette(p)
{
    xRot = 0;    yRot = 0;    zRot = 0;
    xTr = 0;     yTr = 0;     zTr = 0;

    makeCurrent();
    setFocusPolicy(Qt::ClickFocus);

    glNewList (P_SPHERE, GL_COMPILE);
    glColor4fv(RED); glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,RED);
    Primitive_Sphere(100,100,0.5,true);
    glEndList ();
    glNewList (P_DOF, GL_COMPILE);
    Primitive_Dof_rigid(1,2);
    glEndList ();
    glNewList (P_CUBE, GL_COMPILE);
    Primitive_Cube(1);
    glEndList ();

    wireframe=0;
    selected[0]=-1;
    center[0]=0;    center[1]=0;    center[2]=0;
}


GLWidget::~GLWidget()
{
}


//QSize GLWidget::minimumSizeHint() const {    return QSize(50, 50); }
//QSize GLWidget::sizeHint() const {    return QSize(400, 400); }

void moduloAngle(double *angle)
{
    while (*angle < 0)    *angle += 360;
    while (*angle > 360)    *angle -= 360;
}

void GLWidget::setXRotation(double angle) {    moduloAngle(&angle);    if (angle != xRot) {   xRot = angle;        Render();  } }
void GLWidget::setYRotation(double angle) {    moduloAngle(&angle);    if (angle != yRot) {   yRot = angle;        Render();  } }
void GLWidget::setZRotation(double angle) {    moduloAngle(&angle);    if (angle != zRot) {   zRot = angle;        Render();  } }
void GLWidget::setXTranslation(double val) {   if (val != xTr) {   xTr = val;        Render();  } }
void GLWidget::setYTranslation(double val) {   if (val != yTr) {   yTr = val;        Render();  } }
void GLWidget::setZTranslation(double val) {   if (val != zTr) {   zTr = val;        Render();  } }

void GLWidget::mouseReleaseEvent(QMouseEvent */*event*/)
{
    selected[0]=-1;
    Render();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if(event->modifiers() & Qt::ControlModifier) {Select(); Render();}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (!event->modifiers() && event->buttons() & Qt::LeftButton) { qDebug()<<"rotate"; setXRotation(xRot + 1 * dy); setYRotation(yRot + 1 * dx); }
    else if (event->modifiers() & Qt::ShiftModifier && event->buttons() & Qt::LeftButton) {qDebug()<<"translate";  setXTranslation(xTr + 1 * dx); setYTranslation(yTr - 1 * dy); }
    else if (!event->modifiers() && event->buttons() & Qt::MiddleButton) { qDebug()<<"zoom";  setZTranslation(zTr - 1 * dy); }

    lastPos = event->pos();

    //    if (selected[0]!=-1)
    //    {
    //        GLint viewport[4]; glGetIntegerv(GL_VIEWPORT, viewport);
    //        GLdouble modelview[16]; glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    //        GLdouble projection[16]; glGetDoublev(GL_PROJECTION_MATRIX, projection);
    //        GLdouble pin[3]; gluProject(selectedP[0],selectedP[1],selectedP[2],modelview,projection,viewport,pin,pin+1,pin+2);
    //        pin[0]=lastPos.x(); pin[1]=viewport[3]-lastPos.y();
    //        gluUnProject(pin[0],pin[1],pin[2],modelview,projection,viewport,moveP,moveP+1,moveP+2);
    //        if(!redrawTimer.isActive())    Render();
    //    }

}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_R:
        xRot = 0;    yRot = 0;    zRot = 0;
        xTr = 0;     yTr = 0;     zTr = 0;
        break;
    case Qt::Key_W:
         wireframe++; if(wireframe>=3) wireframe=0;
    break;
    default:

        break;
    }
    Render();
}




void GLWidget::initializeGL()
{
    glRenderMode(GL_RENDER);
    glClearColor(1,1,1, 0.0); // background color
    glShadeModel(GL_SMOOTH);//GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    //    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND) ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
    glEnable(GL_LINE_STIPPLE);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glPolygonMode ( GL_FRONT_AND_BACK,GL_FILL );
    glPolygonOffset(1., 1.);

    connect(&redrawTimer, SIGNAL(timeout()), this, SLOT(Render()));
}



void GLWidget::Render()
{
    if(!this->isVisible()) return;
    setupView();
    glDraw();
}



void GLAPIENTRY pickMatrixGL(GLdouble x, GLdouble y, GLdouble deltax, GLdouble deltay,GLint viewport[4])
{
    if (deltax <= 0 || deltay <= 0) return;
    /* Translate and scale the picked region to the entire window */
    glTranslatef((viewport[2] - 2 * (x - viewport[0])) / deltax,(viewport[3] - 2 * (y - viewport[1])) / deltay, 0);
    glScalef(viewport[2] / deltax, viewport[3] / deltay, 1.0);
}
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    GLdouble fW, fH;
    fH = tan( fovY / 360 * PI ) * zNear;
    fW = fH * aspect;
    glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

void GLWidget::setupView()
{
    GLint rendermode; glGetIntegerv(GL_RENDER_MODE,&rendermode);
    GLint viewport[4]; glGetIntegerv(GL_VIEWPORT, viewport);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(rendermode==GL_SELECT)         pickMatrixGL(lastPos.x(),viewport[3]-lastPos.y()+2*viewport[1],PICK_REGION_SIZE, PICK_REGION_SIZE,viewport);

  //  glOrtho (-10, 10,-10, 10, -1000.0, 1000.0);
    perspectiveGL (10, 1.0, 0.1, 10000.0); /* near and far clipping planes */
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    setupView();
}

void GLWidget::paintGL()
{
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // translations
    glTranslated(xTr, yTr, zTr);

    // rotation wrt. center
    glTranslated(center[0],center[1],center[2]);
    glRotated(xRot , 1.0, 0.0, 0.0);
    glRotated(yRot , 0.0, 1.0, 0.0);
    glRotated(zRot , 0.0, 0.0, 1.0);
    glTranslated(-center[0],-center[1],-center[2]);

    GLint rendermode; glGetIntegerv(GL_RENDER_MODE,&rendermode);

    DrawMeshes();

    if(rendermode!=GL_SELECT) glCallList (P_DOF);

    if(selected[0]!=-1)
    {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor4fv(BLACK);glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,BLACK);
        glVertex3d(selectedP[0],selectedP[1],selectedP[2]);
        glVertex3d(moveP[0],moveP[1],moveP[2]);
        glEnd();
    }

    swapBuffers ();
}

/*
void GLWidget::DrawDof_rigid()
{
        Vec4 *q;
        Vec3 *t,u;
        double n,w;
        if(refpos) {q=&dof_rigid.x0[index].q; t=&dof_rigid.x0[index].t;}
        else {q=&dof_rigid.x[index].q; t=&dof_rigid.x[index].t;}
        u[0]=(*q)[0]; u[1]=(*q)[1]; u[2]=(*q)[2];
        n=sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]); if(n!=0) {u[0]/=n; u[1]/=n; u[2]/=n;}
        w=360.*acos((*q)[3])/PI;

        glTranslated ((*t)[0],(*t)[1],(*t)[2]);
        glRotated(w,u[0],u[1],u[2]);

    glCallList (P_DOF);

        glRotated(w,-u[0],-u[1],-u[2]);
        glTranslated (-(*t)[0],-(*t)[1],-(*t)[2]);
}
*/

void GLWidget::DrawMeshes()
{
    // selected points

//    glTranslatef (0, 0, 0);   glCallList (P_SPHERE);   glTranslatef (-0,- 0, -0);
    //   glTranslatef (10, 10, 10);   glCallList (P_SPHERE);   glTranslatef (-10,- 10, -10);

    //    if(rightclick)       glInitNames();

    center[0]=0;    center[1]=0;    center[2]=0;
    unsigned int nbp=0;
        for(unsigned int i=0;i<meshes->size();i++)
        {
            DrawMesh(i);
            for(unsigned int k=0;k<3;k++)
            {
                nbp+=(*meshes)[i].vertices.width();
                center[k]+=(*meshes)[i].center[k]*(*meshes)[i].vertices.width();
            }
        }
    if(nbp) for(unsigned int k=0;k<3;k++)  { center[k]/=(double)nbp; }
}

void GLWidget::DrawMesh(const int index,const GLfloat /*alpha*/)
{
    glPushName(index);

    cimglist_for((*meshes)[index].faces,j)
        {
        if(wireframe!=1) // solid
        {
            glPushName(j);

            glColor4fv(GREY); glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,GREY);

            glEnable(GL_POLYGON_OFFSET_FILL);

            glBegin(GL_POLYGON);
            cimg_forY((*meshes)[index].faces(j),k)
            {
                unsigned int p=(*meshes)[index].faces(j)(k);
                glVertex3d((*meshes)[index].vertices(p,0),(*meshes)[index].vertices(p,1),(*meshes)[index].vertices(p,2));
            }
            glEnd();
            glDisable(GL_POLYGON_OFFSET_FILL);

            glPopName();
        }

        if(wireframe!=0) // wireframe
        {
            glLineWidth(1);
            glColor4fv(BLACK); glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,BLACK);
            glBegin(GL_LINE_LOOP);
            cimg_forY((*meshes)[index].faces(j),k)
            {
                unsigned int p=(*meshes)[index].faces(j)(k);
                glVertex3d((*meshes)[index].vertices(p,0),(*meshes)[index].vertices(p,1),(*meshes)[index].vertices(p,2));
            }
            glEnd();
        }
    }

 //   if(rightclick)  glPopName();
}

/*
void GLWidget::Draw_phys(bool useref)
{
    int i,nb=phys.size();
    double s,w;
    Vec3 dw;

    for(i=0;i<nb;i++)
    {
        if(mapweight!=-1)
        {
            if(mapweight<dof_rigid.nb) GetWeight(&w,&dw,phys[i],mapweight,rigid);
            else if(mapweight<dof_rigid.nb+dof_affine.nb) GetWeight(&w,&dw,phys[i],mapweight-dof_rigid.nb,affine);
            else if(mapweight<dof_rigid.nb+dof_affine.nb+dof_quadratic.nb) GetWeight(&w,&dw,phys[i],mapweight-dof_rigid.nb-dof_affine.nb,quadratic);
        }

        glLineWidth(1);
        double alpha;

        if(phys[i]->Tissu==FAT || phys[i]->Tissu==SKIN) { alpha=GREYTRANSP[3];	glColor4fv(GREYTRANSP); glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,GREYTRANSP); }
        else {alpha=1;	glColor4fv(GREY); glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,GREY);}

        if(mapweight!=-1) MapWeight(w,alpha);
        else if(mapdet) MapDet(phys[i]->det,alpha);
        else if(mapstrain) MapStrain(phys[i]->e,alpha);
        else if(mapstress) MapVonMisesStress(phys[i]->stress,alpha);
        else if(mapenergy) MapEnergy(phys[i]->U,alpha);
        else if(mapbendingenergy) MapEnergy(phys[i]->Ubending,alpha);

        s=pow(phys[i]->vol,1./3.); // m^3
        Vec3 *p=&phys[i]->p; if(useref) p=&phys[i]->p0;

        glTranslated ((*p)[0],(*p)[1],(*p)[2]);
        Primitive_Cube(s);
        glTranslated (-(*p)[0],-(*p)[1],-(*p)[2]);

        // draw dw
        if(mapweight!=-1)
        {
            //double factor=.005;//foot..
            double factor=.05; //bar
            glLineWidth(2);
            glColor4fv(BLACK); glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,BLACK);

            glBegin(GL_LINES);
            glVertex3d((*p)[0],(*p)[1],(*p)[2]);
            glVertex3d((*p)[0]+factor*dw[0],(*p)[1]+factor*dw[1],(*p)[2]+factor*dw[2]);
            glEnd();
        }
    }
}
*/


//void GLWidget::DrawDofs()
//{
//    if(!rightclick) glInitNames();

//    //glTranslatef (0, 0, 0);   glCallList (P_DOF);   glTranslatef (-0,- 0, -0);

//    DrawDof_rigid();
//}


void GLWidget::Select()
{
    bool timer=false;
    if(redrawTimer.isActive())
    {
        timer=true;
        redrawTimer.stop();
    }

    GLuint select_buf[GL_SELECTION_BUFFER_SIZE]; //selection buffer
    GLuint hits; //number of hits

    glSelectBuffer(GL_SELECTION_BUFFER_SIZE, select_buf);
    glRenderMode(GL_SELECT);

    Render();
    glFlush(); //flush the pipeline
    hits = glRenderMode(GL_RENDER); //switch back to RENDER mode


    GLuint *bufp = select_buf, names, numnames=0,*ptrNames,selectedZ= 0xffffffff;
    selected[0]=-1;

    for(unsigned int j = 0; j < hits; j++)
    {
        //        glReadPixels(lastPos.x(),viewport[3]-lastPos.y()+2*viewport[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

        names = *bufp++;
        if (*bufp < selectedZ)
        {
            numnames = names;
            selectedZ = *bufp;
            ptrNames = bufp+2;
        }
        bufp += names+2;
    }

    bufp = ptrNames;
    qDebug()<<"hits";  for (int j = 0; j < (int)numnames; j++,bufp++)  qDebug()<< *bufp;
    /*
    if(numnames!=0)
    {
        if(rightclick && numnames>=2)
        {
            selected[0]=ptrNames[0]; selected[1]=mesh[selected[0]].f[ptrNames[1]][0];
            memcpy(selectedP,&mesh[selected[0]].n[selected[1]].p[0],3*sizeof(double));
        }
        else
        {
            selected[0]=ptrNames[0];
            if(selected[0]<dof_rigid.nb) memcpy(selectedP,&dof_rigid.x[selected[0]].t[0],3*sizeof(double));
            else if(selected[0]<dof_rigid.nb+dof_affine.nb) memcpy(selectedP,&dof_affine.x[selected[0]-dof_rigid.nb].t[0],3*sizeof(double));
            else memcpy(selectedP,&dof_quadratic.x[selected[0]-dof_rigid.nb-dof_affine.nb].t[0],3*sizeof(double));
        }
        memcpy(moveP,selectedP,3*sizeof(double));
    }*/


    if(timer)    redrawTimer.start(TIMERREDRAW_PERIOD);
}





