#include "model.h"
#include "view.h"
#include "terminal_view.h"
#include "sfml_view.h"
#include "controller.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

struct winsize w;

void manual(bool use_sfml);
void test(int runs, bool use_sfml);


int main(int argc, char *argv[])
{

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    bool use_sfml = false;
    std::string mode = "";
    int runs = 10;

    if (argc == 1)
    {
        std::cout << "Please write: ./snake [manual|test N] [sfml]" << std::endl;
        return 0;
    }

// аргументы командной строки
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "sfml") == 0)
        {
            use_sfml = true;
        }
        else if (strcmp(argv[i], "manual") == 0)
        {
            mode = "manual";
        }
        else if (strcmp(argv[i], "test") == 0)
        {
            mode = "test";
            if (i + 1 < argc && atoi(argv[i + 1]) > 0)
            {
                runs = atoi(argv[i + 1]);
                i++;
            }
        }
    }

    if (mode == "manual")
    {
        manual(use_sfml);
    }
    else if (mode == "test")
    {
        test(runs, use_sfml);
    }

    return 0;
}

void manual(bool use_sfml)
{
    int num_snakes = 2;
    int speed = 2*100000;

    Model *model = nullptr;
    View *view = nullptr;   // указатель ни на что не указывает, иначе там будет мусор(

    if (use_sfml)
    {   
        model = new Model(40, 25, num_snakes, false);
        view = new SfmlView(40, 25);
    }
    else
    {
        model = new Model(w.ws_col - 2, w.ws_row - 4, num_snakes, false);
        view = new TerminalView(false); // не silent
    }

    Controller controller(model, view, speed, num_snakes, false, use_sfml);

    // Запускаем игру
    controller.run();

    delete view;
}

void test(int runs, bool use_sfml)
{
    
    std::vector<int> wins(2, 0);
    int num_snakes = 0;
    int speed = 10000*20;

    std::cout << "Запуск " << runs << " прогонов..." << std::endl;
    std::cout << "Ждём...оно играет..." << std::endl;

    for (int i = 0; i < runs; i++)
    {
        Model *model = nullptr;
        View *view = nullptr;
        if (use_sfml)
        {
            model = new Model(40, 25, num_snakes, true);
            view = new SfmlView(40, 25, true);
        }
        else
        {
            model = new Model(w.ws_col - 2, w.ws_row - 4, num_snakes, true);
            view = new TerminalView(true); // silent = true (быстрые прогоны)
        }

        Controller controller(model, view, speed, num_snakes, true, use_sfml);
        controller.run();

        int winner = model->getWinner();

        if (winner >= 0)
        {
            wins[winner]++;
        }

        delete view;
    }

// Вывод статистики
    std::cout << "\n=== СТАТИСТИКА ===" << std::endl;
    std::cout << "Тупой бот: " << wins[0] << " побед" << std::endl;
    std::cout << "Осторожный бот: " << wins[1] << " побед" << std::endl;
    //std::cout << "Умный бот: " << wins[2] << " побед" << std::endl;
    
}

