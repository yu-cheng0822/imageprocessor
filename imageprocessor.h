#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QWidget>

class imageprocessor : public QWidget
{
    Q_OBJECT

public:
    imageprocessor(QWidget *parent = nullptr);
    ~imageprocessor();
};
#endif // IMAGEPROCESSOR_H
