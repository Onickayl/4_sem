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

    int num_snakes = 0;

    // Создаем поле на весь терминал
    Model* model = new Model(w.ws_col - 2, w.ws_row - 3, num_snakes);

    View view;

    Controller controller(model, view, num_snakes);

    // Запускаем игру
    controller.run();

    return 0;
}

/*
чтобы запустить надо: g++ -std=c++11 main.cpp model.cpp view.cpp controller.cpp -o snake
*/