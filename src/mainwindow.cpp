#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDockWidget>
#include <QStyle>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QTabWidget>



MainWindow::MainWindow()
{
    this->resize(800,600);
    setup();
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

    delete fileMenu;
    delete viewMenu;
    delete helpMenu;

    delete toolBar;

    delete openAct;
    delete openLabAct;
    delete aboutAct;
    delete quitAct;
}


void MainWindow::setup()
{
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    openAct = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open image"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(load()));

    openLabAct = new QAction(style()->standardIcon(QStyle::SP_DirOpenIcon), tr("&Open segmentation..."), this);
    openLabAct->setStatusTip(tr("Open segmentation"));
    connect(openLabAct, SIGNAL(triggered()), this, SLOT(loadSegmentation()));

    saveAct = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Save segmentation..."), this);
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save segmentation"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveSegmentation()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(openLabAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    toolBar = addToolBar(tr("File"));
    toolBar->addAction(openAct);
    toolBar->addAction(openLabAct);
    toolBar->addAction(saveAct);

    mprview = new MPRImageView(this,&img);
    connect(mprview, SIGNAL(statusChanged(const QString)), this, SLOT(changeStatus(const QString)));
    setCentralWidget(mprview);

    for(unsigned int i=0;i<3;++i)    viewMenu->addAction(mprview->showViewAct[i]);
    viewMenu->addAction(mprview->showSlices);

    view = new viewTool(mprview,&img);
    labelTable = new labelTableTool(mprview,&img);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(view->getMenu());
    layout->addWidget(labelTable->getMenu());
    layout->addWidget(createTools());
    layout->addStretch();
    QWidget *newWidget = new QWidget(this);
    newWidget->setLayout(layout);

    QDockWidget *dockWidget = new QDockWidget(tr("ToolBox"),this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);
    dockWidget->setWidget(newWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    viewMenu->addAction(dockWidget->toggleViewAction());

    setWindowTitle("QtSegmentation");

    statusBar()->showMessage(tr("Ready"),2000);


    img.loadImage("../data/pied.hdr",true);
    img.loadLabel("../data/pied_label.raw");
    img.loadNames("../data/pied_names.txt");

//    img.loadImage("/data/Data/MRI/sohusim/EP/EP_JAMBE_16_03_2012_SOHUSIM/merge.mhd",true);
//    img.loadLabel("/data/Data/MRI/sohusim/EP/EP_JAMBE_16_03_2012_SOHUSIM/merge_label.raw");
//    img.loadNames("/data/Data/MRI/sohusim/EP/EP_JAMBE_16_03_2012_SOHUSIM/merge_names.txt");
//    qDebug()<<"size:"<<img.dim[0]<<","<<img.dim[1]<<","<<img.dim[2];

    view->reinit();
    mprview->reinit();
    labelTable->updateTable();
}




QGroupBox *MainWindow::createTools()
{
    morpho = new morphoTool(mprview,&img);
    brush = new brushTool(mprview,&img);
    regionGrowing = new regionGrowingTool(mprview,&img);
    marchingCubes = new marchingCubesTool(mprview,&img);

    QGroupBox *groupBox = new QGroupBox(tr("Tools"));

    QTabWidget* tab = new QTabWidget(this);
    tab->addTab(morpho->getMenu(),tr("Morpho"));
    tab->addTab(brush->getMenu(),tr("Brush"));
    tab->addTab(regionGrowing->getMenu(),tr("Region Growing"));
    tab->addTab(marchingCubes->getMenu(),tr("Mesh Tools"));
    connect(tab, SIGNAL(currentChanged(int)),this, SLOT(ToolChanged(int)));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tab);
    layout->addStretch();

    groupBox->setLayout(layout);
    return groupBox;
}

void MainWindow::ToolChanged(int index)
{
    switch(index)
    {
    case 0:
        mprview->setViewMode(GraphView::Morpho);
        break;
    case 1:
        mprview->setViewMode(GraphView::Brush);
        break;
    case 2:
        mprview->setViewMode(GraphView::RegionGrowing);
        break;
    case 3:
        mprview->setViewMode(GraphView::MarchingCubes);
        break;
    default:
        mprview->setViewMode(GraphView::Navigation);
        break;
    }


}



//action.push_back(ACTION(49, "S+left button -- Select label on screen",cimg::keyS,M0,LEFT));
//action.push_back(ACTION(4, "Control+S -- Save labels",cimg::keyS,CTRL));
//action.push_back(ACTION(48,"Control+Z -- Cancel addition/removal",cimg::keyZ,CTRL));
//action.push_back(ACTION(12,""                                       ,cimg::keyPADSUB));
//action.push_back(ACTION(50, "Space -- Clear selection",cimg::keySPACE));


void MainWindow::load()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Image",QString(),
                                                    "Images (*.mhd *.dcm *.hdr)");
    if (fileName.isEmpty()) return;

    if(img.loadImage(fileName.toStdString().c_str(),false))
    {
        view->reinit();
        mprview->reinit();
        labelTable->updateTable();
        changeStatus(tr("Opened '%1'").arg(fileName));
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot open '%1'").arg(fileName));
}

void MainWindow::loadSegmentation()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Segmentation",QString(),
                                                    "Images (*.mhd *.dcm *.hdr *.raw)");
    if (fileName.isEmpty()) return;

    if(img.loadLabel(fileName.toStdString().c_str()))
    {
        mprview->Render(true);
        changeStatus(tr("Opened '%1'").arg(fileName));
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot open '%1'").arg(fileName));

    // try to load names
    std::string nameFile(fileName.toStdString());
    nameFile.replace(nameFile.begin()+nameFile.rfind('.'),nameFile.end(),"_names.txt");
    img.loadNames(nameFile.c_str());
}


void MainWindow::saveSegmentation()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Segmentation"), QString(),
                                                     "Images (*.mhd *.hdr *.raw)");

    if (fileName.isEmpty()) return;

    if(img.saveLabel(fileName.toStdString().c_str()))
    {
        std::string nameFile(fileName.toStdString());
        nameFile.replace(nameFile.begin()+nameFile.rfind('.'),nameFile.end(),"_names.txt");
        img.saveNames(nameFile.c_str());
        changeStatus(tr("Saved '%1' and '%2'").arg(fileName).arg(nameFile.c_str()));
    }
    else  QMessageBox::warning(this, tr("QtSegmentation"), tr("Cannot save '%1'").arg(fileName));

}


void MainWindow::changeStatus(const QString m)
{
    statusBar()->showMessage(m, 0);
}


void MainWindow::about()
{
    QMessageBox::information(this, tr("About QtSegmentation"),  tr("v1.0\nBenjamin GILLES"));
}
