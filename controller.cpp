#include "controller.h"
#include <unistd.h>
#include <sys/ioctl.h>

// используем уникальные коды, не пересекающиеся с буквами
#define KEY_UP    1000
#define KEY_DOWN  1001
#define KEY_RIGHT 1002
#define KEY_LEFT  1003

Controller::Controller(Model *m, View &v, int snake_speed, int num, bool bots)
    : model(m), view(v), running(true), speed(snake_speed), num_snakes(num), bots_enabled(bots) {}

Controller::~Controller()
{
    delete model;
}

void Controller::run()
{

    struct winsize old_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &old_size);

    while (running)
    {
        if (bots_enabled == 0)
        {
            // ручной режим

            struct winsize new_size;
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &new_size);

            if (new_size.ws_col != old_size.ws_col || new_size.ws_row != old_size.ws_row)
            {
                resize_game(new_size.ws_col - 2, new_size.ws_row - 4 );
                old_size = new_size;
            }

            if (view.keyPressed())
            {
                int key = view.getKey();

                switch (key)
                {
                case KEY_UP:
                    if (model->getDirection(0) != 2)
                        model->setDirection(0, 0);
                    break;
                case KEY_RIGHT:
                    if (model->getDirection(0) != 3)
                        model->setDirection(0, 1);
                    break;
                case KEY_DOWN:
                    if (model->getDirection(0) != 0)
                        model->setDirection(0, 2);
                    break;
                case KEY_LEFT:
                    if (model->getDirection(0) != 1)
                        model->setDirection(0, 3);
                    break;

                case 'w':
                case 'W':
                    if (model->getDirection(1) != 2)
                        model->setDirection(1, 0);
                    break;
                case 'd':
                case 'D':
                    if (model->getDirection(1) != 3)
                        model->setDirection(1, 1);
                    break;
                case 's':
                case 'S':
                    if (model->getDirection(1) != 0)
                        model->setDirection(1, 2);
                    break;
                case 'a':
                case 'A':
                    if (model->getDirection(1) != 1)
                        model->setDirection(1, 3);
                    break;

                case 'q':
                case 'Q':
                    running = false;
                    return;
                case 'p':
                case 'P':
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

            model->update(); // обновляем состояние игры

            // проверка
            if (model->isGameOver())
            {
                view.showGameOver();

                while (true)
                {
                    if (view.keyPressed())
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
            view.render(*model); // рисуем новый кадр
            usleep(speed);       // ждём
        }
        else
        {
            // в режиме прогонов — только update
            model->update();

            usleep(10000);  // небольшая задержка (10 мс)

            if (model->isGameOver())
            {
                running = false;
            }
        }
    }
}

void Controller::resize_game(int new_width, int new_height)
{
// сохранить состояние
    std::vector<Snake> saved_snakes = model->getSnakes();
    std::list<Rabbit> saved_rabbits = model->getRabbits();
    //std::list<Apple> saved_apples = model->getApples();

// создать новую модель
    Model *new_model = new Model(new_width, new_height, num_snakes);

// перенести змеек
    new_model->getSnakes().clear();

    for (const auto &old_snake : saved_snakes)
    {
        if (!old_snake.isAlive)
            continue;

        // Создать змейку с сохранённым телом
        Snake new_snake(0, 0); // временные координаты
        new_snake.body = old_snake.body;
        new_snake.direction = old_snake.direction;
        new_snake.isAlive = old_snake.isAlive;
        new_snake.bot_type = old_snake.bot_type;
        new_snake.color = old_snake.color;

        // Проверить, что голова в пределах нового поля
        Position head = new_snake.body.front();
        if (head.x < 0 || head.x >= new_width || head.y < 0 || head.y >= new_height)
        {
            new_snake.isAlive = false; // змейка умерла при изменении размера
        }

        new_model->getSnakes().push_back(new_snake);
    }

// перенести кроликов
    new_model->getRabbits().clear();
    for (const auto &rabbit : saved_rabbits)
    {
        if (rabbit.pos.x >= 0 && rabbit.pos.x < new_width &&
            rabbit.pos.y >= 0 && rabbit.pos.y < new_height)
        {
            new_model->getRabbits().push_back(rabbit);
        }
        // кролики за границей — просто теряются
    }
    
// перенести яблоки
/*
    new_model->getApples().clear();
    for (const auto &apple : saved_apples)
    {
        if (apple.pos.x >= 0 && apple.pos.x < new_width &&
            apple.pos.y >= 0 && apple.pos.y < new_height)
        {
            new_model->getApples().push_back(apple);
        }
        // яблоки за границей — просто теряются
    }*/


// удалить старую модель и заменить новой

    delete model;
    
    model = new_model;
}

void Controller::stop() 
{
    running = false;
}
