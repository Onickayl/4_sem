#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"
#include "view.h"

class Controller 
{
private:
    Model* model;       // указатель на модель
    View* view;         // указатель на представление (абстрактный класс)
    bool running;       // флаг работы игры
    int speed;          // скорость (задержка в мкс)
    int num_snakes;     // кол-во змеек
    bool bots_enabled;  // боты включены
    bool is_sfml;       // флаг для SFML режима
    
public:
    Controller(Model* m, View*v, int snake_speed, int num, bool bots = false, bool use_sfml = false);
    ~Controller();
    
    void run();        // главный цикл игры
    void stop();       // остановить игру
    void resize_game(int new_width, int new_height);
};

#endif