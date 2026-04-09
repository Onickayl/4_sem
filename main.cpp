#include "model.h"
#include "view.h"
#include "controller.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>

struct winsize w;

int main() 
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int num_Snakes = 1;

    // Создаем поле на весь терминал
    Model model(w.ws_col - 2, w.ws_row - 3, num_Snakes);
    View view;

    // Показываем правила перед началом
    view.showRules();

    Controller controller(model, view);
    
    // Запускаем игру
    controller.run();
    
    return 0;
}


/*
чтобы запустить надо: g++ -std=c++11 main.cpp model.cpp view.cpp controller.cpp -o snake
*/