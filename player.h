#ifndef PLAYER_H
#define PLAYER_H

#include <string>
class Board;
class ConnectN;

class Player
{
  public:
    // Default
    Player();

    // New game initialization
    Player(int num, const std::string &name);

    // Saved game initialization
    Player(int num, const std::string &name, bool can_clear_row, bool can_clear_col, bool can_convert);

    Player(const Player& other);
    Player& operator=(const Player& other);

    int get_num() const { return num_; }
    std::string get_name() const { return name_; }
    bool can_clear_row() const { return can_clear_row_; }
    bool can_clear_col() const { return can_clear_col_; }
    bool can_convert() const { return can_convert_; }
    void can_clear_row(bool b) { can_clear_row_ = b; }
    void can_clear_col(bool b) { can_clear_col_ = b; }
    void can_convert(bool b) { can_convert_ = b; }

    void ConvertDisk(Board *board, std::pair<int,int> pos);
    void ClearRow(Board *board, int row);
    void ClearCol(Board *board, int col);

    void ForfeitRound(ConnectN* game);

    std::string Stringify() const;

  private:
    int num_; // 1 or p2
    std::string name_;
    bool can_clear_row_;
    bool can_clear_col_;
    bool can_convert_;
};

#endif // PLAYER_H
