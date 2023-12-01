#include "Chessboard.h"

#include <curses.h>
#include <exception>
#include <memory>
#include <algorithm>
#include <set>
#include <iostream>

Chessboard::Chessboard() : n(8), board(new CHESS[8*8]), chessCount()
{
    reset();
}

void Chessboard::reset(){
    clear();
    current  = BLACK;
    get(3,3) = BLACK;
    get(3,4) = WHITE;
    get(4,3) = WHITE;
    get(4,4) = BLACK;
    count();
  
    boundary.clear();
    for(int i = 2; i <= 5; ++i){
        boundary.insert(ij2idx(i, 2));
        boundary.insert(ij2idx(i, 5));
        boundary.insert(ij2idx(5, i));
        boundary.insert(ij2idx(2, i));
    }

    updatePossiblePlacement(current);
}

int Chessboard::put(unsigned int i, unsigned int j, CHESS chess){
    // return -1 if out of boundary
    if(i >= 8 || j >= 8)
        return -1;
    // return -1 if current step is not valid
    if(candidatePlacement[chess].find(ij2idx(i,j)) == candidatePlacement[chess].end())
        return -1;

    get(i,j) = chess;
    updateBoundary(i,j);
    reversi(i, j, chess);
    updatePossiblePlacement(another(chess));
    chessCount[chess] += 1;
    chessCount[EMPTY] -= 1;

    current = another(chess); 
    return 0;
}

int Chessboard::put(unsigned int i, unsigned int j){
    return put(i,j,current);
}

Chessboard::CHESS &Chessboard::get(unsigned int i, unsigned int j){
    return board.get()[ij2idx(i,j)];
}

Chessboard::CHESS Chessboard::getCurrent(){
    return current;
}

std::vector<Chessboard::Pos> Chessboard::getCandidates(CHESS chess){
    std::vector<Pos> result;
    for(int idx : candidatePlacement[chess]){
        result.push_back(idx2ij(idx));
    }
    return result;
}

int Chessboard::clamp(int i){
    return i < 0 ? 0 : i > 7 ? 7 : i;
}

int Chessboard::ij2idx(unsigned int i, unsigned int j){
    return i*8+j;
}

Chessboard::Pos Chessboard::idx2ij(int idx){
    Pos pos;
    pos.i = idx/8;
    pos.j = idx%8;
    return pos;
}

Chessboard::CHESS Chessboard::another(CHESS chess){
    return chess == WHITE? BLACK : chess == EMPTY ? EMPTY : WHITE;
}

void Chessboard::clear(){
    std::fill(board.get(), board.get()+64, EMPTY);
}

void Chessboard::count(){
    chessCount.clear();
    for(int i = 0; i < 64; ++i){
        chessCount[board.get()[i]]++;
    }
}

void Chessboard::reversi(int i, int j, CHESS chess){
    int idx = ij2idx(i,j);
    for(auto &matchpos : matchedPair[chess][idx]){
        // std::cout << idx << matchpos.idx << std::endl;
        for(int curIdx = idx; curIdx != matchpos.idx; curIdx = matchpos.updateIdx(curIdx)){
            board.get()[curIdx] = chess;
        }
    }
}

void Chessboard::updatePossiblePlacement(CHESS chess){
    candidatePlacement[chess].clear();
    matchedPair[chess].clear();
    for(auto idx : boundary){
        std::set<MatchedPos> endPoint;

        endPoint.insert(_placementLoopHelper(idx, [](int x){return x - 9;}, chess)); // up-left
        endPoint.insert(_placementLoopHelper(idx, [](int x){return x - 8;}, chess)); // up
        endPoint.insert(_placementLoopHelper(idx, [](int x){return x - 7;}, chess)); // up-right
        endPoint.insert(_placementLoopHelper(idx, [](int x){return x - 1;}, chess)); // left
        endPoint.insert(_placementLoopHelper(idx, [](int x){return x + 1;}, chess)); // right
        endPoint.insert(_placementLoopHelper(idx, [](int x){return x + 7;}, chess)); // down-left
        endPoint.insert(_placementLoopHelper(idx, [](int x){return x + 8;}, chess)); // down
        endPoint.insert(_placementLoopHelper(idx, [](int x){return x + 9;}, chess)); // down-right

        // endPoint.erase(-1); // remove null solution
        if(endPoint.size() != 1) candidatePlacement[chess].insert(idx);  // found
        for(auto matched : endPoint){
            if (matched.idx != -1)
                matchedPair[chess][idx].push_back(matched);
        }
    }
}

Chessboard::MatchedPos Chessboard::_placementLoopHelper(int idx, std::function<int(int)> updateIdx, CHESS chess){
    int nAnotherChess = 0;
    for(idx = updateIdx(idx); idx >= 0 && idx < 64; idx = updateIdx(idx)){
        if(board.get()[idx] == EMPTY){
            nAnotherChess = 0;
            break;
        }
        if(board.get()[idx] == chess) break;
        nAnotherChess++;
    }
    if (nAnotherChess == 0)
        return {-1, updateIdx};
    else
        return {idx, updateIdx};
}

void Chessboard::updateBoundary(int i, int j){
    boundary.erase(ij2idx(i,j));

    int im = clamp(i-1);
    int ip = clamp(i+1);
    int jm = clamp(j-1);
    int jp = clamp(j+1);

    std::set<int> neighbor = {
        ij2idx(im, jm),
        ij2idx(im, j),
        ij2idx(im, jp),
        ij2idx(i, jm),
        ij2idx(i, jp),
        ij2idx(ip, jm),
        ij2idx(ip, j),
        ij2idx(ip, jp)
    };
    for(auto idx : neighbor){
        if (board.get()[idx] == EMPTY){
            boundary.insert(idx);
        }
    }
}