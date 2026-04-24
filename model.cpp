#include "model.h"
#include <cstdlib> // для rand(), srand()
#include <ctime>   // для time()
#include <iostream>
#include <stdio.h>

// Конструктор
Model::Model(int w, int h, int num_Snakes, int num_dumb, int num_careful, bool bots, bool manual) 
    : width(w), height(h), bots_enabled(bots), gameOver(false), eaten_rabbits(0), eaten_apples(0), eaten_snowflakes(0), paused(false), is_manual(manual)
{
    // инициализируем генератор случайных чисел
    static bool seeded = false;
    if (!seeded)
    {
        srand(time(nullptr));
        seeded = true;
    }
    /*
    Без srand(time(NULL)) - rand() будет выдавать одну и ту же последовательность чисел
    При каждом запуске игры кролики будут в одних и тех же местах
    С srand(time(NULL))- при каждом запуске игры последовательность разная
    Кролики каждый раз в новых местах
    */

// добавляем змей
    for (int i = 0; i < num_Snakes; i++)
    {
        int startX = width / (2 * (i + 1));
        int startY = height / (2 * (i + 1));

        snakes.push_back(Snake(startX, startY));
    }

// Змей ботов
    int num_bot = num_dumb + num_careful;

    if (bots_enabled == 1)
    {
        // тупой бот
        for (int i = 0; i < num_dumb; i++)
        {
            Snake dumb(width/4 + 5*i, height/2 + 1 + (height/(2*num_dumb))*i);
            dumb.bot_type = 1;
            dumb.color = 36;
            snakes.push_back(dumb);
        }

        // осторожный бот
        for (int i = 0; i < num_careful; i++)
        {
            Snake careful(width*0.65 + 5*i, height/2 + 1 + (height/(2*num_careful))*i);
            careful.bot_type = 2;
            careful.color = 33;
            snakes.push_back(careful);
        }
    }

// Добавляем кроликов
    for (int i = 0; i < (num_Snakes + num_bot) * 2; i++)
    {
        rabbits.push_back(Rabbit(rand() % width, rand() % height)); // случайное число от 0 до width-1 и случайное число от 0 до height-1
    }

}

