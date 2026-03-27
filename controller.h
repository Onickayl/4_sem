#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"
#include "view.h"

class Controller 
{
private:
    Model& model;      // ссылка на модель
    View& view;        // ссылка на представление
    bool running;      // флаг работы игры
    int speed;         // скорость (задержка в мс)
    
public:
    Controller(Model& m, View& v);
    
    void run();        // главный цикл игры
    void stop();       // остановить игру
};

#endif