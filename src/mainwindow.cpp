#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDockWidget>
#include <QStyle>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QTabWidget>


MainWindow::MainWindow(const QString filename)
{
    this->resize(800,600);
    setup(filename);
}

MainWindow::~MainWindow()
{
    delete mprview;

    delete view;
    delete labelTable;
    delete morpho;
    delete brush;
    delete regionGrowing;
    delete marchingCubes;
    delete landmarks;
    delete Segmentation3D; //Patotskaya
#ifdef USEGL
    delete glwidget;
# endif

    delete fileMenu;
    delete editMenu;
    delete viewMenu;
    delete helpMenu;

    delete toolBar;

    delete openAct;
    delete saveAsAct;
    delete openLabAct;
    delete saveLabAct;
    delete saveLabAsAct;
    delete aboutAct;
    delete quitAct;
    delete undoAct;
}


void MainWindow::setup(const QString filename)
{
    //    style()->standardIcon(QStyle::SP_DirOpenIcon);
    //    QIcon::fromTheme("document-open")
    //    icons from http://www.iconarchive.com/show/outline-icons-by-iconsmind.html

    openAct = new QAction(QIcon(":openimg")  , tr("&Open image..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open image"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(load()));

    saveAsAct = new QAction(QIcon(":save"), tr("&Save image as..."), this);
    saveAsAct->setStatusTip(tr("Save image as"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    openLabAct = new QAction(QIcon(":opensegm"), tr("&Open segmentation..."), this);
    openLabAct->setStatusTip(tr("Open segmentation"));
    connect(openLabAct, SIGNAL(triggered()), this, SLOT(loadSegmentation()));

    saveLabAct = new QAction(QIcon(":save"), tr("&Save segmentation"), this);
    saveLabAct->setShortcut(QKeySequence::Save);
    saveLabAct->setStatusTip(tr("Save segmentation"));
    connect(saveLabAct, SIGNAL(triggered()), this, SLOT(saveSegmentation()));

    saveLabAsAct = new QAction(QIcon(":save"), tr("&Save segmentation as..."), this);
    saveLabAsAct->setShortcut(QKeySequence::SaveAs);
    saveLabAsAct->setStatusTip(tr("Save segmentation as"));
    connect(saveLabAsAct, SIGNAL(triggered()), this, SLOT(saveAsSegmentation()));

    quitAct = new QAction(QIcon(":quit"),tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openLabAct);
    fileMenu->addAction(saveLabAct);
    fileMenu->addAction(saveLabAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    undoAct = new QAction(QIcon(":undo"), tr("&Undo last segmentation change"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo last segmentation change"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    aboutAct = new QAction(QIcon(":help"),tr("&About"), this);
    aboutAct->setStatusTip(tr("Show About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    toolBar = addToolBar(tr("File"));
    toolBar->addAction(openAct);
    toolBar->addAction(openLabAct);
    toolBar->addAction(saveLabAct);
    toolBar->addAction(undoAct);

    mprview = new MPRImageView(this,&img);
    connect(mprview, SIGNAL(statusChanged(const QString)), this, SLOT(changeStatus(const QString)));
#ifdef USEGL
    glwidget = new GLWidget(&img.meshes,&img.palette,this);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(glwidget);
    mainLayout->addWidget(mprview);
    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    glwidget->setVisible(false);
    glwidget->setXRotation(15 * 16);
    glwidget->setYRotation(345 * 16);
    glwidget->setZRotation(0 * 16);
# else
    setCentralWidget(mprview);
# endif

    for(unsigned int i=0;i<3;++i)    viewMenu->addAction(mprview->showViewAct[i]);
    viewMenu->addAction(mprview->showSlices);

    labelTable = new labelTableTool(mprview,&img);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(labelTable->getMenu(this));
    layout->addWidget(createTools());
    //    layout->addStretch();
    QWidget *newWidget = new QWidget(this);
    newWidget->setLayout(layout);

    QDockWidget *dockWidget = new QDockWidget(tr("ToolBox"),this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);
    dockWidget->setWidget(newWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    viewMenu->addAction(dockWidget->toggleViewAction());

    setWindowTitle("MIST");

    statusBar()->showMessage(tr("Ready"),2000);

    // try to load provided data
    if(filename.size())
    {
        if(img.loadImage( filename.toStdString().c_str()))
        {
            QFileInfo fileInfo(filename);
            QString labelfile = fileInfo.path()+QString("/")+fileInfo.baseName()+QString("_label.raw");
            QString namefile = fileInfo.path()+QString("/")+fileInfo.baseName()+QString("_names.txt");
            img.loadLabel( labelfile.toStdString().c_str());
            img.loadNames( namefile.toStdString().c_str());
        }
    }

    reinit();
}


void MainWindow::reinit()
{
    view->reinit();
    regionGrowing->reinit();
    mprview->reinit();
    labelTable->updateTable();
}

QWidget *MainWindow::createTools()
{
    view = new imageTool(mprview,&img);
    morpho = new morphoTool(mprview,&img);
    brush = new brushTool(mprview,&img);
    regionGrowing = new regionGrowingTool(mprview,&img);
    landmarks = new landmarksTool(mprview,&img);
    marchingCubes = new marchingCubesTool(mprview,&img);
    Segmentation3D = new Segment3DTool(mprview,&img); //Patotskaya

    QTabWidget* tab = new QTabWidget(this);
    tab->setIconSize(QSize(30,30));

    tab->addTab(view->getMenu(this),tr(""));
    tab->setTabIcon(0,QIcon(":image"));
    tab->setTabToolTip(0,tr("Image tools"));

    tab->addTab(morpho->getMenu(this),tr(""));
    tab->setTabIcon(1,QIcon(":morpho"));
    tab->setTabToolTip(1,tr("Morphological operators"));

    tab->addTab(brush->getMenu(this),tr(""));
    tab->setTabIcon(2,QIcon(":brush"));
    tab->setTabToolTip(2,tr("Brush tool"));

    tab->addTab(regionGrowing->getMenu(this),tr(""));
    tab->setTabIcon(3,QIcon(":regionGrow"));
    tab->setTabToolTip(3,tr("Region growing"));

    tab->addTab(landmarks->getMenu(this),tr(""));
    tab->setTabIcon(4,QIcon(":landmark"));
    tab->setTabToolTip(4,tr("Landmarks"));

    tab->addTab(marchingCubes->getMenu(this),tr(""));
    tab->setTabIcon(5,QIcon(":mesh"));
    tab->setTabToolTip(5,tr("Mesh tools"));

    // >>> Patotskaya
    // Add tool
    tab->addTab(Segmentation3D->getMenu(this),tr(""));
    tab->setTabIcon(6,QIcon(":segm3Dicon"));
    tab->setTabToolTip(6,tr("Segmentation 3D tool"));
    // <<< Patotskaya

    connect(tab, SIGNAL(currentChanged(int)),this, SLOT(ToolChanged(int)));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tab);
    //    layout->addStretch();

    QGroupBox *w = new QGroupBox(tr("Tools"),this);
    //    QWidget *w = new QWidget(this);
    w->setLayout(layout);
    w->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);

    return w;
}

void MainWindow::ToolChanged(int index)
{

    switch(index)
    {
    case 1:
        mprview->setViewMode(GraphView::Morpho);
        break;
    case 2:
        mprview->setViewMode(GraphView::Brush);
        break;
    case 3:
        regionGrowing->reinit(); // hack to propagate min/max changes from imagetools
        mprview->setViewMode(GraphView::RegionGrowing);
        break;
    case 4:
        mprview->setViewMode(GraphView::Landmarks);
        break;
    case 5:
        mprview->setViewMode(GraphView::MarchingCubes);
        break;
    //>>> Patotksya
    case 6:
        mprview->setViewMode(GraphView::Segmentation3D);
        break;
    //<<< Patotksya
    default:
        mprview->setViewMode(GraphView::Navigation);
        break;
    }

#ifdef USEGL
    mprview->setVisible(!(mprview->getViewMode()==GraphView::MarchingCubes));
    glwidget->setVisible(mprview->getViewMode()==GraphView::MarchingCubes);
#endif

}

//action.push_back(ACTION(49, "S+left button -- Select label on screen",cimg::keyS,M0,LEFT));
//action.push_back(ACTION(48,"Control+Z -- Cancel addition/removal",cimg::keyZ,CTRL));
//action.push_back(ACTION(12,""                                       ,cimg::keyPADSUB));
//action.push_back(ACTION(50, "Space -- Clear selection",cimg::keySPACE));


void MainWindow::load()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load Image"),QString(img.currentPath.c_str()),
                                                    tr("Images (*.mhd *.dcm *.hdr)"));

    if (fileName.isEmpty()) return;

    if(img.loadImage(fileName.toStdString().c_str()))
    {
        reinit();
        changeStatus(tr("Opened '%1'").arg(fileName));
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot open '%1'").arg(fileName));
}



void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Image"), QString(img.currentPath.c_str()),
                                                    tr("Images (*.mhd *.hdr)"));
    if (fileName.isEmpty()) return;

    if(img.saveImage(fileName.toStdString().c_str()))
    {
        changeStatus(tr("Saved '%1'").arg(fileName));
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot save '%1'").arg(fileName));

}



void MainWindow::loadSegmentation()
{
    QString path(img.currentPath.c_str());
    if(img.labelFileName.size()) path=QString(img.labelFileName.c_str());
    else if(img.imageFileName.size())
    {
        QFileInfo fileInfo(img.imageFileName.c_str());
        path = fileInfo.path()+QString("/")+fileInfo.baseName()+QString("_label.raw");
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load Segmentation"),path,
                                                    tr("Images (*.mhd *.dcm *.hdr *.raw)"));
    if (fileName.isEmpty()) return;

    if(img.loadLabel(fileName.toStdString().c_str()))
    {
        if(img.loadNames())        labelTable->updateTable();
        changeStatus(tr("Opened '%1'").arg(fileName));
        mprview->Render(true);
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot open '%1'").arg(fileName));
}


void MainWindow::saveAsSegmentation()
{
    QString path(img.currentPath.c_str());
    if(img.labelFileName.size()) path=QString(img.labelFileName.c_str());
    else if(img.imageFileName.size())
    {
        QFileInfo fileInfo(img.imageFileName.c_str());
        path = fileInfo.path()+QString("/")+fileInfo.baseName()+QString("_label.raw");
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Segmentation"), path,
                                                    tr("Images (*.mhd *.raw *.hdr)"));
    if (fileName.isEmpty()) return;

    if(img.saveLabel(fileName.toStdString().c_str()))
    {
        img.saveNames();
        changeStatus(tr("Saved '%1'").arg(fileName));
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot save '%1'").arg(fileName));

}

void MainWindow::saveSegmentation()
{
    QString fileName(img.labelFileName.c_str());
    if(QMessageBox::question(NULL, tr("qtSegmentation"),tr("Overwrite '%1' ?").arg(fileName))==QMessageBox::No) return;

    if(img.saveLabel(fileName.toStdString().c_str()))
    {
        img.saveNames();
        changeStatus(tr("Saved '%1'").arg(fileName));
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot save '%1'").arg(fileName));

}

void MainWindow::undo()
{
    img.undo();
    mprview->Render(true);
}

void MainWindow::changeStatus(const QString m)
{
    statusBar()->showMessage(m, 0);
}


void MainWindow::about()
{
    QMessageBox::information(this, tr("About MIST"),  tr("v1.1\nBenjamin GILLES"));
}