void Model::update()
{

    if (isGameOver() || paused)
    {
        return;
    }

    for (int i = 0; i < snakes.size(); i++)
    {
        Snake &snake = snakes[i];

        if (!snake.isAlive)
        {
            continue; // пропускаем мертвых
        }

        // новое направление - нельзя развернуться на 180 градусов
        if ((snake.direction == 0 && snake.next_direction == 2) ||
            (snake.direction == 2 && snake.next_direction == 0) ||
            (snake.direction == 1 && snake.next_direction == 3) ||
            (snake.direction == 3 && snake.next_direction == 1))
        {
            // разворот запрещён - оставляем текущее направление
            snake.next_direction = snake.direction;
        }
        else
        {
            // можно применить новое направление
            snake.direction = snake.next_direction;
        }

// 1. получаем голову (front() - первый элемент)
        Position head = snake.body.front();

    // боты для режима test
        if (bots_enabled)
        {
            if (snake.bot_type == 1)
            {   
        // тупой бот
                bot_dir(snake, head);
                snake.next_direction = snake.direction;  // синхронизируем для ботов
            }
            else if (snake.bot_type == 2)
            { 
        // осторожный бот
                int old_dir = snake.direction;
                bot_dir(snake, head);
                int dir = snake.direction;

                // Список направлений для проверки
                int directions[4] = 
                {
                    dir,           // желаемое
                    (dir + 3) % 4, // налево от желаемого
                    (dir + 1) % 4, // направо от желаемого
                    old_dir        // до bot_dir (если всё плохо)
                };

                // перебираем и выбираем первое безопасное
                bool found = false;

                for (int d : directions)
                {
                    if (is_safe(head, i, snakes, width, height, d))
                    {
                        snake.direction = d;
                        snake.next_direction = d;
                        found = true;
                        break;
                    }
                }

                // если ничего не безопасно — оставляем текущее (которое было до bot_dir)
                if (!found)
                {
                    snake.direction = old_dir;
                    snake.next_direction = old_dir;
                }
            }
        }

// 2. вычисляем новую позицию головы
        Position newHead = head;

        switch (snake.direction)
        {
        case 0:
            newHead.y--;
            break; // вверх
        case 1:
            newHead.x++;
            break; // вправо
        case 2:
            newHead.y++;
            break; // вниз
        case 3:
            newHead.x--;
            break; // влево
        }

// Проверки

        // если ударяется об рамку, то игра завершается
        if (check_wall(newHead, width,height))
        {
            snake.isAlive = false;
            continue; // пропускаем дальнейшую обработку этой змейки
        }

        // если ударяется об себя, то она "умирает"
        if (check_self(snake, newHead))
        {
            snake.isAlive = false; // умирает только эта змейка
            continue;
        }

        // если стукается с другими змейками -> "умирает"
        if (check_other(snakes, i, newHead))
        {
            snake.isAlive = false; // умирает только та змейка, что налетела
            continue;
        }

// 3. добавляем новую голову
        snake.body.push_front(newHead);



// 4. проверяем, съела ли змейка кролика или яблоко или снежинку
// если съела кролика - добавляем 1 тело и добавляем кролика
// если съела яблоко - добавляем 3 тела змеи в конец раз в 10 кроликов
// если съела снежинку - убираем 5 долек у конца змейки раз в 17 кроликов

        bool ateRabbit = eat_rabbit(newHead, rabbits);
        bool ateApple = eat_apple(newHead, apples);
        bool ateSnowflake = eat_snowflake(newHead, snowflakes);

        if (ateRabbit)
        {
            snake.grow += 1;
            rabbits.push_back(Rabbit(rand() % width, rand() % height));
            eaten_rabbits++;

            if (eaten_rabbits % 10 == 0)
            {
                // новое яблоко
                apples.push_back(Apple(rand() % width, rand() % height));
            }

            if ((eaten_rabbits - 3) % 10 == 0 && !apples.empty())
            {
                // типа если не успели съесть в течении 3-х кроликов, яблоко удалится - окак - отак
                apples.pop_back();
            }

            if (eaten_rabbits % 17 == 0)
            {
                // новая снежинка
                snowflakes.push_back(Snowflake(rand() % width, rand() % height));
            }

            if ((eaten_rabbits - 3) % 17 == 0 && !snowflakes.empty())
            {
                // типа если не успели съесть в течении 3-х кроликов, снежинка удалится - окак - отак
                snowflakes.pop_back();
            }
        }

        if (ateApple)
        {
            eaten_apples++;
            snake.grow += 3;
        }

        if (ateSnowflake)
        {
            eaten_snowflakes++;

            if (snake.body.size() < 6)
            {
                snake.isAlive = false;
            }
            else
            {
                for (int j = 0; j < 5; j++)
                {
                    snake.body.pop_back();
                }
            }
        }

// механизм роста - выполняется всегда, для всех случаев
        if (snake.grow > 0)
        {
            snake.grow--; // не удаляем хвост, ждём следующего тика
        }
        else
        {
            snake.body.pop_back(); // удаляем хвост
        }

        /*
        auto - Компилятор сам определяет тип переменной. Вместо длинного std::list<Position>::iterator
        rabbits - список ВСЕХ кроликов, и при увеличении итератора, мы переходим к след кролику, у кот свои координаты (x, y)
        */
    }

// проверка gameover

    int count = 0;
    int last_alive_type = -1;

    for (int i = 0; i < snakes.size(); i++) 
    {
        if (snakes[i].isAlive) 
        {
            count++;
            last_alive_type = snakes[i].bot_type;
        }
    }

    if (count == 1 && !is_manual) 
    {
        winner = last_alive_type;
        gameOver = true;
    } 
    else if (count == 0) 
    {
        winner = -1;  // ничья (все умерли одновременно)
        gameOver = true;
    }
}


// БОТ
void Model::bot_dir(Snake &snake, Position head)
{
    int min = 999999;
    int targetX = 0, targetY = 0;

    for(auto &apple : apples)
    {
        if (!apple.isAlive)
        {
            continue;
        }

        int dist = abs(head.x - apple.pos.x) + abs(head.y - apple.pos.y);
        if (dist < min)
        {
            min = dist;
            targetX = apple.pos.x;
            targetY = apple.pos.y;
        }
    }

    if (min == 999999)
    {
        for (auto &rabbit : rabbits)
        {
            if (!rabbit.isAlive)
            {
                continue;
            }

            int dist = abs(head.x - rabbit.pos.x) + abs(head.y - rabbit.pos.y);
            if (dist < min)
            {
                min = dist;
                targetX = rabbit.pos.x;
                targetY = rabbit.pos.y;
            }
        }
    }

    int newDir = snake.direction; // пока оставляем текущее

    // Сначала пробуем двигаться по горизонтали
    if (head.x < targetX)
    {
        newDir = 1; // вправо
    }
    else if (head.x > targetX)
    {
        newDir = 3; // влево
    }
    // Если по горизонтали не надо, двигаемся по вертикали
    else if (head.y < targetY)
    {
        newDir = 2; // вниз
    }
    else if (head.y > targetY)
    {
        newDir = 0; // вверх
    }

    // Противоположные направления: 0↔2, 1↔3
    if ((newDir == 0 && snake.direction == 2) ||
        (newDir == 2 && snake.direction == 0) ||
        (newDir == 1 && snake.direction == 3) ||
        (newDir == 3 && snake.direction == 1))
    {
        // Не меняем направление
        newDir = snake.direction;
    }

    snake.direction = newDir;
}

