#include "model.h"
#include <cstdlib>  // для rand(), srand()
#include <ctime>    // для time()



// Конструктор
Model::Model(int w, int h, int num_Snakes) : width(w), height(h), gameOver(false)
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
    
    
    for (int i = 0; i < num_Snakes; i++)
    {
        int startX = width / (2 * (i+1));
        int startY = height / (2 * (i+1));
        
        snakes.push_back(Snake(startX, startY));
    }
    
    // Добавляем 3 кролика
    for (int i = 0; i < num_Snakes*2; i++) 
    {
        rabbits.push_back(Rabbit(rand() % width, rand() % height)); // случайное число от 0 до width-1 и случайное число от 0 до height-1
    }
}



void Model::update() 
{

    if (isGameOver())
    {
        return;
    }

    for(int i = 0; i < snakes.size(); i++)
    {
        Snake& snake = snakes[i];

        if (!snake.isAlive) 
        {
            continue;  // пропускаем мертвых
        }

        //1. получаем голову (front() - первый элемент)
        Position head = snake.body.front();
        
        //2. вычисляем новую позицию головы
        Position newHead = head;

        switch(snake.direction) 
        {
            case 0: newHead.y--; break;  // вверх
            case 1: newHead.x++; break;  // вправо
            case 2: newHead.y++; break;  // вниз
            case 3: newHead.x--; break;  // влево
        }

// если ударяется об рамку, то игра завершается
        if (newHead.x < 0 || newHead.x >= width || newHead.y < 0 || newHead.y >= height)
        {
            snake.isAlive = false;
            continue;  // пропускаем дальнейшую обработку этой змейки
        }

// если ударяется об себя, то она "умирает"
        for (auto iter = snake.body.begin(); iter !=snake.body.end(); ++iter)
        {
            if (iter->x == newHead.x && iter->y == newHead.y)
            {
                snake.isAlive = false;  // умирает только эта змейка   
                break;        
            }
        }

// если стукается с другими змейками -> "умирает"
        for(int j = 0; j < snakes.size(); j++)
        {
            if (j == i)
            {
                continue;
            }
            else
            {
                for (auto iter = snakes[j].body.begin(); iter !=snakes[j].body.end(); ++iter)
                {
                    if (iter->x == newHead.x && iter->y == newHead.y)
                    {
                        snake.isAlive = false;  // умирает только та змейка, что налетела   
                        break;        
                    }
                }
            }
        }

        if (!snakes[i].isAlive) 
        {
            continue;  // пропускаем мертвых
        }

        
//3. добавляем новую голову
        snake.body.push_front(newHead);
        
//4. проверяем, съела ли змейка кролика (только у живых кроликов)
        bool ateRabbit = false;

        // & чтобы можно было менять значение переменной
        // для каждого элемента rabbit в контейнере (список) rabbits
        for (auto& rabbit : rabbits) 
        {  
            if (rabbit.isAlive && rabbit.pos.x == newHead.x && rabbit.pos.y == newHead.y) 
            {
                rabbit.isAlive = false; // кролик становится невидимым
                ateRabbit = true;
                break;
            }
        }
        
        // 5. если не съела - удаляем хвост, иначе - оставляем (змейка растет)
        if (!ateRabbit) 
        {
            snake.body.pop_back();
        }
        else 
        {
            // Создаем нового живого кролика
            rabbits.push_back(Rabbit(rand() % width, rand() % height));
        }
        /*
        auto - Компилятор сам определяет тип переменной. Вместо длинного std::list<Position>::iterator
        rabbits - список ВСЕХ кроликов, и при увеличении итератора, мы переходим к след кролику, у кот свои координаты (x, y)
        */
    }

    int count = 0;

    for(auto& snake : snakes)
    {
        if(snake.isAlive)
        {
            count++;
        }
    }

    if(count == 0)
    {
        gameOver = true;
    }

}


// просто возвращаем данные
const std::vector<Snake>& Model::getSnakes() const 
{
    return snakes;
}

const std::list<Rabbit>& Model::getRabbits() const 
{
    return rabbits;
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
    snakes[id_snake].direction = dir;
}

int Model::getDirection(int id_snake) const 
{ 
    return snakes[id_snake].direction; 
}

bool Model::isGameOver() const
{
    return gameOver;
}