#include "perft.h"

void Perft::test()
{
    std::string line;
    std::ifstream input("tests");
    Board pos;
    for(int i = 0; std::getline(input, line); i++) {
        std::string delimiter = ";";
        std::string token;
        bool first = true;
        size_t p = 0;
        while((p = line.find(delimiter)) != std::string::npos) {
            token = line.substr(0, p);
            if(first) {
                std::cout << token << std::endl;
                pos.parseFen(token);
            }
            else {
                testPosition(token, pos);
            }
            line.erase(0, p + delimiter.length());
            first = false;
        }
        token = line.substr(0, p);
        testPosition(token, pos);
    }
}

void Perft::testPosition(const std::string token, Board& pos)
{
    int depth = token[1] - '0';
    size_t p = token.find(" ");
    std::string target = token.substr(p + 1, token.length());
    nodes = 0;
    testPositionInner(depth, pos);
    if(target.compare("" + target) == 0) {
        std::cout << "D" << depth << " " << nodes << std::endl;
    }
    else {
        std::cout << "D" << depth << " " << nodes << " != " << target << std::endl;
    }
}

void Perft::testPositionInner(const int depth, Board pos)
{
    if(depth == 0) {
        nodes++;
        return;
    }
    MoveList moves = generateAllMoves(pos);
    for(int i = 0; i < moves.getCount(); i++) {
        Move move = moves.getMoves()[i];
        if(!makeMove(move, pos)) {
            continue;
        }
        testPositionInner(depth - 1, pos);
        takeMove(pos);
    }
}
