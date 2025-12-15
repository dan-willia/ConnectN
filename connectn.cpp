#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "board.h"
#include "connectn.h"
#include "player.h"

/* Set the static member. Saved and updated to persistent memory after initialization in my Qt application.
 */
int ConnectN::global_uid_ = 0;

/* Create a new ConnectN instance, including required Board and Player instances, for starting a new game.
 *
 * @param n The number of disks needed in a row to win; also determines the size of the board (n + 3) by (n + 2).
 * @param p1_name Player 1's name.
 * @param p2_name Player 2's name.
 */
ConnectN::ConnectN(int n, const std::string &p1_name, const std::string &p2_name) :
    uid_{global_uid_++},
    name_{std::to_string(uid_)},
    p1_{new Player{1, p1_name}},
    p2_{new Player{2, p2_name}},
    board_{new Board{n}},
    score_{0,0,0},
    round_{1},
    n_{n},
    whose_turn_{1},
    timestamp_{time(nullptr)}
{}

/* Create an instance from a previous game state. Recommend using the ConnectNFactory when loading a game state
 * from a csv.
 *
 * @param uid Game uid.
 * @param name Game name.
 * @param p1 Player 1.
 * @param p2 Player 2.
 * @param board Board.
 * @param score score, where score[0] is number of draws, score[1] is player 1's score, and score[2] is player 2's score.
 * @param round What round it is.
 * @param n n_ parameter.
 * @param whose_turn Whose turn it is (1 or 2).
 */
ConnectN::ConnectN(int uid, const std::string &name, 
        const Player &p1, const Player &p2,
        const Board &board, 
        std::vector<int> score, int round, int n, int whose_turn)
        :
        uid_{uid}, name_{name},
        p1_{new Player{p1}}, p2_{new Player{p2}},
        board_{new Board{board.get_board()}},
        score_{score}, round_{round}, n_{n}, whose_turn_{whose_turn},
        timestamp_{time(nullptr)}
        {}


// Delete dynamically allocated members
ConnectN::~ConnectN()
{
    delete board_;
    delete p1_;
    delete p2_;
}

/* Set the global uid.
 *
 * @param global_uid The new global uid value.
 */
void ConnectN::set_global_uid(int global_uid)
{
    global_uid_ = global_uid;
}

/* @return the Player whose turn it is.
 */
Player* ConnectN::get_whose_turn() const
{
    return whose_turn_ == 1 ? p1_ : p2_;
}

/* @return the name of the Player whose turn it is.
 */
std::string ConnectN::WhoseTurnName() const
{
    if (whose_turn_ == 1) 
        return p1_->get_name();
    else 
        return p2_->get_name();
}

/* Toggles whose_turn_ between 1 and 2.
 */
void ConnectN::NextTurn()
{
    whose_turn_ == 1 ? whose_turn_ = 2 : whose_turn_ = 1;
}

/* Advance game state to a new round:
 *  - Reset the board to 0's
 *  - Increment the round counter
 *  - Calculate who goes first in the new round
 *  - Resets the clear row/column abilities for both players.
 */
void ConnectN::NextRound()
{
    // Reset board to 0's
    board_->ClearBoard();
    round_++;
    // If round is odd, p1 goes first
    round_ % 2 == 1 ? whose_turn_ = 1 : whose_turn_ = 2;
    // Reset row/col abilities
    p1_->can_clear_row(true);
    p2_->can_clear_row(true);
    p1_->can_clear_col(true);
    p2_->can_clear_col(true);
}

/* Convert game to a string, with data members separated by commas.
 * String is in same order as args expected for ConnectNFactory
 */
std::string ConnectN::Stringify()
{
    timestamp_ = time(nullptr);
    std::string output{};
    output += std::to_string(uid_) + ',';
    output += name_ + ',';
    output += p1_->Stringify() + ',';
    output += p2_->Stringify() + ',';
    output += board_->Stringify() + ',';
    for (int s : score_)
        output += std::to_string(s) + ',';
    output += std::to_string(round_) + ',';
    output += std::to_string(n_) + ',';
    output += std::to_string(whose_turn_) + ',';
    output += ctime(&timestamp_);
    return output;
}
