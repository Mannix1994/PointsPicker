#include "qpoint3d.h"
QPoint3d::QPoint3d(int xpos, int ypos, int zpos) {
    m_x = xpos;
    m_y = ypos;
    m_z = zpos;
}
QPoint3d::QPoint3d(const QPoint3d& other): QObject( nullptr ){
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = other.m_z;
}
QPoint3d::QPoint3d(){}
QPoint3d::~QPoint3d(){}

int QPoint3d::x(){
    return m_x;
}

int QPoint3d::y(){
    return m_y;
}

int QPoint3d::z(){
    return m_z;
}
