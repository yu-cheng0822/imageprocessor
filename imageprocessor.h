#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QRubberBand> // 使用橡皮筋元件來顯示框選範圍
#include <QInputDialog> // 提供輸入放大倍率的對話框
#include <QPushButton> // 讓新視窗可以有按鈕
#include <QVBoxLayout> // 建立簡單的垂直版面配置
#include <QFileDialog> // 讓新視窗可以另存圖片
#include <QPainter> // 在放大圖上畫筆塗改
#include <QMouseEvent>
#include <QStatusBar>
#include "imagetransform.h"

class ZoomLabel : public QLabel // 可框選區域的影像顯示標籤
{
    Q_OBJECT // 啟用 Qt 的訊號與槽機制
public:
    explicit ZoomLabel(QWidget *parent = nullptr); // 建構子設定父物件
    void setSourceImage(QImage *img); // 指定來源影像以便座標轉換
signals:
    void selectionReady(const QRect &rect); // 框選完成後送出選取範圍
protected:
    void mousePressEvent(QMouseEvent *event) override; // 監聽滑鼠按下開始框選
    void mouseMoveEvent(QMouseEvent *event) override; // 監聽滑鼠移動更新框選範圍
    void mouseReleaseEvent(QMouseEvent *event) override; // 監聽滑鼠放開完成框選
private:
    QRubberBand *rubberBand; // 負責顯示框選矩形的橡皮筋
    QPoint origin; // 記錄框選起點
    QImage *sourceImg; // 指向原始影像以便換算座標
};

class PaintLabel : public QLabel // 可在圖片上繪圖的標籤
{
    Q_OBJECT // 啟用 Qt 訊號槽
public:
    explicit PaintLabel(QWidget *parent = nullptr); // 建構子
    void setImage(const QImage &image); // 設定要繪圖的影像
    QImage currentImage() const; // 取出目前被修改的影像
protected:
    void mousePressEvent(QMouseEvent *event) override; // 開始畫筆繪製
    void mouseMoveEvent(QMouseEvent *event) override; // 拖曳時連續畫線
    void mouseReleaseEvent(QMouseEvent *event) override; // 放開後停止畫筆
private:
    QImage workingImage; // 儲存可被塗改的影像
    QPoint lastPoint; // 記錄上一個繪圖點
    bool painting; // 記錄畫筆是否啟動
};

class ZoomWindow : public QWidget // 放大結果與塗改的新視窗
{
    Q_OBJECT // 啟用 Qt 訊號槽
public:
    explicit ZoomWindow(QWidget *parent = nullptr); // 建構子
    void setZoomedImage(const QImage &image); // 將放大後影像放入視窗
private slots:
    void saveImage(); // 另存新檔
private:
    PaintLabel *paintArea; // 顯示並可塗改的影像區
    QPushButton *saveButton; // 觸發存檔的按鈕
    QVBoxLayout *layout; // 簡單的垂直版面配置
    QImage displayedImage; // 儲存目前顯示的影像
};

class imageprocessor : public QMainWindow
{
    Q_OBJECT

public:
    explicit imageprocessor(QWidget *parent = nullptr);
    ~imageprocessor();

private slots:
    void showopenfile();  // 對應 SLOT(showopenfile())
    void big1();          // 對應 SLOT(big1())
    void small1();        // 對應 SLOT(small1())
    void showGeometryTransform();
    void showZoomedSelection(const QRect &rect); // 顯示框選放大的結果


protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    void createActions();
    void createMenus();
    void createToolBars();
    void loadFile(QString filename);

    imagetransform *gWin;
    QWidget *central;
    ZoomLabel *imgWin; // 可框選的影像標籤
    QImage img;
    QString filename;

    // Menu / Action / Toolbar
    QMenu *fileMenu;
    QToolBar *fileTool;

    QAction *openFileAction;
    QAction *exitAction;
    QAction *bigFileAction;
    QAction *sAction;
    QAction *geometryAction;
    QLabel *statusLabel;
    QLabel *MousePosLabel;
};

#endif // IMAGEPROCESSOR_H
