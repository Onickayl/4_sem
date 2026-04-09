#include "controller.h"
#include <unistd.h>

// используем уникальные коды, не пересекающиеся с буквами
#define KEY_UP    1000
#define KEY_DOWN  1001
#define KEY_RIGHT 1002
#define KEY_LEFT  1003

Controller::Controller(Model& m, View& v) : model(m), view(v), running(true), speed(20000) {}

void Controller::run() 
{
    while (running) 
    {
        if (view.keyPressed()) 
        {
            int key = view.getKey();
            
            switch(key) 
            {
                case KEY_UP:    
                    if (model.getDirection(0) != 2) model.setDirection(0, 0);
                    break;
                case KEY_RIGHT:
                    if (model.getDirection(0) != 3) model.setDirection(0, 1); 
                    break; 
                case KEY_DOWN:  
                    if (model.getDirection(0) != 0) model.setDirection(0, 2);
                    break;
                case KEY_LEFT:  
                    if (model.getDirection(0) != 1) model.setDirection(0, 3);
                    break;

                case 'w': case 'W': 
                    if (model.getDirection(1) != 2) model.setDirection(1, 0);
                    break;
                case 'd': case 'D':
                    if (model.getDirection(1) != 3) model.setDirection(1, 1);
                    break;
                case 's': case 'S':
                    if (model.getDirection(1) != 0) model.setDirection(1, 2);
                    break;
                case 'a': case 'A':
                    if (model.getDirection(1) != 1) model.setDirection(1, 3);
                    break;

                case 'q': case 'Q': running = false; return;
                case 'p': case 'P': 
                    {
                        while (!view.keyPressed()) // пока нет нажатой клавы, ждём
                        {
                            usleep(100000);
                        }

                        view.getKey();
                        break;
                    }
            }
        }
        
        model.update();         // обновляем состояние игры

        // проверка
        if (model.isGameOver())
        {
            view.showGameOver();

            while (true) 
            {
                if(view.keyPressed())
                {
                    int key = view.getKey();
                    
                    if (key == 'q' || key == 'Q')
                    {
                        break;
                    }
                }

                usleep(100000);
            }

            running = false;
            break;
        }
        view.render(model);     // рисуем новый кадр
        usleep(speed);          // ждём
    }
}

void Controller::stop() 
{
    running = false;
}