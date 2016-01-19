#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <imageview.h>
#include <image.h>

#include <tools/view.h>
#include <tools/labelTable.h>
#include <tools/morpho.h>
#include <tools/brush.h>
#include <tools/marchingCubes.h>
#include <tools/regionGrowing.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString filename);
    ~MainWindow();

public slots:
    void about();
    void load();
    void loadSegmentation();
    void saveSegmentation();
    void changeStatus(const QString);

    void reinit();
    void ToolChanged(int index);


private:
    void setup(const QString filename);
    QGroupBox* createTools();

    viewTool* view;
    labelTableTool* labelTable;
    morphoTool* morpho;
    brushTool* brush;
    regionGrowingTool* regionGrowing;
    marchingCubesTool* marchingCubes;

    image<T> img;
    MPRImageView* mprview;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QToolBar *toolBar;

    QAction *openAct;
    QAction *openLabAct;
    QAction *saveAct;
    QAction *aboutAct;
    QAction *quitAct;
};

#endif // MAINWINDOW_H
