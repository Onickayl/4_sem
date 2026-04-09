#ifndef MODEL_H
#define MODEL_H

#include <list>
#include <vector>


// Только координаты
struct Position 
{
    int x, y;

    // конструктор позволяет писать Position p(5, 3) вместо p.x=5; p.y=3;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};
/*
Position p1;           // x=0, y=0
Position p2(5, 3);     // x=5, y=3
Position p3(10);       // x=10, y=0
*/

// змейка с дополнительными свойствами
struct Snake 
{
    std::list<Position> body;       // список позиций сегментов змейки
    bool isAlive;                   // флаг только для змеек
    int direction;                  // 0=вверх, 1=вправо, 2=вниз, 3=влево
    int color;                      // цвет змейки 
    int bot_type;                   // 0 - не бот 1 - тупой 2 - осторожный
    
    Snake(int x, int y) : isAlive(true), direction(1), color(32), bot_type(0)
    {
        // Создаем змейку из 3 сегментов
        body.push_back(Position(x, y));         // голова
        body.push_back(Position(x - 1, y));     // тело
        body.push_back(Position(x - 2, y));     // хвост
    }
};

// Кролик с дополнительными свойствами
struct Rabbit 
{
    Position pos;
    bool isAlive;  // флаг только для кроликов
    
    Rabbit(int x, int y) : pos(x, y), isAlive(true) {}
};


class Model 
{
private:
    int width;      // ширина поля
    int height;     // высота поля

    std::vector<Snake> snakes;           // вектор змеек
    std::list<Rabbit> rabbits;           // кролики
    bool gameOver;                       // флаг завершения игры
    
public:

    // Конструктор (вызывается при создании объекта)
    Model(int w, int h, int num_Snakes = 1);
    
    // Обновление состояния (один шаг игры)
    void update();

    // проверки
    bool check_wall(Position newHead, int width, int height);
    bool check_self(Snake &snake, Position newHead);
    bool check_other(std::vector<Snake> &snakes, int index, Position newHead);
    bool eat_rabbit(Position newHead, std::list<Rabbit> &rabbits);
    bool is_safe(Position head, int index, std::vector<Snake> &snakes, int width, int height, int dir);
    
    // Изменение направления
    void setDirection(int id_snake, int dir); 
    int getDirection(int id_snake) const;
    void bot_dir(Snake &snake, Position head);
    
    // Геттеры (методы для получения данных)
    const std::vector<Snake>& getSnakes() const;
    const std::list<Rabbit>& getRabbits() const;
    int getWidth() const;
    int getHeight() const;
    bool isGameOver() const;
    // const в конце означает - "этот метод НЕ изменяет объект"
    // std::list<Position>& аналогично List* getSnake()
};

#endif