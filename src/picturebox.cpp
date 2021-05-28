#include "picturebox.h"
#include <QPainter>
#include <QDebug>
#include <QCursor>
static const int IMAGE_WIDTH = 1;
static const int IMAGE_HEIGHT = 1;
static const QSize IMAGE_SIZE = QSize(IMAGE_WIDTH, IMAGE_HEIGHT);

PictureBox::PictureBox(QWidget *parent) : QWidget(parent),m_scale(0),x(0),y(0)
{
    m_pixmap = QPixmap(IMAGE_SIZE);
    m_pixmap.fill();
    m_scale = 1;
    m_brush = QBrush(Qt::white);
    m_paintMode = Border;
}

/**
 * @brief PictureBox::setBackground 设置背景
 * @param brush
 */
void PictureBox::setBackground(QBrush brush)
{
    m_brush = brush;
    update();
}

/**
 * @brief PictureBox::updateTrackingCross 更新追踪十字位置
 * @param value
 */
void PictureBox::updateTrackingCross(int value)
{
    Q_UNUSED(value);
    QPoint global = QCursor::pos();             //获取全局坐标
    QPoint local = this->mapFromGlobal(global); //转换为问控件的坐标

    //刷新位置
    x = local.x();
    y = local.y();
    m_paintMode = Tracking;
    update();
    emit mouseMoved(x/m_scale,y/m_scale); //send position
}

/**
 * @brief PictureBox::setPenColor 设置追踪十字的颜色
 * @param color 颜色
 */
void PictureBox::setPenColor(QColor color)
{
    this->m_penColor = color;
    //更新
    this->update();
}

void PictureBox::scaleImage(int scale)
{
    m_scale = scale;
    m_pixmap = drawLocationCross(m_points,m_scale);
    m_paintMode = Tracking;
    update();
}

/**
 * @brief PictureBox::setImage 设置图片
 * @param image 图片
 * @param scale 缩放因子
 * @return
 */
bool PictureBox::setImage(QString imagePath)
{
    m_points.clear();
    m_image_path = imagePath;
    if(m_pixmap.load(imagePath)){
        scaleImage();
        return true;
    }else{
        return false;
    }
}

/**
 * @brief PictureBox::paintEvent 调用update()以后引发的paintEvent事件
 * @param event
 */
void PictureBox::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(m_penColor);
    painter.setBackground(m_brush);

    painter.eraseRect(rect());
    painter.drawPixmap(0, 0, m_pixmap);
    switch (m_paintMode) {
    case Border:
    {
        QVector<QLine> lines;
        int xMax = painter.window().width()-1;
        int yMax = painter.window().height()-1;
        QPoint p1(0,0),p2(xMax,0),p3(xMax,yMax),p4(0,yMax);
        QLine line1(p1,p2);
        QLine line2(p2,p3);
        QLine line3(p3,p4);
        QLine line4(p4,p1);
        lines.append(line1);
        lines.append(line2);
        lines.append(line3);
        lines.append(line4);
        painter.drawLines(lines);
        break;
    }
    case Tracking:
    {
        QVector<QLine> lines;
        int xMax = painter.window().width()-1;
        int yMax = painter.window().height()-1;
        QPoint p1(x,0),p2(x,yMax),p3(0,y),p4(xMax,y);
        QLine line1(p1,p2);
        QLine line2(p3,p4);
        lines.append(line1);
        lines.append(line2);
        painter.drawLines(lines);
        break;
    }
    }
}

/**
 * @brief PictureBox::mousePressEvent 鼠标点击，左键在图片上添加一个定位十字，右键在图片上删除一个定位十字
 * @param event
 */
