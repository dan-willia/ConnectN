/* Board.h
 *
 * The ConnectN board is represented as a 2D vector of ints:
 * n_ is a parameter that determines the size of the board:
 *      (n + 3) vectors of (n + 2) ints.
 *
 * Each position in the board is either 0, 1, or 2.
 *  0 represents an empty position.
 *  1 and 2 represent player 1 and player 2's disks.
 *
 * For n_ = 4, we could have the following board state:
 *
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 2 2 1 0
 *
 * When we refer to rows and columns, notes that
 *  row 0 starts at the top.
 *  column 0 starts at the left.
 *
*/

#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>

class Board
{
  public:
    Board(int n);
    Board(const std::vector<std::vector<int>> &board);
    Board(const std::string &board, char sep = '|');

    const std::vector<std::vector<int>>& get_board() const { return board_; }
    void set_pos(std::pair<int,int> pos, int p);
    int get_n() const { return n_; }
    size_t get_rows() const { return board_.size(); }
    size_t get_cols() const { return board_[0].size(); }

    void ClearBoard();
    void ClearRow(int row);
    void ClearCol(int col);
    int GetHighestEmptyRow(int col);

    int CheckWin() const;
    int CheckRowWin() const;
    int CheckColWin() const;
    int CheckRightDiagonalWin() const;
    int CheckLeftDiagonalWin() const;
    bool CheckDraw() const;

    std::string Stringify(char sep = '|') const;
    friend std::ostream& operator<<(std::ostream &os, const Board &board);

  private:
    std::vector<std::vector<int>> board_;
    int n_;
};

#endif // BOARD_H
