#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include "imagetransform.h"
#include "qstatusbar.h"

imageprocessor::imageprocessor(QWidget *parent)
    : QMainWindow(parent)
{
    statusLabel = new QLabel(this);
    statusLabel->setText(QStringLiteral("指標位置"));
    statusLabel->setFixedWidth(100);

    MousePosLabel = new QLabel(this);
    MousePosLabel->setText(QString());
    MousePosLabel->setFixedWidth(100);

    statusBar()->addPermanentWidget(statusLabel);
    statusBar()->addPermanentWidget(MousePosLabel);


    setMouseTracking(true);
    setWindowTitle (QStringLiteral("影像處理"));
    central =new QWidget();
    central->setMouseTracking (true);
    QHBoxLayout *mainLayout = new QHBoxLayout (central);
    imgWin = new QLabel();
     imgWin->setMouseTracking (true);
    QPixmap *initPixmap = new QPixmap(300,200);

    initPixmap->fill (QColor(255,255,255));
    gWin =new imagetransform();
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

    geometryAction = new QAction(QStringLiteral("幾何轉換"),this);
    geometryAction->setShortcut (tr("Ctrl+G"));
    geometryAction->setStatusTip (QStringLiteral("影像幾何轉換"));
    connect (geometryAction, SIGNAL (triggered()), this, SLOT (showGeometryTransform()));
    connect (exitAction, SIGNAL (triggered()),gWin, SLOT (close()));

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
    fileMenu->addAction(geometryAction);
    fileMenu->addAction (sAction);
}
void imageprocessor::createToolBars ()
{
    fileTool = addToolBar("file");
    fileTool->addAction (openFileAction);
    fileTool = addToolBar("file");
    fileTool->addAction (bigFileAction);
    fileTool->addAction (sAction);
    fileTool->addAction(geometryAction);
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
void imageprocessor::showGeometryTransform()
{
    if (!img.isNull())
        gWin->srcImg = img;
    gWin->inWin->setPixmap (QPixmap:: fromImage (gWin->srcImg));
    gWin->show();
}
void imageprocessor::mouseMoveEvent (QMouseEvent *event)
{
    int x=event->x();
    int y=event->y();
    QString str = "("+ QString::number(x) + "," +
                  QString::number (y) +")";
    if (!img.isNull() && x >= 0 && x < img.width() && y >= 0 && y < img.height())
    {
        QColor color = img.pixelColor(x, y);
        int grayValue = (color.red() + color.green() + color.blue()) / 3;
        str += " = " + QString::number(grayValue);
    }
    MousePosLabel->setText(str);
}
void imageprocessor::mousePressEvent (QMouseEvent *event)
{
    QString str = "("+ QString::number (event->x()) + "," +
                  QString::number (event->y()) +")";
    if (event->button() == Qt::LeftButton)
    {
        statusBar()->showMessage (QStringLiteral("左鍵:")+str);
    }
    else if (event->button()== Qt::RightButton)
    {
        statusBar()->showMessage (QStringLiteral("右鍵:")+str);
    }
    else if (event->button() == Qt::MiddleButton)
    {
        statusBar()->showMessage (QStringLiteral("中鍵:")+str);
    }
}

void imageprocessor::mouseReleaseEvent (QMouseEvent *event)
{
    QString str = "(" + QString::number (event->x()) + "," +
                  QString::number (event->y()) +")";
    statusBar ()->showMessage (QStringLiteral("釋放:")+str);
}
