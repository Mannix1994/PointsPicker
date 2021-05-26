#ifndef PICTUREBOX_H
#define PICTUREBOX_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QPoint>
#include <QLine>
#include <QVector>
#include <QStack>
#include <QStringList>
#include <QDebug>

class PictureBox : public QWidget
{
    Q_OBJECT
public:
    explicit PictureBox(QWidget *parent = nullptr);
    void setPenColor(QColor color); //设置画笔颜色
    void scaleImage(int scale=1);        //设置缩放因子
    int drawRectangleFlag = 0;
    int drawLineFlag = 0;
    int getPointPosFlag = 0;
    int linePointNum = 0;
    int releaseMouse = 1;
    QPoint startPoint, endPoint;
    ~PictureBox();

private:
    typedef enum {Border,Tracking}PaintMode;
    PaintMode m_paintMode;
    QPixmap m_pixmap;       //当前绘制的图像
    QPixmap m_raw_pixmap;   //原图像
    QString m_image_path;
    int m_scale;            //缩放因子
    QBrush m_brush;         //
    QColor m_penColor;      //坐标，追踪十字的颜色
    int x,y;                //当前被鼠标点击的点的x,y坐标
    QVector<QPoint> m_points;   //存储历次被点击的坐标
    QPixmap drawBorder(QPixmap m_pixmap);
    QPixmap drawLocationCross(QVector<QPoint>& m_points, int scale);
    QPixmap drawTrackingCross(QPixmap m_pixmap,int x,int y);

    QPixmap drawRect(QPoint startPoint, QPoint endPoint);
    QPixmap drawLine(QPoint startPoint, QPoint endPoint);
protected:
    void paintEvent(QPaintEvent * event); //绘制事件

signals:
//    void clicked(QString points);         //图片被点击的信号，发送的是被点击的点的坐标
    void clicked(int x,int y);              //控件当前被点击的点
    void clicked(QVector<QPoint> points);   //被点击的所有点
    void mouseMoved(int x,int y);           //当前鼠标的实时位置

public slots:
    bool setImage(QString imagePath);  //设置要显示的图像
    void setBackground(QBrush brush);           //设置背景
    void updateTrackingCross(int value);

protected:
    void mousePressEvent(QMouseEvent *event); //鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event); //鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event); //鼠标进入窗口事件
    void leaveEvent(QEvent *event); //鼠标离开窗口事件
    void keyPressEvent(QKeyEvent *event); //键盘点击事件
};

#endif // PICTUREBOX_H
