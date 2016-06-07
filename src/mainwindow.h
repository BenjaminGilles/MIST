#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <imageview.h>
#include <image.h>

#include <tools/imageTool.h>
#include <tools/labelTable.h>
#include <tools/morpho.h>
#include <tools/brush.h>
#include <tools/marchingCubes.h>
#include <tools/regionGrowing.h>
#include <tools/landmarks.h>
#include <tools/segmentation3D.h>

#ifdef USEGL
#include <widgets/glWidget.h>
# endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString filename);
    ~MainWindow();

public slots:
    void about();
    void load();
    void saveAs();
    void loadSegmentation();
    void saveSegmentation();
    void saveAsSegmentation();
    void undo();

    void changeStatus(const QString);

    void reinit();
    void ToolChanged(int index);


private:
    void setup(const QString filename);
    QWidget* createTools();

    imageTool* view;
    labelTableTool* labelTable;
    morphoTool* morpho;
    brushTool* brush;
    regionGrowingTool* regionGrowing;
    marchingCubesTool* marchingCubes;
    landmarksTool* landmarks;
    // Patotskaya
    Segment3DTool* Segmentation3D;

#ifdef USEGL
    GLWidget* glwidget;
# endif

    image<T> img;
    MPRImageView* mprview;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QToolBar *toolBar;

    QAction *openAct;
    QAction *saveAsAct;
    QAction *openLabAct;
    QAction *saveLabAsAct;
    QAction *saveLabAct;
    QAction *aboutAct;
    QAction *quitAct;
    QAction *undoAct;
};

#endif // MAINWINDOW_H
