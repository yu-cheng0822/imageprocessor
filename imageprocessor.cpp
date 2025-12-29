#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>

imageprocessor::imageprocessor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle (QStringLiteral("影像處理"));
    central =new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout (central);
    imgWin = new QLabel();
    QPixmap *initPixmap = new QPixmap(300,200);
    initPixmap->fill (QColor(255,255,255));
    imgWin->resize (300,200);
    imgWin->setScaledContents (true);
    imgWin->setPixmap (*initPixmap);
    mainLayout->addWidget(imgWin);
    setCentralWidget (central);
    createActions();
    createMenus();
    createToolBars();
}


imageprocessor::~imageprocessor()
{

}

void imageprocessor::createActions()
{
    openFileAction = new QAction (QStringLiteral("開啟檔案&O"),this);
    openFileAction->setShortcut (tr("Ctrl+o"));
    openFileAction->setStatusTip (QStringLiteral("開啟影像檔案"));
    connect (openFileAction, SIGNAL (triggered()), this, SLOT (showopenfile()));

    exitAction = new QAction (QStringLiteral("結束&Q"),this);
    exitAction->setShortcut (tr("Ctrl+o"));
    exitAction->setStatusTip (QStringLiteral("退出程式"));
    connect (exitAction, SIGNAL (triggered()), this, SLOT (close()));

    bigFileAction = new QAction (QStringLiteral("放大&+"),this);
    bigFileAction->setShortcut (tr("Ctrl+"));
    bigFileAction->setStatusTip (QStringLiteral("放大"));
    connect (bigFileAction, SIGNAL (triggered()), this, SLOT (big1()));

    sAction = new QAction (QStringLiteral("縮小&-"),this);
    sAction->setShortcut (tr("Ctrl-"));
    sAction->setStatusTip (QStringLiteral("縮小"));
    connect (sAction, SIGNAL (triggered()), this, SLOT (small1()));
}
void imageprocessor::createMenus()
{
    fileMenu = menuBar ()->addMenu (QStringLiteral ("檔案&F"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction (exitAction);
    fileMenu = menuBar ()->addMenu (QStringLiteral ("工具&T"));
    fileMenu->addAction(bigFileAction);
    fileMenu->addAction (sAction);
}
void imageprocessor::createToolBars ()
{
    fileTool = addToolBar("file");
    fileTool->addAction (openFileAction);
    fileTool = addToolBar("file");
    fileTool->addAction (bigFileAction);
    fileTool->addAction (sAction);
}
void imageprocessor::loadFile (QString filename)
{
    qDebug() <<QString("file name: %1").arg(filename);
    QByteArray ba=filename.toLatin1();
    printf("FN:%s\n", (char *) ba.data());
    img.load(filename);
    imgWin->setPixmap (QPixmap::fromImage(img));
}
void imageprocessor::showopenfile()
{
    filename = QFileDialog::getOpenFileName(this,
                                            QStringLiteral("開啟影像"),
                                            tr("."),
                                            "bmp(*.bmp);;png(*.png)"
                                            ";;Jpeg(*.jpg)");
    if (!filename.isEmpty())
    {
        if (img.isNull())
        {
            loadFile(filename);
        }
        else
        {
            imageprocessor *newIPWin
                = new imageprocessor();
            newIPWin->show();
            newIPWin->loadFile(filename);
        }
    }
}

void imageprocessor::big1()
{
    QImage big1;
    big1 =img.scaled(img.width()*2,img.width()*2);
    QLabel *ret=new QLabel();
    ret->setPixmap(QPixmap::fromImage(big1));
    ret->setWindowTitle(tr("放大結果"));
    ret->show();
}

void imageprocessor::small1()
{
    QImage small1;
    small1 =img.scaled(img.width()/2,img.width()/2);
    QLabel *ret=new QLabel();
    ret->setPixmap(QPixmap::fromImage(small1));
    ret->setWindowTitle(tr("縮小結果"));
    ret->show();
}