// Проверки
bool Model::check_wall(Position newHead, int width, int height)
{
    if (newHead.x < 0 || newHead.x >= width || newHead.y < 0 || newHead.y >= height)
    {
        return true;
    }

    return false;
}

bool Model::check_self(Snake &snake, Position newHead)
{
    for (auto iter = snake.body.begin(); iter != snake.body.end(); ++iter)
    {
        if (iter->x == newHead.x && iter->y == newHead.y)
        {
            return true;
        }
    }

    return false;
}

bool Model::check_other(std::vector<Snake> &snakes, int index, Position newHead)
{
    for (int j = 0; j < snakes.size(); j++)
    {
        if (j == index || !snakes[j].isAlive)
        {
            continue;
        }

        for (auto iter = snakes[j].body.begin(); iter != snakes[j].body.end(); ++iter)
        {
            if (iter->x == newHead.x && iter->y == newHead.y)
            {
                return true;
            }
        }
    }

    return false;
}

bool Model::eat_rabbit(Position newHead, std::list<Rabbit> &rabbits)
{
    // & чтобы можно было менять значение переменной
    // для каждого элемента rabbit в контейнере (список) rabbits
    for (auto &rabbit : rabbits)
    {
        if (rabbit.isAlive && rabbit.pos.x == newHead.x && rabbit.pos.y == newHead.y)
        {
            rabbit.isAlive = false;
            return true;
        }
    }

    return false;
}

bool Model::eat_apple(Position newHead, std::list<Apple> &apples)
{
    // & чтобы можно было менять значение переменной
    // для каждого элемента apple в списоке apples
    for (auto &apple : apples)
    {
        if (apple.isAlive && apple.pos.x == newHead.x && apple.pos.y == newHead.y)
        {
            apple.isAlive = false;
            return true;
        }
    }

    return false;
}

bool Model::eat_snowflake(Position newHead, std::list<Snowflake> &snowflakes)
{
    // & чтобы можно было менять значение переменной
    // для каждого элемента snowflake в списоке snowflakes
    for (auto &snowflake : snowflakes)
    {
        if (snowflake.isAlive && snowflake.pos.x == newHead.x && snowflake.pos.y == newHead.y)
        {
            snowflake.isAlive = false;
            return true;
        }
    }

    return false;
}

bool Model::is_safe(Position head, int index, std::vector<Snake> &snakes, int width, int height, int dir)
{
    Position newHead = head;

    switch (dir)
    {
        case 0:
            newHead.y--;
            break; // вверх
        case 1:
            newHead.x++;
            break; // вправо
        case 2:
            newHead.y++;
            break; // вниз
        case 3:
            newHead.x--;
            break; // влево
    }

    if (!check_wall(newHead, width, height) && !check_self(snakes[index], newHead) && !check_other(snakes, index, newHead))
    {
        return true;
    }

    return false;
}

// просто возвращаем данные
const std::vector<Snake> &Model::getSnakes() const
{
    return snakes;
}

const std::list<Rabbit> &Model::getRabbits() const
{
    return rabbits;
}

const std::list<Apple> &Model::getApples() const
{
    return apples; 
}

const std::list<Snowflake> &Model::getSnowflakes() const
{
    return snowflakes; 
}

int Model::getWidth() const
{
    return width;
}

int Model::getHeight() const
{
    return height;
}

void Model::setDirection(int id_snake, int dir)
{
    //snakes[id_snake].direction = dir;
    if (id_snake >= 0 && id_snake < snakes.size())
    {
        // сохраняем новое направление, не применяем сразу
        snakes[id_snake].next_direction = dir;
    }
}

int Model::getDirection(int id_snake) const
{
    return snakes[id_snake].direction;
}

int Model::getWinner() const
{
    int winner = -1;

    for (int i = 0; i < snakes.size(); i++) 
    {
        if (snakes[i].isAlive) 
        {
            if (winner != -1) 
            {
                return -1;  // больше одного живого
            }

            winner = snakes[i].bot_type;
        }
    }
    return winner;
}

bool Model::isGameOver() const
{
    return gameOver;
}