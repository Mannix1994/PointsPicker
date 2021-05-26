#include "widget.h"
#include "ui_widget.h"
#include <QScrollBar>
#include <QColorDialog>
#include <QStandardPaths>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),showScrollBar(false)
{
    ui->setupUi(this);
    this->setWindowTitle("x-y");
    this->setMinimumSize(1000,500);
    this->setWindowIcon(QIcon(":/icon/1/x-y.ico"));
    this->initialize();
}

Widget::~Widget()
{
    delete pictureBox;
    delete ui;
}

void Widget::initialize(){
    //
    ui->pbChoosePic->setFocus();
    ui->lePoints->setReadOnly(true);
    //图片控件
    pictureBox = new PictureBox();
    this->connect(pictureBox,SIGNAL(clicked(QVector<QPoint>)),this,SLOT(onPointsChanged(QVector<QPoint>)));
    this->connect(pictureBox,SIGNAL(mouseMoved(int,int)),this,SLOT(onPointUpdated(int,int)));
    this->connect(ui->scrollArea->verticalScrollBar(),SIGNAL(valueChanged(int)),
                  pictureBox,SLOT(updateTrackingCross(int)));
    this->connect(ui->scrollArea->horizontalScrollBar(),SIGNAL(valueChanged(int)),
                  pictureBox,SLOT(updateTrackingCross(int)));
    this->connect(ui->sbScale,SIGNAL(valueChanged(int)),this,SLOT(onScaleChanged(int)));

    //滚动区域
    this->ui->scrollArea->setWidget(pictureBox);
    pictureBox->setEnabled(false);

    this->move(0,0);

//    this->setMouseTracking(true);
}

/**
 * @brief Widget::setImage 设置图片和窗口大小
 * @param image
 */
void Widget::setImage(QImage image){
    this->setSizes(image.size());
    ui->sbScale->setValue(1);
    ui->lePoints->clear();
    //设置图片
    pictureBox->setImage(imagePath);
}

void Widget::setScaledImage(QImage image,int scale){
    image = image.scaled(image.size()*scale,Qt::KeepAspectRatio,
                         Qt::FastTransformation);
    this->setSizes(image.size());
    //设置图片
    pictureBox->scaleImage(scale);
}

void Widget::onScaleChanged(int scale)
{
    if(!imagePath.isEmpty()){
        this->setScaledImage(QImage(imagePath),scale);//设置图片
    }
}

/**
 * @brief Widget::onPointUpdated 接收来自PictureBox的坐标信息，并在主界面显示
 * @param x x
 * @param y y
 */
void Widget::onPointUpdated(int x, int y){
    ui->lbXy->setText(QString("位置:(x:%1, y:%2)").arg(x).arg(y));
}
/**
 * @brief Widget::onPointsStringChanged 接收PictureBox中发送过来的的坐标信息
 * @param s 坐标信息，以英文分号分割，demo:(1,1);(2,2)
 */
void Widget::onPointsChanged(QVector<QPoint> points){
    _points = points;
    QString s = getPointsString(points, ui->cbParenthesis->isChecked());
    ui->lePoints->setText(s);
}

QString Widget::getPointsString(QVector<QPoint> &points, bool parenthesis){
    if(points.isEmpty())
        return "";
    QString data;
    QString s;
    if (parenthesis)
        s = QString("(%1,%2)");
    else
        s = QString("%1,%2");
    for(int i=0;i<points.count();i++){
        auto p = points[i];
        auto temp = s.arg(p.x()).arg(p.y());
        data += ","+temp; //以逗号分割
    }
    data = data.mid(1);
    return data;
}

void Widget::setSizes(QSize imageSize)
{
    this->showNormal();

    //获取桌面大小
    QRect deskRect = QApplication::desktop()->availableGeometry();
    QSize deskSize = deskRect.size() - QSize(10,30);

    //设置最大值
    this->setMaximumSize(imageSize+QSize(150,80));
    ui->scrollArea->setMaximumSize(imageSize+QSize(2,2));

    //设置窗口大小
    int win_w = imageSize.width()+50>deskSize.width()?deskSize.width():imageSize.width()+50;
    int win_h = imageSize.height()+80>deskSize.height()?deskSize.height():imageSize.height()+80;
    this->resize(win_w,win_h);
    this->move(0,0);

    //设置scrollArea大小
    int sa_w = imageSize.width()+2;
    int sa_h = imageSize.height()+2;
    win_w = this->width();
    win_h = this->height();
    sa_w = sa_w>win_w-30?win_w-30:sa_w;
    sa_h = sa_h>win_h-75?win_h-75:sa_h;
    ui->scrollArea->resize(sa_w,sa_h);

    //设置PictureBox的大小
    pictureBox->setMinimumSize(imageSize);//必须设置此行，否则不能显示滚动条
    pictureBox->resize(imageSize);
    pictureBox->setEnabled(true);
}

