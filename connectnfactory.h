/* connectnfactory.h
 *
 * Factory class for creating a ConnectN game. Designed to be used with ConnectN::Stringify().
 * The Stringify() method can be used to output the game state to a csv file, which can later be
 * read and parsed into vector of strings, which can be directly passed to the CreateGame method here.
 *
 */
#ifndef CONNECTNFACTORY_H
#define CONNECTNFACTORY_H

#include <string>
#include <vector>

class ConnectN;

class ConnectNFactory
{
public:
    static ConnectN* CreateGame(const std::vector<std::string> &args);
private:
    ConnectNFactory();
};

#endif // CONNECTNFACTORY_H
