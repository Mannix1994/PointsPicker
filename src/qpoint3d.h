#ifndef QPOINT3D_H
#define QPOINT3D_H
#include <QObject>
class QPoint3d : public QObject
{
    Q_OBJECT
private:
    int m_x;
    int m_y;
    int m_z;
public:
    QPoint3d(const QPoint3d& other);
    QPoint3d();
    QPoint3d(int xpos, int ypos, int zpos);
    ~QPoint3d();
    int x();
    int y();
    int z();
};

Q_DECLARE_METATYPE(QPoint3d)
#endif // QPOINT3D_H
