#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>

class imageprocessor : public QMainWindow
{
    Q_OBJECT

public:
    imageprocessor(QWidget *parent = nullptr);
    ~imageprocessor();
};
#endif // IMAGEPROCESSOR_H
