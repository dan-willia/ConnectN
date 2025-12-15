#include "diskfactory.h"
#include "disk.h"
#include <QColor>
#include <QObject>
#include <QString>
#include <vector>
#include <string>
#include "board.h"

DiskFactory::DiskFactory() {}

/* Create a vector of disk pointers from the board representation found in a csv game file.
 *
 * @param board String representation of board that represents row of ConnectN as X's, 1's, 2's separated by '|'.
 *              X means no disk at that location, 1 means player 1's disk, 2 means player 2's disk.
 * @param n The number of disks in a row required to win.
 * @param h The height of the scene (for calculating y coordinate).
 * @param colors Three-element vector of colors to use for disks: {empty color, p1 color, p2 color}
 * @return A vector of disk pointers.
 */
// std::vector<Disk*> DiskFactory::CreateDisks(const QString &board, const int n, const int h, const std::vector<QColor> &colors)
// {
//     std::vector<Disk*> disks{};
//     QStringList rows = board.split('|');
//     const qreal w = h / (n+3);
//     qreal x = 0;
//     qreal y = 0;
//     for (int j=0; j<rows.length(); j++) { // j tracks rows
//         QString row = rows[j];
//         for (int i=0; i<row.length(); i++) { // i tracks columns
//             if (row[i] == '1') {
//                 disks.push_back(new Disk{x, y, j, i, w, colors[1], 1});
//             }
//             else if (row[i] == '2') {
//                 disks.push_back(new Disk{x, y, j, i, w, colors[2], 2});
//             }
//             else {
//                 disks.push_back(new Disk{x, y, j, i, w, colors[0], 0});
//             }
//             x += w; // advance x coord across scene
//         }
//         x = 0;
//         y += w; // next row's y coord
//     }
//     return disks;
// }

std::vector<std::vector<Disk*>> DiskFactory::CreateDisks(const Board &board, const int n, const int h, const std::vector<QColor> &colors, QObject *parent)
{
    std::vector<std::vector<Disk*>> disks(board.get_rows(),
                                           std::vector<Disk*>(board.get_cols(), nullptr));
    const qreal w = h / (n+3);
    qreal x = 0;
    qreal y = 0;
    for (size_t row=0; row<board.get_rows(); row++) {
        for (size_t col=0; col<board.get_cols(); col++) {
            if (board.get_board()[row][col] == '1') {
                disks[row][col] = new Disk{x, y, static_cast<int>(row), static_cast<int>(col), w, colors[1], 1, parent};
            } else if (board.get_board()[row][col] == '2') {
                disks[row][col] = new Disk{x, y, static_cast<int>(row), static_cast<int>(col), w, colors[2], 2, parent};
            } else {
                disks[row][col] = new Disk{x, y, static_cast<int>(row), static_cast<int>(col), w, colors[0], 0, parent};
            }
            x += w; // advance x coord across scene
        }
        x = 0;
        y += w; // next row's y coord
    }
    return disks;
}

Disk* DiskFactory::CreateDisk(int row, int col, const int n, const int h, const QColor &color, int p, QObject *parent)
{
    const qreal w = h / (n+3);
    qreal x = col * w;
    qreal y = row * w;
    return new Disk{x, y, row, col, w, color, p, parent};
}