/**
 * @brief Widget::keyPressEvent 键盘事件的响应
 * @param e e
 */
void Widget::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == Qt::ControlModifier){
        //如果是Ctrl键，禁用滑动区域滚动条
        ui->scrollArea->verticalScrollBar()->setEnabled(false);
        ui->scrollArea->horizontalScrollBar()->setEnabled(false);
    }else if(e->modifiers() == Qt::ShiftModifier){
        //如果是Shift键，按一次隐藏滑动条，再按一次显示滑动条
        if(showScrollBar){
            ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else{
            ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
        showScrollBar = !showScrollBar;
    }
    e->accept();
}
/**
 * @brief Widget::keyReleaseEvent 按键抬起时，启用滚动条
 * @param e e
 */
void Widget::keyReleaseEvent(QKeyEvent *e)
{
//
    ui->scrollArea->verticalScrollBar()->setEnabled(true);
    ui->scrollArea->horizontalScrollBar()->setEnabled(true);
    e->accept();
}

/**
 * @brief Widget::wheelEvent 鼠标滚轮事件
 * @param e e
 */
void Widget::wheelEvent(QWheelEvent *e)
{
    if(QApplication::keyboardModifiers () == Qt::ControlModifier){
        int where = ui->scrollArea->horizontalScrollBar()->value();
        if(e->delta() > 0){
            ui->scrollArea->horizontalScrollBar()->setValue(where-=30);
        }else{
            ui->scrollArea->horizontalScrollBar()->setValue(where+=30);
        }
        e->accept();
    }
}
/**
 * @brief Widget::resizeEvent 窗口size变化时，调整scrollArea的大小
 * @param e e
 */
void Widget::resizeEvent(QResizeEvent *e)
{
    ui->scrollArea->resize(e->size().width()-40,e->size().height()-70);
    e->accept();
}

/**
 * @brief Widget::on_pbChoosePic_clicked 从电脑中选取图片
 */
void Widget::on_pbChoosePic_clicked()
{
    // 获取主目录路径
    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    qDebug("%s\n", home.toStdString().data());
    QString filename = QFileDialog::getOpenFileName(this,
                           tr("打开图片"),home,tr("图片 (*.jpg *.png *.bmp *.jpeg *.webp)")); // 图片格式
    qDebug("%s\n", filename.toStdString().data());
    if(!filename.isEmpty()){
        imagePath = filename;
        this->setImage(QImage(filename));//设置图片
    }
}
/**
 * @brief Widget::on_pbChooseColor_clicked 选择颜色
 */
void Widget::on_pbChooseColor_clicked()
{
    QColorDialog color;
    color.setModal(true);
    color.exec();
    QColor c = color.selectedColor();
    if(c.isValid())
        pictureBox->setPenColor(c);//修改追踪十字的颜色
    qDebug()<<c.isValid();
}

void Widget::on_pbCopy_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();      //获取系统剪贴板指针
    clipboard->setText(ui->lePoints->text());
}

void Widget::on_cbParenthesis_stateChanged(int arg1)
{
    if (arg1 == Qt::Unchecked){
        QString s = getPointsString(_points, false);
        ui->lePoints->setText(s);
    }else if(arg1 == Qt::Checked){
        QString s = getPointsString(_points, true);
        ui->lePoints->setText(s);
    }
}

void Widget::on_pbNextPic_clicked()
{
    if (firstOpenDir) {
        firstOpenDir = 0;
        QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString dirname = QFileDialog::getExistingDirectory(this, "open a dir", home);
        if (!dirname.isNull() && !dirname.isEmpty()){
        QDir dir(dirname);
        _images = dir.entryList(QStringList() << "*.jpg" << "*.png", QDir::Files);
        _images.sort();
        if (!dirname.endsWith('/')) dirname+="/";
        QStringList tmp;
            for (auto &image: _images){
                tmp.push_back(dirname+image);
            }
            _images = tmp;
        }

    }
    if (_imageCurIndex < _images.size()) {
        QString filename = _images[_imageCurIndex++];
        imagePath = filename;
        this->setImage(QImage(filename));//设置图片
    } else {
        _imageCurIndex = 0;
        QString filename = _images[_imageCurIndex++];
        imagePath = filename;
        this->setImage(QImage(filename));//设置图片

    }
}


void Widget::on_pbDrawRectangle_clicked()
{
    pictureBox->drawRectangleFlag = 1;
}
