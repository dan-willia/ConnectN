/* connectn.h
 *
 * ConnectN is a game wherein players must try to place "n" disks in a row (horizontally, vertically,
 * or diagonally) in order to win.
 *
 * A game requires two players and a board. The associated classes Player and Board implement these.
 *
 * A ConnectN instance can be constructed with a "new game" by passing in "n" and the player's names:
 *  ConnectN(4, "Player 1", "Player 2");
 * This will initialize a new game and play can begin with by using the member functions here and in
 * Board and Player.
 *
 * An instance can also be created from a previous game state using the associated constructor.
 *
 * See the ConnectNFactory class for creating a game from a csv, and the related Stringify method.
 */

#ifndef CONNECTN_H
#define CONNECTN_H

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "board.h"
#include "player.h"

class ConnectN
{
public:
    // Create new game
    ConnectN(int n, const std::string &p1_name, const std::string &p2_name);

    // Load from previous game state
    ConnectN(int uid, const std::string &name, const Player &p1, const Player &p2,
                const Board &board, std::vector<int> score,
                int round, int n, int whose_turn);

    ~ConnectN();

    int get_uid() const { return uid_; }
    std::string get_name() const { return name_; }
    Board* get_board() { return board_; }
    Player* get_p1() const { return p1_; }
    Player* get_p2() const { return p2_; }
    std::vector<int> get_score() const { return score_; }
    int get_round() const { return round_; }
    int get_n() const { return n_; }
    int whose_turn() const { return whose_turn_; }
    Player* get_whose_turn() const;
    std::string WhoseTurnName() const;
    time_t get_timestamp() const { return timestamp_; }

    static void set_global_uid(int global_uid);
    void set_name(std::string name) { name_ = name; }
    void IncrScore(int player) { score_[player]++; }
    void NextTurn();
    void NextRound();
    void set_timestamp(time_t ts) { timestamp_ = ts; }

    std::string Stringify();

private:
    static int global_uid_;
    int uid_;
    std::string name_;
    Player* p1_;
    Player* p2_;
    Board *board_;
    std::vector<int> score_;            // Draws, P1 score, P2 score
    int round_;                         // Current round
    int n_;                             // Number of disks in a row needed
    int whose_turn_;                    // Whose turn it is: 1 or 2
    time_t timestamp_;                  // Last saved or loaded time
};

#endif // CONNECTN_H
