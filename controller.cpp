#include "controller.h"
#include <unistd.h>

// используем уникальные коды, не пересекающиеся с буквами
#define KEY_UP    1000
#define KEY_DOWN  1001
#define KEY_RIGHT 1002
#define KEY_LEFT  1003

Controller::Controller(Model& m, View& v) : model(m), view(v), running(true), speed(400000) {}

void Controller::run() 
{
    while (running) 
    {
        if (view.keyPressed()) 
        {
            int key = view.getKey();
            
            switch(key) 
            {
                case KEY_UP:    model.setDirection(0); break;
                case KEY_RIGHT: model.setDirection(1); break;
                case KEY_DOWN:  model.setDirection(2); break;
                case KEY_LEFT:  model.setDirection(3); break;

                case 'w': case 'W': model.setDirection(0); break;
                case 'd': case 'D': model.setDirection(1); break;
                case 's': case 'S': model.setDirection(2); break;
                case 'a': case 'A': model.setDirection(3); break;
                case 'q': case 'Q': running = false; break;
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
        }
        view.render(model);     // рисуем новый кадр
        usleep(speed);          // ждём
    }
}

void Controller::stop() 
{
    running = false;
}