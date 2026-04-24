#include "controller.h"
#include "view.h"
#include "terminal_view.h"
#include "sfml_view.h"
#include <unistd.h>
#include <sys/ioctl.h>

// используем уникальные коды, не пересекающиеся с буквами
#define KEY_UP    1000
#define KEY_DOWN  1001
#define KEY_RIGHT 1002
#define KEY_LEFT  1003

Controller::Controller(Model *m, View *v, int snake_speed, int num, bool bots, bool use_sfml)
    : model(m), view(v), running(true), speed(snake_speed), num_snakes(num), bots_enabled(bots), is_sfml(use_sfml) {}

Controller::~Controller()
{
    delete model;
}

void Controller::run()
{
    sf::Clock clock;                    // таймер для контроля скорости игры
    float timer = 0.0f;                 // накопленное время
    float delay = speed / 1000000.0f;   // переводим микросекунды в секунды
                                        // например, 100000 = 0.1 секунды

    // защита от слишком маленькой задержки
    if (delay < 0.03f) 
    {
        delay = 0.03f;
    }

    struct winsize old_size;
    if (!is_sfml && !bots_enabled)
    {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &old_size);
    }

    while (running)
    {

// ручной режим
        if (!bots_enabled)
        {
            if (!is_sfml)  // только для терминала
            {
                struct winsize new_size;
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &new_size);
                
                if (new_size.ws_col != old_size.ws_col || new_size.ws_row != old_size.ws_row)
                {
                    resize_game(new_size.ws_col - 2, new_size.ws_row - 4);
                    old_size = new_size;
                }
            }

            if (view->keyPressed())
            {
                int key = view->getKey();

                switch (key)
                {
                case KEY_UP:
                    model->setDirection(0, 0); 
                    break;
                case KEY_RIGHT:
                    model->setDirection(0, 1); 
                    break;
                case KEY_DOWN:
                    model->setDirection(0, 2); 
                    break;
                case KEY_LEFT:
                    model->setDirection(0, 3);  
                    break;
                case 'w': case 'W':
                    model->setDirection(1, 0);
                    break;
                case 'd': case 'D':
                    model->setDirection(1, 1);
                    break;
                case 's': case 'S':
                    model->setDirection(1, 2);
                    break;
                case 'a': case 'A':
                    model->setDirection(1, 3);
                    break;
                case 'q': case 'Q':
                    running = false;
                    return;
                case 'p': case 'P':
                    model->togglePause();  // просто переключаем паузу, без ожидания
                    break;
                
                }
            }


            // логика - только по таймеру
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();

            // защита от слишком больших значений (если игра подвисла)
            if (time > 0.1f) 
            {
                time = 0.1f;
            }
            
            timer += time;

            if (timer > delay)
            {
                timer -= delay;
                model->update(); // змейка делает шаг

                if (model->isGameOver()) 
                {
                    view->showGameOver();
                    running = false;
                }
            }
            
        }
// режим тестирования ботов (без отрисовки)
        else
        {
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            
            if (time > 0.1f) 
            {
                time = 0.1f;
            }

            timer += time;
            
            while (timer >= delay && running)
            {
                timer -= delay;
                model->update();
                
                if (model->isGameOver())
                {
                    running = false;
                }
            }
        }

        // отрисовка
        view->render(*model);

        // чтобы компьютер не работал на износ, когда ничего не происходит
        if (!is_sfml)
        {
            usleep(10000);  // 10 мс для терминала
        }
        else
        {
            sf::sleep(sf::milliseconds(5));  // 5 мс для SFML
        }
    }
}

void Controller::resize_game(int new_width, int new_height)
{
// сохранить состояние
    std::vector<Snake> saved_snakes = model->getSnakes();
    std::list<Rabbit> saved_rabbits = model->getRabbits();
    std::list<Apple> saved_apples = model->getApples();
    std::list<Snowflake> saved_snowflakes = model->getSnowflakes();

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
    new_model->getApples().clear();
    for (const auto &apple : saved_apples)
    {
        if (apple.pos.x >= 0 && apple.pos.x < new_width &&
            apple.pos.y >= 0 && apple.pos.y < new_height)
        {
            new_model->getApples().push_back(apple);
        }
        // яблоки за границей — просто теряются
    }

// перенести снежинки
    new_model->getSnowflakes().clear();
    for (const auto &snowflake : saved_snowflakes)
    {
        if (snowflake.pos.x >= 0 && snowflake.pos.x < new_width &&
            snowflake.pos.y >= 0 && snowflake.pos.y < new_height)
        {
            new_model->getSnowflakes().push_back(snowflake);
        }
    }


// удалить старую модель и заменить новой

    delete model;
    
    model = new_model;
}

void Controller::stop() 
{
    running = false;
}
