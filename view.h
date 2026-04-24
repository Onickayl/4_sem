#ifndef VIEW_H
#define VIEW_H

#include "model.h"

class View 
{
public:
    virtual ~View() = default;                      // виртуальный деструктор
    
    virtual void render(const Model& model) = 0;    // отрисовка
    virtual bool keyPressed() = 0;                  // нажата клавиша?
    virtual int getKey() = 0;                       // какая клавиша?
    virtual void showGameOver() = 0;                // показать конец игры
};

/*
... = 0 значит, что у самого View нет реализации этого метода, его обязаны предоставить классы-наследники
*/

#endif