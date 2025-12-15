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

#include "board.h"
#include <vector>
#include <iostream>

/* Construct a board for a game of size n.
 *
 * @param n The number of disks to get in a row to win.
 */
Board::Board(int n) : 
  board_(n + 3, std::vector<int>(n + 2, 0)),
  n_{n}
  {}

/* Turn a vector of vectors of ints into a Board.
 *
 * @param board The vector of vectors of ints to turn into a Board.
 *              Assumed to be well-formed.
 */
Board::Board(const std::vector<std::vector<int>> &board) :
  board_{board},
  n_{static_cast<int>(board.size()) - 3}
  {}

/* Construct a Board from a string where rows are separated by sep.
 *
 * @param board The input string. Assumed to be well-formed.
 * @param sep The separator.
 */
Board::Board(const std::string &board, char sep)
{
  // Calculate n
  int cols{0};
  for (const char ch : board) {
    if (ch == sep) {
      break;
    }
    cols++;
  }
  n_ = cols - 2;
  // Resize according to n; fill with 0's
  board_.resize(n_ + 3);
  for (auto &row : board_) {
    row.resize(n_ + 2, 0);
  }
  // Overwite positions with player numbers
  int row{0}, col{0};
  for (const char &ch : board) {
    if (ch == sep) {
      row++; col = 0;
    } else {
      int d{ch - '0'};
      board_[row][col] = d;
      col++;
    }
  }
}


/* Change a position to player identity p.
 *
 * @param pos The {row, column} to change.
 * @param p The player identity to change it to.
 */
void Board::set_pos(std::pair<int,int> pos, int p)
{
  if (pos.first < 0 || pos.second < 0) { return; }
  if (pos.first < get_rows() && pos.second < get_cols()) {
    board_[pos.first][pos.second] = p;
  }
}

/* Clear the row; by doing so, entries above it "fall" by one row and a
 * blank row is inserted at the top.
 *
 * Given this Board state:
 *
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 2 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 2 2 1 0
 *
 * ClearRow(0) results in
 *
 *  0 0 0 0 0 0
 *  0 1 0 0 0 0
 *  0 2 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *
 * @param row The row to clear.
 */
void Board::ClearRow(int row)
{
  // Erase row, insert blank one at front of vector
  board_.erase(board_.begin() + row);
  board_.insert(board_.begin(), std::vector<int>(get_cols(), 0));
}

/* Clear the player entries in a column.
 *
 * Given this Board state:
 *
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 2 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 2 2 1 0
 *
 *  ClearCol(1) results in
 *
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 2 2 1 0
 *
 * @param col The column to clear.
 */
void Board::ClearCol(int col)
{
  for (size_t row = 0; row < get_rows(); row++) {
    board_[row][col] = 0;
  }
}

/* Set all positions to 0.
 */
void Board::ClearBoard()
{
    for (size_t row = 0; row < get_rows(); row++) {
        for (size_t col = 0; col < get_cols(); col++) {
            board_[row][col] = 0;
        }
    }
}

/* Return the highest empty row in the given column.
 *
 * @param col The column to search.
 * @return The highest empty row; if column has no empty rows, then -1.
 */
int Board::GetHighestEmptyRow(int col)
{
    int highest_empty_row = -1;
    for (size_t row = 0; row < get_rows(); row++) {
        if (board_[row][col] == 0) {
                highest_empty_row = static_cast<int>(row);
        }
    }
    return highest_empty_row;
}

/* Check if current board state has a winner and return the identity of the winner if so.
 *
 * Note that the method cannot determine if both players are in winning states. It only returns
 * the first winner it finds.
 *
 * @return 0 if no winner; otherwise 1 or 2.
 */
int Board::CheckWin() const 
{
  int winner{};
  if ((winner = CheckRowWin()))
    return winner;
  if ((winner = CheckColWin()))
    return winner;
  if ((winner = CheckRightDiagonalWin()))
    return winner;
  if ((winner = CheckLeftDiagonalWin()))
    return winner;
  return 0;
}

 /* @return The identity of the player (1 or 2) who has n_ in a row; 0 if no players have.
 */
int Board::CheckRowWin() const 
{
  int run{1}, who{0};
  for (size_t row = 0; row < get_rows(); row++) {
    for (size_t col = 0; col < get_cols(); col++) {
      if (board_[row][col] == who) {
        run++;
        if (run == n_ && who != 0) {
          return who;
        }
      } else {
          run = 1; who = board_[row][col];
        }
    }
    run = 1; who = 0;
  }
  return 0;
}

