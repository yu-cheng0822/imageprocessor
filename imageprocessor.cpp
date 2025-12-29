#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include "imagetransform.h"
#include "qstatusbar.h"

ZoomLabel::ZoomLabel(QWidget *parent) // 建構子
    : QLabel(parent) // 初始化標籤並指定父物件
    , rubberBand(new QRubberBand(QRubberBand::Rectangle, this)) // 建立橡皮筋用來顯示框選
    , sourceImg(nullptr) // 初始沒有來源影像
{
    setMouseTracking(true); // 啟用滑鼠追蹤以便即時顯示框選
}

void ZoomLabel::setSourceImage(QImage *img) // 設定來源影像
{
    sourceImg = img; // 保存來源影像指標以便換算座標
}

void ZoomLabel::mousePressEvent(QMouseEvent *event) // 監聽滑鼠按下
{
    if (event->button() != Qt::LeftButton || !sourceImg) // 只有有影像且左鍵才開始
        return; // 沒符合條件就離開
    origin = event->pos(); // 記錄框選起點
    rubberBand->setGeometry(QRect(origin, QSize())); // 初始化橡皮筋大小
    rubberBand->show(); // 顯示橡皮筋
}

void ZoomLabel::mouseMoveEvent(QMouseEvent *event) // 監聽滑鼠移動
{
    if (!rubberBand->isVisible()) // 只有在框選時才更新
        return; // 沒在框選就不處理
    QRect rect(origin, event->pos()); // 建立起點到目前點的矩形
    rubberBand->setGeometry(rect.normalized()); // 更新橡皮筋範圍並正規化
}

void ZoomLabel::mouseReleaseEvent(QMouseEvent *event) // 監聽滑鼠放開
{
    if (!rubberBand->isVisible() || event->button() != Qt::LeftButton) // 只有框選且左鍵放開才觸發
        return; // 不符合條件就離開
    rubberBand->hide(); // 隱藏橡皮筋
    if (!sourceImg || sourceImg->isNull()) // 沒有有效影像就不處理
        return; // 沒影像直接離開
    QRect viewRect = QRect(origin, event->pos()).normalized(); // 取得在標籤上的選取範圍
    double scaleX = static_cast<double>(sourceImg->width()) / width(); // 計算寬度縮放比例
    double scaleY = static_cast<double>(sourceImg->height()) / height(); // 計算高度縮放比例
    QRect mapped(QPoint(viewRect.x() * scaleX, viewRect.y() * scaleY),
                 QSize(viewRect.width() * scaleX, viewRect.height() * scaleY)); // 將座標換算到原圖
    QRect bounded = mapped.intersected(sourceImg->rect()); // 限制在原圖範圍內
    if (bounded.width() > 0 && bounded.height() > 0) // 確認選取範圍有效
        emit selectionReady(bounded); // 發出選取完成訊號
}

PaintLabel::PaintLabel(QWidget *parent) // 建構子
    : QLabel(parent) // 初始化標籤
    , painting(false) // 畫筆預設關閉
{
    setScaledContents(true); // 讓圖可隨標籤大小縮放
}

void PaintLabel::setImage(const QImage &image) // 指定顯示與編輯的影像
{
    workingImage = image; // 儲存可編輯影像
    setPixmap(QPixmap::fromImage(workingImage)); // 將影像顯示在標籤上
}

QImage PaintLabel::currentImage() const // 回傳目前影像
{
    return workingImage; // 回傳目前影像
}

void PaintLabel::mousePressEvent(QMouseEvent *event) // 監聽滑鼠按下開始畫筆
{
    if (event->button() != Qt::LeftButton || workingImage.isNull()) // 必須有影像且用左鍵
        return; // 不符合條件就離開
    painting = true; // 開啟畫筆
    lastPoint = event->pos(); // 記錄起始點
}

void PaintLabel::mouseMoveEvent(QMouseEvent *event) // 監聽滑鼠拖曳繪圖
{
    if (!painting || workingImage.isNull()) // 只有畫筆啟動才處理
        return; // 沒畫筆就離開
    double scaleX = static_cast<double>(workingImage.width()) / width(); // 計算寬度比例
    double scaleY = static_cast<double>(workingImage.height()) / height(); // 計算高度比例
    QPoint start(lastPoint.x() * scaleX, lastPoint.y() * scaleY); // 將上一個點換算到原圖
    QPoint end(event->pos().x() * scaleX, event->pos().y() * scaleY); // 將目前點換算到原圖
    QPainter painter(&workingImage); // 建立畫家在影像上作畫
    painter.setPen(QPen(Qt::red, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)); // 設定紅色粗線條
    painter.drawLine(start, end); // 連續畫線
    setPixmap(QPixmap::fromImage(workingImage)); // 更新標籤顯示
    lastPoint = event->pos(); // 更新上一點
}

