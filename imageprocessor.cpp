#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QInputDialog>
#include "imagetransform.h"
#include "qstatusbar.h"

imageprocessor::imageprocessor(QWidget *parent)
    : QMainWindow(parent)
    , rubberBand(nullptr)
    , isSelecting(false)
    , currentZoomFactor(2.0)  // 預設放大倍率為2倍
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
    // 清理橡皮筋選取框
    if (rubberBand)
        delete rubberBand;
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
    
    // 更新橡皮筋選取框
    if (isSelecting && rubberBand)
    {
        rubberBand->setGeometry(QRect(selectionOrigin, event->pos()).normalized());
    }
}
void imageprocessor::mousePressEvent (QMouseEvent *event)
{
    QString str = "("+ QString::number (event->x()) + "," +
                  QString::number (event->y()) +")";
    
    // 左鍵：開始區域選取（用於放大功能）
    if (event->button() == Qt::LeftButton)
    {
        statusBar()->showMessage (QStringLiteral("左鍵:")+str);
        
        // 如果有載入影像，啟動選取模式
        if (!img.isNull())
        {
            // 獲取imgWin在主視窗中的位置
            QPoint globalPos = imgWin->mapToGlobal(QPoint(0, 0));
            QPoint localPos = mapFromGlobal(globalPos);
            
            // 檢查點擊是否在imgWin範圍內
            QRect imgRect(localPos, imgWin->size());
            if (imgRect.contains(event->pos()))
            {
                isSelecting = true;
                selectionOrigin = event->pos();
                
                // 建立橡皮筋選取框
                if (!rubberBand)
                    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
                
                rubberBand->setGeometry(QRect(selectionOrigin, QSize()));
                rubberBand->show();
            }
        }
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
    
    // 完成區域選取並開啟放大視窗
    if (isSelecting && event->button() == Qt::LeftButton)
    {
        isSelecting = false;
        
        if (rubberBand && rubberBand->isVisible())
        {
            // 獲取選取區域
            QRect selectionRect = rubberBand->geometry();
            rubberBand->hide();
            
            // 獲取imgWin在主視窗中的位置
            QPoint globalPos = imgWin->mapToGlobal(QPoint(0, 0));
            QPoint localPos = mapFromGlobal(globalPos);
            
            // 將選取區域轉換為相對於imgWin的座標
            QRect imgRect(localPos, imgWin->size());
            selectionRect.translate(-localPos);
            
            // 確保選取區域在影像範圍內
            selectionRect = selectionRect.intersected(QRect(0, 0, imgWin->width(), imgWin->height()));
            
            // 如果選取區域有效（大於10x10像素）
            if (selectionRect.width() > 10 && selectionRect.height() > 10)
            {
                // 詢問使用者放大倍率
                bool ok;
                double zoomFactor = QInputDialog::getDouble(
                    this,
                    QStringLiteral("設定放大倍率"),
                    QStringLiteral("請輸入放大倍率 (1.0 - 10.0):"),
                    currentZoomFactor,  // 預設值
                    1.0,                // 最小值
                    10.0,               // 最大值
                    1,                  // 小數位數
                    &ok);
                
                if (ok)
                {
                    currentZoomFactor = zoomFactor;
                    
                    // 將選取區域從顯示座標轉換為實際影像座標
                    // 因為imgWin使用setScaledContents，需要根據縮放比例調整
                    double scaleX = static_cast<double>(img.width()) / imgWin->width();
                    double scaleY = static_cast<double>(img.height()) / imgWin->height();
                    
                    QRect imageRect(
                        static_cast<int>(selectionRect.x() * scaleX),
                        static_cast<int>(selectionRect.y() * scaleY),
                        static_cast<int>(selectionRect.width() * scaleX),
                        static_cast<int>(selectionRect.height() * scaleY)
                    );
                    
                    // 確保imageRect在影像範圍內
                    imageRect = imageRect.intersected(QRect(0, 0, img.width(), img.height()));
                    
                    // 建立並顯示放大視窗
                    ZoomWindow *zoomWin = new ZoomWindow(img, imageRect, zoomFactor);
                    zoomWin->setAttribute(Qt::WA_DeleteOnClose); // 關閉視窗時自動釋放記憶體
                    zoomWin->show();
                }
            }
        }
    }
}
