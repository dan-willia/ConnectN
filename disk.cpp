/* Disk width defines the boundingRect . The disk itself is drawn starting at (padding, padding)
 * relative to the boundingRect so that Disks can be placed contiguously across a scene.
 */
#include <QObject>
#include <QGraphicsItem>
#include <QColor>
#include <QtWidgets>
#include "disk.h"

Disk::Disk(qreal x, qreal y, int row, int col, qreal width, QColor color, int player, QObject *parent)
    : QObject(parent),
      QGraphicsItem(),
      row_{row}, col_{col}, width_{width}, padding_{10}, color_{color}, player_{player}
{
    setPos(x, y);
}

QRectF Disk::boundingRect() const
{
    return QRectF(0, 0, width_, width_);
}

QPainterPath Disk::shape() const
{
    QPainterPath path;
    path.addEllipse(padding_/2,padding_/2,width_ - padding_,width_ - padding_);
    return path;
}

void Disk::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(item);

    QBrush b = painter->brush();
    painter->setBrush(QBrush(color_));

    painter->drawEllipse(QRect(padding_/2,padding_/2,width_-padding_,width_-padding_));
    painter->setBrush(b);
}

Disk* Disk::Clone(int player, const QColor &color) const
{
    return new Disk{x(), y(), row_, col_, width_, color, player};
}

void Disk::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "emitting mouse press signal for disk at " << row_ << ", " << col_;
    emit DiskSelected(this);
    // emit ConvertDisk(this);
}
