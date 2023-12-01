#include <curses.h>
#include <locale>
#include <iostream>
#include <memory>
#include <exception>

#include "Chessboard.h"
#include "UIHandler.h"

int main(){

    std::shared_ptr<UIHandler> handler;
    try{
        handler.reset(new UIHandler);
    }
    catch (const std::runtime_error &e){
        std::cerr << e.what() << std::endl;
        return 0;
    }
    handler->run();
    // Chessboard *c = new Chessboard;
    // auto v = c->getCandidates(Chessboard::BLACK);
    // std::cout << c->put(2,4) << std::endl;


    // std::cout << c->getCurrent() << std::endl;
    // c.draw();
    // printw("HELLO★WORLD");
    // int ch = getch();
    // MEVENT mevent;
    // if(ch == KEY_MOUSE){
    //     if(getmouse(&mevent) == OK){
    //         mvprintw(1,0,"OK!");
    //     //     if (mevent.bstate & BUTTON1_PRESSED){
    //             mvprintw(0,0,"%d,%d,%d", mevent.x, mevent.y, mevent.z);
    //     //     }
    //     }
    // }

    // getch();
    // getch();
    // endwin();
    // std::cout << c.n << std::endl;
    return 0;
}