#include "imagetransform.h"
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
imagetransform::imagetransform(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QHBoxLayout(this);
    leftLayout = new QVBoxLayout(this);
    mirrorGroup = new QGroupBox(QStringLiteral("鏡射"),this);
    groupLayout = new QVBoxLayout(mirrorGroup);
    hCheckBox = new QCheckBox (QStringLiteral("水平"), mirrorGroup);
    vCheckBox = new QCheckBox (QStringLiteral("垂直"), mirrorGroup);
    mirrorButton = new QPushButton (QStringLiteral("執行"), mirrorGroup);
    saveButton  = new QPushButton(QStringLiteral("存檔"), this);
    hCheckBox->setGeometry (QRect (13, 28, 87, 19));
    vCheckBox->setGeometry (QRect (13, 54, 87, 19));
    mirrorButton->setGeometry (QRect (13, 80, 93, 28));
    saveButton->setGeometry (QRect (13, 240, 93, 28));

    groupLayout->addWidget (hCheckBox);
    groupLayout->addWidget (vCheckBox);
    groupLayout->addWidget (mirrorButton);
    leftLayout->addWidget (mirrorGroup);
    rotateDial = new QDial (this);
    rotateDial->setNotchesVisible(true);
    rotateDial->setRange(0,360);
    rotateDial->setWrapping(true);
    vSpacer = new QSpacerItem (20, 58, QSizePolicy:: Minimum,
                                  QSizePolicy:: Expanding);
    leftLayout->addWidget (rotateDial);
    leftLayout->addItem(vSpacer);
    mainLayout->addLayout (leftLayout);
    inWin = new QLabel(this);
    inWin->setScaledContents(true);
    QPixmap *initPixmap= new QPixmap (300,200);
    initPixmap->fill(QColor (255,255,255));
    inWin->setPixmap (*initPixmap);
    //--------------------------------
    QPainter *paint= new QPainter(initPixmap);
    paint->setPen(*(new QColor(0,0,0)));
    paint->begin(initPixmap);
    paint->drawRect(15,15,60,40);
    paint->end();
    //--------------------------
    inWin->setSizePolicy (QSizePolicy:: Expanding, QSizePolicy:: Expanding);
    if (srcImg.isNull())
    {
        srcImg = initPixmap->toImage();
    }
    /*    if (srcImg.isNull())
    {
        QPixmap *initPixmap= new QPixmap(300,200);
        initPixmap->fill (QColor(255,255,255));
        inWin->setPixmap (*initPixmap);
    }*/
    inWin->setPixmap(*initPixmap);
    mainLayout->addWidget(inWin);
    connect (mirrorButton, SIGNAL(clicked()),this, SLOT (mirroredImage()));
    connect (rotateDial, SIGNAL (valueChanged (int)), this, SLOT (rotatedImage()));
    connect (saveButton, SIGNAL (clicked()), this, SLOT (saveImage()));
}

imagetransform::~imagetransform() {}
void imagetransform::mirroredImage(){
    bool H,V;
    if (srcImg.isNull())
        return;
    H=hCheckBox->isChecked();
    V=vCheckBox->isChecked();
    dstImg = srcImg.mirrored (H,V);
    inWin->setPixmap (QPixmap:: fromImage (dstImg));
    srcImg = dstImg;
}
void imagetransform::rotatedImage(){
    QTransform tran;
    int angle;
    if (srcImg.isNull())
        return;
    angle=rotateDial->value();
    tran.rotate (angle);
    dstImg=srcImg.transformed (tran);
    inWin->setPixmap (QPixmap:: fromImage(dstImg));

}
void imagetransform::saveImage()
{

    if (srcImg.isNull() && dstImg.isNull())
        return;

    QImage imgToSave;
    if (!dstImg.isNull())
        imgToSave = dstImg;
    else
        imgToSave = srcImg;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("存檔圖片"),
        "",
        tr("PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;BMP Files (*.bmp);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    imgToSave.save(fileName);
}
