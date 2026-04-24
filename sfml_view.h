#ifndef SFML_VIEW_H
#define SFML_VIEW_H

#include "view.h"
#include <SFML/Graphics.hpp>

class SfmlView : public View 
{
private:
    sf::RenderWindow window;
    int cell_size;              // размер одной клетки в пикселях (например, 30)
    int width;                  // ширина поля в клетках
    int height;                 // высота поля в клетках
    const sf::Font& font;       // теперь это ссылка на внешний шрифт
    sf::Text text;
    bool silent;  

    // рисование текста
    void draw_text(int x, int y, const std::string& str, sf::Color color, int size = 20);

public:
    SfmlView(int width, int height, const sf::Font& externalFont, bool silent_mode = false);
    ~SfmlView();

    void render(const Model& model) override;
    bool keyPressed() override;
    int getKey() override;
    void showGameOver() override;
};

#endif