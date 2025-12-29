#ifndef ZOOMWINDOW_H
#define ZOOMWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QSpinBox>
#include <QColorDialog>

// 放大視窗類別 - 用於顯示放大的影像區域並提供繪圖功能
class ZoomWindow : public QWidget
{
    Q_OBJECT

public:
    // 建構子：接收原始影像和選取區域
    explicit ZoomWindow(const QImage &sourceImage, const QRect &selectedRegion, double zoomFactor = 2.0, QWidget *parent = nullptr);
    ~ZoomWindow();

protected:
    // 滑鼠事件處理 - 用於繪圖功能
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    // 另存新檔
    void saveImage();
    // 選擇畫筆顏色
    void choosePenColor();
    // 清除繪圖
    void clearDrawing();
    // 改變畫筆粗細
    void changePenWidth(int width);

private:
    // UI元件
    QImage zoomedImage;        // 放大後的影像
    QImage drawingLayer;       // 繪圖圖層
    QPushButton *saveButton;   // 另存新檔按鈕
    QPushButton *colorButton;  // 選擇顏色按鈕
    QPushButton *clearButton;  // 清除繪圖按鈕
    QSpinBox *penWidthSpinBox; // 畫筆粗細選擇器
    QVBoxLayout *mainLayout;   // 主要版面配置
    QHBoxLayout *toolLayout;   // 工具列版面配置
    
    // 繪圖相關變數
    bool isDrawing;            // 是否正在繪圖
    QPoint lastPoint;          // 上一個繪圖點
    QColor penColor;           // 畫筆顏色
    int penWidth;              // 畫筆粗細
    
    // 常數定義
    static constexpr int TOOLBAR_HEIGHT = 60;  // 工具列高度
    
    // 初始化UI
    void setupUI();
};

#endif // ZOOMWINDOW_H
