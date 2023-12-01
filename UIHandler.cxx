#include "UIHandler.h"
#include "Chessboard.h"

#include <curses.h>
#include <locale>
#include <iostream>
#include <exception>
#include <sstream>

void UIHandler::initCurses(){
    std::locale::global(std::locale(""));
    initscr();
    curs_set(0);  //Hide cursor
    cbreak();
    noecho();
    start_color();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, nullptr);
    getmaxyx(stdscr, rowMax, colMax);
    if(rowMax < 20 || colMax < 60){
        std::stringstream ss;
        ss << "Terminal too small, should larger than 60x20, current terminal size is "
           << colMax <<"x"<< rowMax;
        throw std::runtime_error(ss.str());
    }

    int split = 0.3*colMax;
    statusWindow = newwin(rowMax, split, 0, 0);
    chessboardWindow = newwin(rowMax, colMax-split, 0, split);
    getmaxyx(chessboardWindow, rowMax, colMax);
    ::refresh();
}

void UIHandler::drawBoard(){
    startRow = (rowMax - 2*n-1)/2;
    startCol = (colMax - 4*n-1)/2;
    endRow   = startRow+2*n;
    endCol   = startCol+4*n;
    for(int i = startRow; i <= endRow; i=i+2){
        for(int j = startCol; j <= endCol; ++j){
            mvwaddch(chessboardWindow, i, j, ACS_HLINE);
        }
    }
    for(int i = startRow; i <= endRow; ++i){
        for(int j = startCol; j <= endCol; j=j+4){
            mvwaddch(chessboardWindow, i, j, ACS_VLINE);
        }
    }
    for(int i = startRow; i <= endRow; i=i+2){
        for(int j = startCol; j <= endCol; j=j+4){
            mvwaddch(chessboardWindow, i, j, ACS_PLUS);
        }
    }
    for(int i = startRow; i <= endRow; i=i+2){
        mvwaddch(chessboardWindow, i,startCol,ACS_LTEE);
        mvwaddch(chessboardWindow, i,endCol,ACS_RTEE);
    }
    for(int j = startCol; j <= endCol; j=j+4){
        mvwaddch(chessboardWindow, startRow, j, ACS_TTEE);
        mvwaddch(chessboardWindow, endRow, j, ACS_BTEE);
    }
    mvwaddch(chessboardWindow, startRow, startCol, ACS_ULCORNER);
    mvwaddch(chessboardWindow, startRow, endCol, ACS_URCORNER);
    mvwaddch(chessboardWindow, endRow, startCol, ACS_LLCORNER);
    mvwaddch(chessboardWindow, endRow, endCol, ACS_LRCORNER);
}

void UIHandler::drawOn(int i, int j, const std::string &c){
    Pos winCoord = mappingCoords({i,j}, C2W);
    mvwaddstr(chessboardWindow, winCoord.i, winCoord.j, c.c_str());
}

void UIHandler::drawChess(){
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(cb->get(i,j) == Chessboard::BLACK){
                drawOn(i,j,BLACK_CHESS);
            }
            else if(cb->get(i,j) == Chessboard::WHITE){
                drawOn(i,j,WHITE_CHESS);
            }
        }
    }
}

void UIHandler::drawPrompt(){
    auto v = cb->getCandidates(cb->getCurrent());
    for(auto pos : v){
        drawOn(pos.i, pos.j, PROMPT);
    }
}

void UIHandler::drawStatus(){
    // printw();
}
UIHandler::Pos UIHandler::mappingCoords(Pos in, MappingType mp){
    Pos out;
    if (mp == W2C){
        if(in.i < startRow || in.i > endRow || in.j < startCol || in.j > endCol){
            return {-1,-1};
        }
        
        out.i = (in.i - startRow)/2;
        out.j = (in.j - startCol)/4;
        
        if(((in.i - startRow)%2==0) || ((in.j - startCol)%4 == 0)){
            out.i = out.j = -1;
        }
    }
    else{
        out.i = startRow + in.i*2+1;
        out.j = startCol + in.j*4+2;
    }
    return out;
}

void UIHandler::mouseEvent(){
    MEVENT mevent;
    if(getmouse(&mevent) == OK){
        if (mevent.bstate & BUTTON1_CLICKED){
            if(wenclose(chessboardWindow, mevent.y, mevent.x)){
                int x = mevent.x, y = mevent.y;
                wmouse_trafo(chessboardWindow, &y, &x, false);
                Pos chessBoardCoord = mappingCoords({y,x}, W2C);
                // mvwprintw(chessboardWindow, 1, 1, "%02d, %02d", chessBoardCoord.i, chessBoardCoord.j);
                if(chessBoardCoord.i >= 0 && chessBoardCoord.j >= 0){
                    cb->put(chessBoardCoord.i, chessBoardCoord.j);
                }
            }
            else if(wenclose(statusWindow, mevent.y, mevent.x)){
                ;//noting to do for now
            }
        }
    }
}

void UIHandler::refresh(){
    ::refresh();
    wrefresh(chessboardWindow);
    wrefresh(statusWindow);
}

void UIHandler::redrawAll(){
    clear();
    wclear(statusWindow);
    wclear(chessboardWindow);
    box(statusWindow, 0, 0);
    box(chessboardWindow, 0, 0);
    drawBoard();
    drawChess();
    drawPrompt();
    refresh();
}

UIHandler::UIHandler():cb(new Chessboard){
    initCurses();
    n = cb->size();
}

UIHandler::~UIHandler(){
    delwin(statusWindow);
    delwin(chessboardWindow);
    endwin();
}

void UIHandler::run(){
    while(1){
        redrawAll();
        int ch = getch();
        if(ch == 'q') break;
        if(ch == KEY_MOUSE){
            mouseEvent();
        }
    }
}