/* @return the identity of the player (1 or 2) who has n_ in a row in a column;
 *         0 if no players have.
 */
int Board::CheckColWin() const
{
  int run{1}, who{0};
  for (size_t col = 0; col < get_cols(); col++) {
    for (size_t row = 0; row < get_rows(); row++) {
      if (board_[row][col] == who) {
        run++;
        if (run == n_ && who != 0) {
          return who;
        }
      } else {
          run = 1; who = board_[row][col];
        }
    }
    run = 1; who = 0;
  }
  return 0;
}

/* @return the identity of the player who has n_ in a row diagonally, from upper left
 *         to lower right.
 *
 *  Since rows/columns increase by 1 when n_ increases by 1, there are only six diagonals
 *  to check, regardless of n_.
 *
 *  For instance, for n_ = 4:
 *
 *  x x x 0 0 0
 *  x x x x 0 0
 *  x x x x x 0
 *  x x x x x x
 *  0 x x x x x
 *  0 0 x x x x
 *  0 0 0 x x x
 *
 *  Increasing n_ by one adds a row and a column; but since we need n_ in a row to achieve a win,
 *  the six starting locations along the top row/left column remain the same.
 */
int Board::CheckRightDiagonalWin() const { 
  int run{1}, who{0};
  // Only 6 possible starting positions
  std::vector<std::pair<int,int>> start_pos{ {0,0}, {0,1}, {0,2}, {1,0}, {2,0}, {3,0} };
  for (const auto& pos : start_pos) {
    int row{pos.first}, col{pos.second};
    while (row < get_rows() && col < get_cols()) {
      if (board_[row][col] == who) {
        run++;
        if (run == n_ && who != 0) {
          return who;
        }
      } else {
        run = 1; who = board_[row][col];
      }
      row++, col++;
    }
    run = 1; who = 0;
  }
  return 0;
}

/* @return the identity of the player who has n_ in a row diagonally, from lower left
 *         to upper right.
 *
 *  Since rows/columns increase by 1 when n_ increases by 1, there are only six diagonals
 *  to check, regardless of n_.
 *
 *  For instance, for n_ = 4:
 *
 *  0 0 0 x x x
 *  0 0 x x x x
 *  0 x x x x x
 *  x x x x x x
 *  x x x x x 0
 *  x x x x 0 0
 *  x x x 0 0 0
 *
 *  Increasing n_ by one adds a row and a column; but since we need n_ in a row to achieve a win,
 *  the six starting locations along the top row/left column remain the same.
 */
int Board::CheckLeftDiagonalWin() const { 
  int run{1}, who{0};
  // Only 6 possible starting positions
  std::vector<std::pair<int,int>> start_pos{ {0, get_cols() - 1}, {0, get_cols() - 2}, {0, get_cols() - 3},
                                             {1, get_cols() - 1}, {2, get_cols() - 1}, {3, get_cols() - 1} };
  for (const auto& pos : start_pos) {
    int row{pos.first}, col{pos.second};
    while (row < get_rows() && col >= 0) {
      if (board_[row][col] == who) {
        run++;
        if (run == n_ && who != 0) {
          return who;
        }
      } else {
        run = 1; who = board_[row][col];
      }
      row++, col--;
    }
    run = 1, who = 0;
  }
  return 0;
}

/* A draw is defined to occur when there are no more empty positions.
 *
 * @return true when there are no empty positions; false otherwise.
 */
bool Board::CheckDraw() const
{
  for (size_t row = 0; row < get_rows(); row++) {
    for (size_t col = 0; col < get_cols(); col++) {
      if (board_[row][col] == 0) {
        return false;
      }
    }
  }
  return true;
}

/* Returns a string representation of the board, with each row separated by sep.
 */
std::string Board::Stringify(char sep) const
{
  std::string output{};
  for (size_t row = 0; row < get_rows(); row++) {
    for (size_t col = 0; col < get_cols(); col++) {
      output += std::to_string(board_[row][col]);
    }
    output += sep;
  }
  return output;
}

/* Outputs a representation of the board state in the following format:
 *
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 0 0 0 0 0
 *  0 1 0 0 0 0
 *  0 1 2 2 1 0
 *
 */
std::ostream& operator<<(std::ostream &os, const Board &board)
{
  for (size_t row = 0; row < board.get_rows(); row++) {
    for (size_t col = 0; col < board.get_cols(); col++) {
      os << board.get_board()[row][col] << " ";
    }
    os << '\n';
  }
  return os;
}
