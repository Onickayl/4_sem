#include "model.h"
#include "view.h"
#include "controller.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

struct winsize w;

void manual();
void test(int runs);


int main(int argc, char *argv[])
{

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    //int width = w.ws_col - 2;

    if (argc >= 2)
    {
        if (strcmp(argv[1], "manual") == 0)
        {
            manual();
        }
        else if (strcmp(argv[1], "test") == 0)
        {
            int runs = 10;

            if (argc >= 3)
            {
                runs = atoi(argv[2]);
            }
                
            test(runs);
        }
        else
        {
            std::cout << "Использование: ./snake [manual|test N]" << std::endl;
        }
    }
    else
    {
        // По умолчанию — ручной режим
        manual();
    }

    return 0;
}

void manual()
{
    int num_snakes = 2;
    int speed = 2*100000;

    // Создаем поле на весь терминал
    Model *model = new Model(w.ws_col - 2, w.ws_row - 4 , num_snakes, false);

    View view;

    Controller controller(model, view, speed, num_snakes, false);

    // Запускаем игру
    controller.run();

}

void test(int runs)
{
    
    std::vector<int> wins(3, 0);
    int num_snakes = 0;
    int speed = 10000;

    std::cout << "Ждём...оно играет..." << std::endl;

    for (int i = 0; i < runs; i++)
    {
        Model *model = new Model(w.ws_col - 2, w.ws_row - 4 , num_snakes, true);
        View view(true);
        Controller controller(model, view, speed, num_snakes, true);
        controller.run();

        int winner = model->getWinner(); 

        if (winner >= 0) 
        {
            wins[winner]++;
        }
        
        //std::cout << "Прогон " << i + 1 << " завершён" << std::endl;
    }

// Вывод статистики
    std::cout << "\n=== СТАТИСТИКА ===" << std::endl;
    std::cout << "Тупой бот: " << wins[0] << " побед" << std::endl;
    std::cout << "Осторожный бот: " << wins[1] << " побед" << std::endl;
    std::cout << "Умный бот: " << wins[2] << " побед" << std::endl;
    
}

/*
чтобы запустить надо: g++ -std=c++11 main.cpp model.cpp view.cpp controller.cpp -o snake
*/