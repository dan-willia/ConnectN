#include "player.h"
#include "board.h"
#include "connectn.h"
#include <string>
#include <iostream>

Player::Player() :
    num_{0}, name_{"NoName"}, can_clear_row_{false}, can_clear_col_{false}, can_convert_{false}
{}

Player::Player(int num, const std::string &name) :
  num_{num}, name_{name}, can_clear_row_{true}, can_clear_col_{true}, can_convert_{true}
  {}

Player::Player(int num, const std::string& name, bool can_clear_row, bool can_clear_col, bool can_convert) :
  num_{num}, name_{name}, can_clear_row_{can_clear_row}, can_clear_col_{can_clear_col}, can_convert_{can_convert}
  {}

Player::Player(const Player &other)
    : num_{other.num_},
      name_{other.name_},
      can_clear_row_{other.can_clear_row_},
      can_clear_col_{other.can_clear_col_},
      can_convert_{other.can_convert_}
    {}

Player& Player::operator=(const Player& other)
{
    if (this == &other)
        return *this;

    num_ = other.get_num();
    name_ = other.get_name();
    can_clear_row_ = other.can_clear_row();
    can_clear_col_ = other.can_clear_col();
    can_convert_ = other.can_convert();

    return *this;
}

void Player::ConvertDisk(Board *board, std::pair<int,int> pos)
{
  if (!can_convert_) { return; }
  board->set_pos(pos, num_);
  can_convert_ = false;
}

void Player::ClearRow(Board *board, int row)
{
  if (!can_clear_row_) { return; }
  board->ClearRow(row);
  can_clear_row_ = false;
}

void Player::ClearCol(Board *board, int col)
{
  if (!can_clear_col_) { return; }
  board->ClearCol(col);
  can_clear_col_ = false;
}

void Player::ForfeitRound(ConnectN *game)
{
    // Increment other player's score and advance game to next round
    num_ == 1 ? game->IncrScore(2) : game->IncrScore(1);
    game->NextRound();
}

std::string Player::Stringify() const
{
  std::string output{};
  output += std::to_string(num_) + ",";
  output += name_ + ",";
  output += can_clear_row_ ? "1" : "0"; output += ",";
  output += can_clear_col_ ? "1" : "0"; output += ",";
  output += can_convert_ ? "1" : "0"; 
  return output;
}
