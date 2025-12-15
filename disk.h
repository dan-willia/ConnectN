#ifndef DISK_H
#define DISK_H

#include <QObject>
#include <QGraphicsItem>
#include <QColor>

class Disk : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    Q_PROPERTY(qreal x READ x WRITE set_x)
    Q_PROPERTY(qreal y READ y WRITE set_y)
    Q_PROPERTY(QColor color_ READ get_color WRITE set_color)
public:
    // explicit Disk(QObject *parent = nullptr);
    explicit Disk(qreal x, qreal y, int row, int col, qreal width, QColor color, int player, QObject *parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    // Create a new disk identical to this one, except with different player and color
    Disk* Clone(int player, const QColor &color) const;

    // Getters
    qreal x() const { return QGraphicsItem::x(); }
    qreal y() const { return QGraphicsItem::y(); }
    int get_row() const { return row_; }
    int get_col() const { return col_; }
    qreal get_width() const { return width_; }
    QColor get_color() const { return color_; }
    int get_player() const { return player_; }

    // Setters
    void set_x(qreal x) { setPos(x, y()); }
    void set_y(qreal y) { setPos(x(), y); }
    void set_width(qreal width) { width_ = width; }
    void set_color(QColor color) { color_ = color; update(); } // need update() call?
    void set_player(int player) { player_ = player; }

signals:
    void DiskSelected(Disk *d);
    // void ConvertDisk(Disk *d);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int row_;
    int col_;
    qreal width_;     // width of boundingRect
    int padding_;   // space between boundingRect and drawn circle
    QColor color_;
    int player_;    // 0, 1 or 2; 0 indicates an "empty" disk, i.e.

    // friend bool operator==(const Disk& disk, const Disk& other);
};

#endif // DISK_H
