#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QMouseEvent>
#include <QStatusBar>
#include <QRubberBand>
#include <QInputDialog>
#include "imagetransform.h"
#include "zoomwindow.h"

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
    QLabel *imgWin;
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
    
    // 區域選取相關變數
    QRubberBand *rubberBand;    // 橡皮筋選取框
    QPoint selectionOrigin;     // 選取起始點
    bool isSelecting;           // 是否正在選取
    double currentZoomFactor;   // 當前放大倍率
};

#endif // IMAGEPROCESSOR_H
