#pragma once
#include <memory>
#include <map>
#include <set>
#include <functional>
#include <vector>

class Chessboard{
    public:
    enum CHESS{
        WHITE = -1,
        BLACK = 1,
        EMPTY = 0
    };

    struct Pos{
        int i;
        int j;
    };

    Chessboard();
    int size(){return n;}

    int put(unsigned int i, unsigned int j, CHESS chess);
    int put(unsigned int i, unsigned int j);
    CHESS &get(unsigned int i, unsigned int j);
    CHESS getCurrent();
    std::vector<Pos> getCandidates(CHESS chess);

    // void draw();
    void reset();

    private:
    struct MatchedPos{
        int idx;
        std::function<int(int)> updateIdx;
        bool operator<(const MatchedPos & rhs) const{
            return this->idx < rhs.idx;
        }
    };

    int   ij2idx(unsigned int i, unsigned int j);
    Pos   idx2ij(int idx);
    int   clamp(int i);
    CHESS another(CHESS);
    void  clear();
    void  count();
    void  reversi(int i, int j, CHESS chess);
    void  updatePossiblePlacement(CHESS);
    MatchedPos _placementLoopHelper(int idx, std::function<int(int)> updateIdx, CHESS chess);
    void  updateBoundary(int i, int j);


    

    CHESS current = BLACK;
    unsigned int n;
    std::unique_ptr<CHESS>            board;
    std::map<CHESS, int>              chessCount;
    std::map<CHESS, std::set<int>>    candidatePlacement;
    std::map<CHESS, std::map<int, std::vector<MatchedPos>>> matchedPair;
    std::set<int>                     boundary;
};