void PictureBox::mousePressEvent(QMouseEvent *event){
    if(m_pixmap.isNull()){
        return;
    }
    this->setMouseTracking(false); //关闭鼠标追踪
    x = event->x();
    y = event->y();

   if(event->button() == Qt::LeftButton && drawLineFlag){ // 绘制直线
        if (linePointNum == 0) {
            linePointNum++;
            startPoint = event->pos();
        }
        else if (linePointNum == 1) {
            linePointNum++;
            endPoint = event->pos();

            m_pixmap = drawLine(startPoint, endPoint);
            m_paintMode = Tracking;
            this->update();

            linePointNum = 0;
            drawLineFlag = 0;
        }
   }
   else if(event->button() == Qt::LeftButton && drawRectangleFlag){ // 绘制矩形
        releaseMouse = 0;
        startPoint = event->pos();
        endPoint = event->pos();
        //qDebug("press:(%d, %d), (%d, %d)", startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y());
        return;
   }
   else if(event->button() == Qt::LeftButton && getPointPosFlag){ // 获取点坐标
        //存储当被点击的点
        m_points.append(QPoint(x/m_scale,y/m_scale));
        //绘制定位十字
        m_pixmap = drawLocationCross(m_points,m_scale);
        //绘制追踪十字
        m_paintMode = Tracking;
        this->update();
        getPointPosFlag = 0;
    }
    else if(event->button() == Qt::RightButton){ //右键
       bool bRet = false;
       //单文本输入框
       QString text = QInputDialog::getText(this, "输入真实坐标",
                       "x,y", QLineEdit::Normal, "", &bRet);
       QStringList list = text.split(",");
       if (bRet && !text.isEmpty())
       {
           qDebug() << "press ok text = " << list[0].toInt() << "," << list[1].toInt() << text;
           real_points.append(QPoint(list[0].toInt(), list[1].toInt()));
       }
       else
       {
           qDebug() << "press Cancel text = " << text;
       }
        //m_pixmap = drawLocationCross(m_points,m_scale);
        //m_paintMode = Tracking;
        //this->update();
    }
    else if(event->button() ==Qt::MiddleButton){
        this->activateWindow(); //中键激活窗口
    }
    this->setMouseTracking(true);
    emit clicked(m_points); //发送被点击的信号
    emit clicked(x/m_scale,y/m_scale);
}

/**
 * @brief PictureBox::mouseMoveEvent 鼠标移动事件，绘制追踪十字
 * @param event
 */
void PictureBox::mouseMoveEvent(QMouseEvent *event){
    if(m_pixmap.isNull()){
        return;
    }
    if (drawRectangleFlag && !releaseMouse) {
        endPoint = event->pos();
        update();
        return;
    }
    x = event->x();
    y = event->y();
    //绘制定位十字
    m_paintMode = Tracking;
    this->update();
    emit mouseMoved(x/m_scale,y/m_scale); //send position
}
void PictureBox::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    if (drawRectangleFlag) {
        m_pixmap = drawRect(startPoint, endPoint);
        m_paintMode = Tracking;
        this->update();
    }
    drawRectangleFlag = 0;
    releaseMouse = 1;
    //qDebug("release:(%d, %d), (%d, %d)", startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y());
}

/**
 * @brief PictureBox::enterEvent 进入此控件时设置鼠标追踪，通过mouseMoveEvent绘制追踪十字
 * @param event
 */
void PictureBox::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    this->setMouseTracking(true);
    this->setFocus(); //方向键的监听需要调用此函数
    //this->activateWindow();
}

/**
 * @brief PictureBox::leaveEvent 离开此控件时关闭追踪，并将绘制模式paintMode设置边框(Border)，清除会绘制的追踪十字
 * @param event
 */
void PictureBox::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    this->setMouseTracking(false);
    if(!m_pixmap.isNull()){
        m_paintMode = Border;
        this->update();
    }
}

