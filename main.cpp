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

// глобальный шрифт 
sf::Font global_font;
bool font_loaded = false;

void manual(bool use_sfml, int num_snakes, int num_dumb, int num_careful, int speed);
void test(int runs, bool use_sfml, int num_dumb, int num_careful, bool silent, int speed);


int main(int argc, char *argv[])
{

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    bool use_sfml = false;
    std::string mode = "";
    int runs = 10;

    // загружаем шрифт один раз для всех SFML окон
    font_loaded = global_font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");

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

    int num_snakes = 0;
    int num_dumb = 0;
    int num_careful = 0;
    bool silent = 0;
    int speed = 0;

    if (mode == "manual")
    {
        std::cout << "Num of manual snakes (no more than two): ";
        std::cin >> num_snakes;
    }

    std::cout << "Num of dumb bots: ";
    std::cin >> num_dumb;

    std::cout << "Num of careful bots: ";
    std::cin >> num_careful;
    
    std::cout << "Speed of snakes (10.000 - 200.000): ";
    std::cin >> speed;

    if (mode == "manual")
    {
        manual(use_sfml, num_snakes, num_dumb, num_careful, speed);
    }
    else if (mode == "test")
    {
        std::cout << "With picture? (0 - no, 1 - yes): ";
        std::cin >> silent;
        test(runs, use_sfml, num_dumb, num_careful, silent, speed);
    }

    return 0;
}

void manual(bool use_sfml, int num_snakes, int num_dumb, int num_careful, int speed)
{

    Model *model = nullptr;
    View *view = nullptr;   // указатель ни на что не указывает, иначе там будет мусор(

    if (use_sfml)
    {   
        model = new Model(40, 25, num_snakes, num_dumb, num_careful, true, true); // false
        view = new SfmlView(40, 25, global_font, false);
    }
    else
    {
        model = new Model(w.ws_col - 2, w.ws_row - 4, num_snakes, num_dumb, num_careful, true, true); // false
        view = new TerminalView(false); // не silent
    }

    Controller controller(model, view, speed, num_snakes, false, use_sfml);

    // Запускаем игру
    controller.run();

    delete view;
}

void test(int runs, bool use_sfml, int num_dumb, int num_careful, bool silent, int speed)
{
    
    std::vector<int> wins(2, 0);
    int num_snakes = 0;

    //std::cout << "silent " << silent << std::endl;

    if (!silent)
    {
        std::cout << "Запуск " << runs << " прогонов..." << std::endl;
        std::cout << "Ждём...оно играет..." << std::endl;
    }

    // режим когда хотим видеть 
    // вменяемый выбор аргуемнтов

    for (int i = 0; i < runs; i++)
    {
        Model *model = nullptr;
        View *view = nullptr;

        if (use_sfml)
        {
            model = new Model(40, 25, num_snakes, num_dumb, num_careful, true, false);
            view = new SfmlView(40, 25, global_font, !silent);
        }
        else
        {
            model = new Model(w.ws_col - 2, w.ws_row - 4, num_snakes, num_dumb, num_careful, true, false);
            view = new TerminalView(!silent);  
        }

        Controller controller(model, view, speed, num_snakes, true, use_sfml);
        controller.run();

        int winner = model->getWinner();

        if (winner >= 0)
        {
            wins[winner-1]++;
        }

        delete view;
    }

// Вывод статистики
    std::cout << "\n=== СТАТИСТИКА ===" << std::endl;
    std::cout << "Тупой бот: " << wins[0] << " побед" << std::endl;
    std::cout << "Осторожный бот: " << wins[1] << " побед" << std::endl;
    //std::cout << "Умный бот: " << wins[2] << " побед" << std::endl;
    
}

