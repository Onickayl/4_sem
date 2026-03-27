#ifndef MODEL_H
#define MODEL_H

#include <list>

// Простая структура для координат (как struct в C)
// Только координаты (используется змейкой)
struct Position 
{
    int x, y;
    
    // Конструктор - это новая штука в C++
    // Позволяет писать Position p(5, 3) вместо p.x=5; p.y=3;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};
/*
Position p1;           // x=0, y=0
Position p2(5, 3);     // x=5, y=3
Position p3(10);       // x=10, y=0
*/

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
    std::list<Position> snake;   // тело змейки (голова в начале)
    std::list<Rabbit> rabbits; // кролики
    int direction;  // 0=вверх, 1=вправо, 2=вниз, 3=влево
    
public:
    // Конструктор (вызывается при создании объекта)
    Model(int w, int h);
    
    // Обновление состояния (один шаг игры)
    void update();
    
    // Изменение направления
    void setDirection(int dir);
    int getDirection() const;
    
    // Геттеры (методы для получения данных)
    const std::list<Position>& getSnake() const;
    const std::list<Rabbit>& getRabbits() const;
    int getWidth() const;
    int getHeight() const;
    // const в конце означает - "этот метод НЕ изменяет объект"
    // std::list<Position>& аналогично List* getSnake()
};

#endif