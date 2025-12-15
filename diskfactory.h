#ifndef DISKFACTORY_H
#define DISKFACTORY_H

#include <vector>
#include <QString>
#include <QColor>
#include <QObject>

class Disk;
class Board;

class DiskFactory
{
public:
    // static std::vector<Disk*> CreateDisks(const QString &board, const int n, const int h,
    //                                        const std::vector<QColor> &colors);
    static std::vector<std::vector<Disk*>> CreateDisks(const Board &board, const int n, const int h,
                                           const std::vector<QColor> &colors, QObject *parent = nullptr);
    static Disk* CreateDisk(int row, int col, const int n, const int h, const QColor &color, int p, QObject *parent = nullptr);
private:
    DiskFactory();
};

#endif // DISKFACTORY_H
