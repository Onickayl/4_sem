#include "model.h"
#include <cstdlib>  // для rand(), srand()
#include <ctime>    // для time()

// Конструктор
Model::Model(int w, int h) : width(w), height(h), direction(1) 
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
    
    // Создаем змейку в центре
    int centerX = width / 2;
    int centerY = height / 2;
    
    // push_back - добавить в конец списка
    snake.push_back(Position(centerX, centerY));     // голова
    snake.push_back(Position(centerX - 1, centerY)); // сегмент
    snake.push_back(Position(centerX - 2, centerY)); // хвост
    
    // Добавляем 3 кролика
    for (int i = 0; i < 3; i++) 
    {
        rabbits.push_back(Rabbit(rand() % width, rand() % height)); // случайное число от 0 до width-1 и случайное число от 0 до height-1
    }
}



void Model::update() 
{
    // 1. Получаем голову (front() - первый элемент)
    Position head = snake.front();
    
    // 2. Вычисляем новую позицию головы
    Position newHead = head;
    switch(direction) 
    {
        case 0: newHead.y--; break;  // вверх
        case 1: newHead.x++; break;  // вправо
        case 2: newHead.y++; break;  // вниз
        case 3: newHead.x--; break;  // влево
    }
    
    // 3. Добавляем новую голову
    snake.push_front(newHead);
    
    // 4. Проверяем, съела ли змейка кролика (только у живых кроликов)
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
    
    // 5. Если не съела - удаляем хвост, иначе - оставляем (змейка растет)
    if (!ateRabbit) 
    {
        snake.pop_back();
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


// просто возвращаем данные
const std::list<Position>& Model::getSnake() const 
{
    return snake;
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

void Model::setDirection(int dir) 
{
    direction = dir;
}

int Model::getDirection() const 
{ 
    return direction; 
}