void PaintLabel::mouseReleaseEvent(QMouseEvent *event) // 監聽滑鼠放開
{
    if (event->button() != Qt::LeftButton) // 只處理左鍵放開
        return; // 其他按鍵不處理
    painting = false; // 關閉畫筆
}

ZoomWindow::ZoomWindow(QWidget *parent) // 建構子
    : QWidget(parent) // 初始化視窗
    , paintArea(new PaintLabel(this)) // 建立可塗改的顯示區
    , saveButton(new QPushButton(QStringLiteral("另存新檔"), this)) // 建立存檔按鈕
    , layout(new QVBoxLayout(this)) // 建立垂直版面
{
    layout->addWidget(paintArea); // 把顯示區放到版面上
    layout->addWidget(saveButton); // 把存檔按鈕放到版面上
    setLayout(layout); // 套用版面配置
    connect(saveButton, &QPushButton::clicked, this, &ZoomWindow::saveImage); // 連結按鈕觸發存檔
}

void ZoomWindow::setZoomedImage(const QImage &image) // 放入放大後影像
{
    displayedImage = image; // 儲存放大後影像
    paintArea->setImage(displayedImage); // 在畫布上顯示並可塗改
}

void ZoomWindow::saveImage() // 執行存檔
{
    QImage toSave = paintArea->currentImage(); // 取得目前被修改的影像
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("另存放大圖片"),
                                                    "",
                                                    tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)")); // 取得儲存路徑
    if (fileName.isEmpty()) // 使用者取消就離開
        return; // 不儲存
    toSave.save(fileName); // 將影像存檔
}

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
    imgWin = new ZoomLabel(); // 建立可框選的影像標籤
     imgWin->setMouseTracking (true); // 標籤開啟滑鼠追蹤
    QPixmap *initPixmap = new QPixmap(300,200);

    initPixmap->fill (QColor(255,255,255));
    gWin =new imagetransform();
    imgWin->resize (300,200); // 設定預設大小
    imgWin->setScaledContents (true); // 允許影像隨標籤縮放
    imgWin->setPixmap (*initPixmap); // 顯示空白初始化圖片
    imgWin->setSourceImage(&img); // 把來源影像指標交給標籤
    mainLayout->addWidget(imgWin);
    setCentralWidget (central); // 套用中央元件
    createActions(); // 建立動作
    createMenus(); // 建立選單
    createToolBars(); // 建立工具列
    connect(imgWin, &ZoomLabel::selectionReady, this, &imageprocessor::showZoomedSelection); // 接收框選完成訊號
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
void imageprocessor::showZoomedSelection(const QRect &rect) // 顯示框選放大的結果
{
    if (img.isNull()) // 沒有影像就不處理
        return; // 直接離開
    bool ok = false; // 用來接收輸入狀態
    double factor = QInputDialog::getDouble(this,
                                            tr("放大倍率"),
                                            tr("輸入放大倍率"),
                                            2.0,
                                            0.5,
                                            10.0,
                                            1,
                                            &ok); // 讓使用者輸入放大倍率
    if (!ok) // 使用者取消輸入
        return; // 不繼續處理
    QRect bounded = rect.intersected(img.rect()); // 確保框選在圖片範圍內
    if (bounded.width() <= 0 || bounded.height() <= 0) // 若沒有有效範圍
        return; // 不進行放大
    QImage cropped = img.copy(bounded); // 依框選擷取子圖
    QImage zoomed = cropped.scaled(cropped.width() * factor, cropped.height() * factor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 依倍率放大並使用平滑插值
    ZoomWindow *window = new ZoomWindow(this); // 建立新視窗顯示放大結果並指定父物件
    window->setAttribute(Qt::WA_DeleteOnClose); // 關閉時自動釋放
    window->setZoomedImage(zoomed); // 將放大影像放入新視窗
    window->setWindowTitle(tr("放大視窗")); // 設定新視窗標題
    window->show(); // 顯示新視窗
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
