#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"
#include "view.h"

class Controller 
{
private:
    Model* model;      // указатель на модель
    View& view;        // ссылка на представление
    bool running;      // флаг работы игры
    int speed;         // скорость (задержка в мкс)
    int num_snakes;    // кол-во змеек
    bool bots_enabled; // боты включены
    
public:
    Controller(Model* m, View& v, int snake_speed, int num, bool bots = false);
    ~Controller();
    
    void run();        // главный цикл игры
    void stop();       // остановить игру
    void resize_game(int new_width, int new_height);
};

#endif