#ifndef MODEL_H
#define MODEL_H

#include <list>


// Только координаты (используется змейкой)
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
/*struct Snake 
{
    Position body;
    bool isAlive;   // флаг только для змеек
    int direction;  // 0=вверх, 1=вправо, 2=вниз, 3=влево
    int color;      // цвет змейки 
    
    Snake(int x, int y) : body(x, y), isAlive(true) {}
};*/

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

    // НАДО СПИСОК ЗМЕЕК!!!

    std::list<Position> snake;         // тело змейки (голова в начале)
    std::list<Rabbit> rabbits;      // кролики
    int direction;  // 0=вверх, 1=вправо, 2=вниз, 3=влево
    bool gameOver;      // флаг завершения игры
    
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
    bool isGameOver();
    // const в конце означает - "этот метод НЕ изменяет объект"
    // std::list<Position>& аналогично List* getSnake()
};

#endif