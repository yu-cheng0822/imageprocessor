#include "zoomwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>

// 建構子：初始化放大視窗
ZoomWindow::ZoomWindow(const QImage &sourceImage, const QRect &selectedRegion, double zoomFactor, QWidget *parent)
    : QWidget(parent)
    , isDrawing(false)
    , penColor(Qt::red)
    , penWidth(3)
{
    // 設定視窗標題
    setWindowTitle(QStringLiteral("放大視窗 - 可繪圖編輯"));
    
    // 從原始影像中裁剪選取的區域
    QImage croppedImage = sourceImage.copy(selectedRegion);
    
    // 根據放大倍率縮放影像
    int newWidth = static_cast<int>(croppedImage.width() * zoomFactor);
    int newHeight = static_cast<int>(croppedImage.height() * zoomFactor);
    zoomedImage = croppedImage.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 初始化繪圖圖層（透明背景）
    drawingLayer = QImage(zoomedImage.size(), QImage::Format_ARGB32);
    drawingLayer.fill(Qt::transparent);
    
    // 設定UI
    setupUI();
    
    // 啟用滑鼠追蹤以支援繪圖
    setMouseTracking(true);
    
    // 設定視窗大小
    resize(zoomedImage.width() + 40, zoomedImage.height() + 100);
}

ZoomWindow::~ZoomWindow()
{
}

// 初始化使用者介面
void ZoomWindow::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    
    // 建立工具列
    toolLayout = new QHBoxLayout();
    
    // 畫筆粗細選擇器
    QLabel *widthLabel = new QLabel(QStringLiteral("畫筆粗細:"), this);
    penWidthSpinBox = new QSpinBox(this);
    penWidthSpinBox->setRange(1, 20);
    penWidthSpinBox->setValue(penWidth);
    connect(penWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changePenWidth(int)));
    
    // 選擇顏色按鈕
    colorButton = new QPushButton(QStringLiteral("選擇顏色"), this);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(choosePenColor()));
    
    // 清除繪圖按鈕
    clearButton = new QPushButton(QStringLiteral("清除繪圖"), this);
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearDrawing()));
    
    // 另存新檔按鈕
    saveButton = new QPushButton(QStringLiteral("另存新檔"), this);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveImage()));
    
    // 將按鈕加入工具列
    toolLayout->addWidget(widthLabel);
    toolLayout->addWidget(penWidthSpinBox);
    toolLayout->addWidget(colorButton);
    toolLayout->addWidget(clearButton);
    toolLayout->addWidget(saveButton);
    toolLayout->addStretch();
    
    // 將工具列加入主版面配置
    mainLayout->addLayout(toolLayout);
    
    setLayout(mainLayout);
}

// 繪製事件：顯示放大影像和繪圖圖層
void ZoomWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);  // 標記未使用的參數
    QPainter painter(this);
    
    // 計算影像顯示位置（置中）
    int xOffset = (width() - zoomedImage.width()) / 2;
    int yOffset = TOOLBAR_HEIGHT; // 工具列下方
    
    // 繪製放大的影像
    painter.drawImage(xOffset, yOffset, zoomedImage);
    
    // 繪製繪圖圖層
    painter.drawImage(xOffset, yOffset, drawingLayer);
}

// 滑鼠按下事件：開始繪圖
void ZoomWindow::mousePressEvent(QMouseEvent *event)
{
    // 計算影像顯示位置
    int xOffset = (width() - zoomedImage.width()) / 2;
    int yOffset = TOOLBAR_HEIGHT;
    
    // 檢查是否在影像範圍內
    QPoint imagePos = event->pos() - QPoint(xOffset, yOffset);
    
    if (imagePos.x() >= 0 && imagePos.x() < zoomedImage.width() &&
        imagePos.y() >= 0 && imagePos.y() < zoomedImage.height())
    {
        isDrawing = true;
        lastPoint = imagePos;
    }
}

// 滑鼠移動事件：繼續繪圖
void ZoomWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!isDrawing)
        return;
    
    // 計算影像顯示位置
    int xOffset = (width() - zoomedImage.width()) / 2;
    int yOffset = TOOLBAR_HEIGHT;
    
    QPoint imagePos = event->pos() - QPoint(xOffset, yOffset);
    
    // 檢查是否在影像範圍內
    if (imagePos.x() >= 0 && imagePos.x() < zoomedImage.width() &&
        imagePos.y() >= 0 && imagePos.y() < zoomedImage.height())
    {
        // 在繪圖圖層上繪製線條
        QPainter painter(&drawingLayer);
        painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(lastPoint, imagePos);
        
        lastPoint = imagePos;
        
        // 觸發重繪
        update();
    }
}

// 滑鼠釋放事件：結束繪圖
void ZoomWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);  // 標記未使用的參數
    if (isDrawing)
    {
        isDrawing = false;
    }
}

// 另存新檔功能
void ZoomWindow::saveImage()
{
    // 合併放大影像和繪圖圖層
    QImage finalImage = zoomedImage.copy();
    QPainter painter(&finalImage);
    painter.drawImage(0, 0, drawingLayer);
    
    // 開啟檔案對話框
    QString fileName = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("另存影像"),
        "",
        tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;BMP Files (*.bmp);;All Files (*.*)"));
    
    if (fileName.isEmpty())
        return;
    
    // 儲存影像
    if (finalImage.save(fileName))
    {
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("影像已成功儲存！"));
    }
    else
    {
        QMessageBox::warning(this, QStringLiteral("錯誤"), QStringLiteral("影像儲存失敗！"));
    }
}

// 選擇畫筆顏色
void ZoomWindow::choosePenColor()
{
    QColor color = QColorDialog::getColor(penColor, this, QStringLiteral("選擇畫筆顏色"));
    
    if (color.isValid())
    {
        penColor = color;
    }
}

// 清除繪圖
void ZoomWindow::clearDrawing()
{
    // 清空繪圖圖層
    drawingLayer.fill(Qt::transparent);
    
    // 觸發重繪
    update();
}

// 改變畫筆粗細
void ZoomWindow::changePenWidth(int width)
{
    penWidth = width;
}
