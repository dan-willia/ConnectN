#include "board.h"
#include "connectn.h"
#include "connectnfactory.h"
#include "player.h"

#include <string>
#include <vector>

/* Create a game from a vector of strings.
 *
 * @param args A vector of strings that have been parsed from the ConnectN::Stringify() method.
 * @return A pointer to a new ConnectN instance.
 */
ConnectN* ConnectNFactory::CreateGame(const std::vector<std::string> &args)
{
    // args format: 
    // (0)uid, (1)game_name,
    // (2)p1_num, (3)p1_name, (4)p1_can_clear_row, (5)p1_can_clear_col, (6)p1_can_convert,
    // (7)p2_num, (8)p2_name, (9)p2_can_clear_row, (10)p2_can_clear_col, (11)p2_can_convert,
    // (12)board,
    // (13)draws, (14)p1_score, (15)p2_score, (16)round, (17)n, (18)whose_turn,

    int uid{std::stoi(args[0])};
    std::string game_name{args[1]};

    std::string p1_name{args[3]};
    bool p1_can_clear_row{args[4] == "1" ? true : false};
    bool p1_can_clear_col{args[5] == "1" ? true : false};
    bool p1_can_convert{args[6] == "1" ? true : false};
    Player p1{1, p1_name, p1_can_clear_row, p1_can_clear_col, p1_can_convert};

    std::string p2_name{args[8]};
    bool p2_can_clear_row{args[9] == "1" ? true : false};
    bool p2_can_clear_col{args[10] == "1" ? true : false};
    bool p2_can_convert{args[11] == "1" ? true : false};
    Player p2{2, p2_name, p2_can_clear_row, p2_can_clear_col, p2_can_convert};

    int draws{std::stoi(args[13])};
    int p1_score{std::stoi(args[14])};
    int p2_score{std::stoi(args[15])};
    std::vector<int>score{draws, p1_score, p2_score};

    int round{std::stoi(args[16])};
    int n{std::stoi(args[17])};
    int whose_turn{std::stoi(args[18])};

    Board board{args[12]};

    return new ConnectN{
                        uid, game_name, 
                        p1, p2,
                        board, 
                        score, round, n, whose_turn
                       };
}
