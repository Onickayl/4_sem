#ifndef VIEW_H
#define VIEW_H

#include "model.h"
#include <string>

class View 
{
private:
    std::string buffer;  // буфер для устранения мигания
    // Рисуем в память (в строку buffer) — это быстро и незаметно. Выводим всё сразу одним куском
    int width;
    int height;
    
    // ANSI escape последовательности
    void clearScreen();
    void gotoxy(int x, int y);
    void hideCursor();
    void showCursor();
    void setColor(int color);
    void resetColor();
    
public:
    View();     // конструктор — вызывается при создании объекта        
    ~View();    // деструктор — вызывается при уничтожении объекта (вернет курсор обратно)
    
    // Отрисовка всего
    void showRules();
    void render(const Model& model);
    
    // Проверка нажатия клавиши (для Controller)
    bool keyPressed();
    int getKey();
};

#endif