QPixmap PictureBox::drawLocationCross(QVector<QPoint> &m_points,int scale)
{
    //载入
    QPixmap p;
    if (!m_pixmap) {
        p = QPixmap(m_image_path);
        if(p.isNull())
            return QPixmap();
    } else {
        p = m_pixmap;
    }

    p  = p.scaled(p.size()*scale,Qt::KeepAspectRatio);
    //缩放
    QPainter painter(&p);//创建一个画笔
    painter.setPen(m_penColor);
    painter.setBackground(m_brush);
    QVector<QLine> lines;
    int xMax = painter.window().width()-1;
    int yMax = painter.window().height()-1;
    for(int i=0;i<m_points.size();i++){
        QPoint p = m_points[i];
        int x = p.x()*scale;
        int y = p.y()*scale;
        //this->update();
        int xMinP = (x-10)>0 ? (x-10):0;
        int xMaxP = (x+10)<xMax ? (x+10):xMax;
        int yMinP = (y-10)>0 ? (y-10):0;
        int yMaxP = (y+10)<yMax ? (y+10):yMax;
        QPoint p1(x,yMinP),p2(x,yMaxP),p3(xMinP,y),p4(xMaxP,y);
        QLine line1(p1,p2);
        QLine line2(p3,p4);
        lines.append(line1);
        lines.append(line2);
        painter.drawLines(lines);
    }
    return p;
}
QPixmap PictureBox::drawRect(QPoint startPoint, QPoint endPoint)
{
    QPainter painter(&m_pixmap);//创建一个画笔
    painter.setPen(m_penColor);
    painter.setBackground(m_brush);
    painter.drawRect(startPoint.x(), startPoint.y(), endPoint.x() - startPoint.x(), endPoint.y() - startPoint.y());

    painter.drawLine(startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y());
    painter.drawLine(endPoint.x(), startPoint.y(), startPoint.x(), endPoint.y());
    return m_pixmap;
}

QPixmap PictureBox::drawLine(QPoint startPoint, QPoint endPoint)
{
    QPainter painter(&m_pixmap);//创建一个画笔
    painter.setPen(m_penColor);
    painter.setBackground(m_brush);
    painter.drawLine(startPoint.x(), startPoint.y(), endPoint.x(), endPoint.y());
    return m_pixmap;
}

/**
 * @brief PictureBox::keyPressEvent 键盘点击事件，通过方向键来控制追踪十字的位置，按回车键是添加该点
 * @param event
 */
void PictureBox::keyPressEvent(QKeyEvent *event){
    this->setMouseTracking(false);
    int h = this->height();
    int w = this->width();
    switch(event->key()){
    case Qt::Key_W:
        y = (y-1)>0 ? (y-1):0;
        break;
    case Qt::Key_S:
        y = (y+1)<h ? (y+1):h;
        break;
    case Qt::Key_A:
        x = (x-1)>0 ? (x-1):0;
        break;
    case Qt::Key_D:
        x = (x+1)<w ? (x+1):w;
        break;
    case Qt::Key_Return:
    {
        //存储当被点击的点
        m_points.append(QPoint(x/m_scale,y/m_scale));
        //绘制定位十字
        m_pixmap = drawLocationCross(m_points,m_scale);
        //绘制追踪十字
//        m_paintMode = Tracking;
//        this->update();
        emit clicked(m_points); //发送被点击之后点序列
        emit clicked(x/m_scale,y/m_scale);
        break;
    }
    default:
        break;
    }
    m_paintMode = Tracking;
    this->update();
    event->accept();
    QWidget::keyPressEvent(event);
    emit mouseMoved(x/m_scale,y/m_scale);
    this->setMouseTracking(true);
}

PictureBox::~PictureBox()
{
}



QPixmap PictureBox::drawTrackingCross(QPixmap m_pixmap, int x, int y)
{
    QPainter painter(&m_pixmap);//创建一个画笔
    painter.setPen(m_penColor);
    painter.setBackground(m_brush);
    QVector<QLine> lines;
    int xMax = painter.window().width()-1;
    int yMax = painter.window().height()-1;
    QPoint p1(x,0),p2(x,yMax),p3(0,y),p4(xMax,y);
    QLine line1(p1,p2);
    QLine line2(p3,p4);
    lines.append(line1);
    lines.append(line2);
    painter.drawLines(lines);
    return m_pixmap;
}

QPixmap PictureBox::drawBorder(QPixmap m_pixmap)
{
    QPainter painter(&m_pixmap);//创建一个画笔
    painter.setPen(m_penColor);
    painter.setBackground(m_brush);
    QVector<QLine> lines;
    int xMax = painter.window().width()-1;
    int yMax = painter.window().height()-1;
    QPoint p1(0,0),p2(xMax,0),p3(xMax,yMax),p4(0,yMax);
    QLine line1(p1,p2);
    QLine line2(p2,p3);
    QLine line3(p3,p4);
    QLine line4(p4,p1);
    lines.append(line1);
    lines.append(line2);
    lines.append(line3);
    lines.append(line4);
    painter.drawLines(lines);
    return m_pixmap;
}
