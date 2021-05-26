#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QClipboard>
#include <QApplication>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QResizeEvent>
#include "picturebox.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    PictureBox *pictureBox;
    bool showScrollBar;
    QString imagePath;
    QVector<QPoint> _points;
    QStringList _images;
    int _imageCurIndex = 0;
    int firstOpenDir= 1;
    void initialize();
    static QString getPointsString(QVector<QPoint> &points, bool parenthesis);
private slots:
    void setImage(QImage image);
    void setScaledImage(QImage image,int scale);
    void onScaleChanged(int scale);
    void onPointUpdated(int x,int y);
    void onPointsChanged(QVector<QPoint> points);
    void setSizes(QSize imageSize);
    void on_pbChoosePic_clicked();
    void on_pbChooseColor_clicked();

    void on_pbCopy_clicked();

    void on_cbParenthesis_stateChanged(int arg1);

    void on_pbNextPic_clicked();
    void on_pbDrawRectangle_clicked();

    void on_pbDrawLine_clicked();

protected:
//    void closeEvent(QCloseEvent *event);
//    void enterEvent(QEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // WIDGET_H
