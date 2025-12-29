#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("測試滑鼠事件"));

    statusLabel = new QLabel(this);
    statusLabel->setText(QStringLiteral("指標位置"));
    statusLabel->setFixedWidth(100);

    MousePosLabel = new QLabel(this);
    MousePosLabel->setText(QString());
    MousePosLabel->setFixedWidth(100);

    statusBar()->addPermanentWidget(statusLabel);
    statusBar()->addPermanentWidget(MousePosLabel);

    setMouseTracking(true);
    resize(400, 300);
}

MainWindow::~MainWindow() {}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QString str = "(" + QString::number(event->x()) + "," +
                  QString::number(event->y()) + ")";
    MousePosLabel->setText(str);


}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QString str = "(" + QString::number (event->x()) + "," +
                  QString::number (event->y()) +")";
    if (event->button() == Qt::LeftButton)
    {
        statusBar ()->showMessage (QStringLiteral("左鍵:")+str);
    }
    else if (event->button()== Qt:: RightButton)
    {
        statusBar ()->showMessage (QStringLiteral("右鍵:")+str);
    }
    else if (event->button()== Qt::MiddleButton)
    {
        statusBar ()->showMessage (QStringLiteral("中鍵:")+str);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QString str = "(" + QString::number (event->x()) + "," +
                  QString::number (event->y()) +")";
    statusBar ()->showMessage (QStringLiteral("釋放:")+str);
}
