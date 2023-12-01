#pragma once

#include "Chessboard.h"
#include <curses.h>
#include <memory>
#include <string>

#define BLACK_CHESS "⚆"
#define WHITE_CHESS "⚈"
#define PROMPT      "▫"

class UIHandler{
    public:
    UIHandler();
    // UIHandler(double ratio);
    ~UIHandler();

    void run();

    private:
    struct Pos{
        int i;
        int j;
    };
    enum MappingType{
        W2C,  // windows coordinates to chessboard coordinates
        C2W   // chessboard coordinates to windows coordinates
    };
    void initCurses();
    void drawOn(int i, int j, const std::string &c);
    void drawBoard();
    void drawChess();
    void drawPrompt();
    void drawStatus();
    void mouseEvent();
    void refresh();
    void redrawAll();
    Pos mappingCoords(Pos in, MappingType mp);

    int rowMax, colMax, startRow, startCol, endRow, endCol, n;
    WINDOW *statusWindow, *chessboardWindow;
    std::unique_ptr<Chessboard> cb;

};