#include "sfml_view.h"
#include <iostream>

SfmlView::SfmlView(int width, int height, const sf::Font& externalFont, bool silent_mode)
    : width(width), height(height), font(externalFont), silent(silent_mode)
{

    if (silent)
    {
        // в тихом режиме окно не создаём
        return;
    }

    // вычисляем оптимальный размер клетки
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float available_height = desktop.height * 0.7f;              // 70% высоты экрана
    int calculated_cell_size = static_cast<int>(available_height / (height + 2));
    
    if (calculated_cell_size < 15) 
    {
        calculated_cell_size = 15;
    }

    if (calculated_cell_size > 60) 
    {
        calculated_cell_size = 60;
    }
    
    cell_size = calculated_cell_size;

    // Создаём окно: ширина = (поле + рамка 2) * размер клетки
    int windowWidth = (width + 2) * cell_size;
    int windowHeight = (height + 2) * cell_size + 50; // +50 пикселей для статуса

    window.create(sf::VideoMode(windowWidth, windowHeight), "Snake Game");

    // ограничим fps для плавности
    window.setFramerateLimit(60);

    // используем переданный шрифт - уже загружен
    text.setFont(font);
    text.setCharacterSize(cell_size - 5);
    text.setFillColor(sf::Color::White);
    
    // проверяем, что окно создалось
    if (!window.isOpen())
    {
        std::cout << "окно не создалось" << std::endl;
    }
}

SfmlView::~SfmlView()
{
    if (window.isOpen())
    {
        window.close();
    }
}


// Отрисовка текста
void SfmlView::draw_text(int x, int y, const std::string &str, sf::Color color, int size)
{
    // используем существующий объект text вместо создания нового
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    window.draw(text);
}

// Основная отрисовка
void SfmlView::render(const Model &model)
{
    if (silent)
    {
        return;  // в тихом режиме ничего не рисуем
    }

    // проверяем, открыто ли окно перед отрисовкой
    if (!window.isOpen()) 
    {
        return;
    }

    window.clear(sf::Color::Black);


// рисуем рамку (белые клетки по краям)
    for (int y = 0; y <= height + 1; y++)
    {
        for (int x = 0; x <= width + 1; x++)
        {
            if (y == 0 || y == height + 1 || x == 0 || x == width + 1)
            {
                 draw_text(x * cell_size, y * cell_size, "#", sf::Color::White, cell_size);
            }
        }
    }

// рисуем кроликов (красные клетки)
    for (const auto &rabbit : model.getRabbits())
    {
        if (rabbit.isAlive)
        {
            draw_text((rabbit.pos.x + 1) * cell_size, (rabbit.pos.y + 1) * cell_size, "X", sf::Color::Red, cell_size);
        }
    }

// рисуем яблоки (жёлтые клетки)
    for (const auto &apple : model.getApples())
    {
        if (apple.isAlive)
        {
            draw_text((apple.pos.x + 1) * cell_size, (apple.pos.y + 1) * cell_size, "@", sf::Color::Yellow, cell_size);
        }
    }

// рисуем снежинки (голубые клетки)
    for (const auto &snowflake : model.getSnowflakes())
    {
        if (snowflake.isAlive)
        {
            draw_text((snowflake.pos.x + 1) * cell_size, (snowflake.pos.y + 1) * cell_size, "*", sf::Color::Cyan, cell_size);
        }
    }

// рисуем змеек
    int id_snake = 0;
    for (const auto &snake : model.getSnakes())
    {
        if (!snake.isAlive)
        {
            id_snake++;
            continue;
        }


        // определяем цвет змейки
        sf::Color snake_color;
        switch (snake.color)
        {
        case 32:
            snake_color = sf::Color::Green;
            break;
        case 33:
            snake_color = sf::Color::Yellow;
            break;
        case 35:
            snake_color = sf::Color::Magenta;
            break;
        case 36:
            snake_color = sf::Color::Cyan;
            break;        
        default:
            snake_color = sf::Color::White;
            break;
        }

        bool head = true;
        for (const auto &segment : snake.body)
        {
            std::string sym;
            sf::Color color = snake_color;

            // голова рисуется ярче - чтобы визуально отличать голову от тела
            if (head)
            {
                // Определяем символ головы по направлению
                // 0: Вверх, 1: Вправо, 2: Вниз, 3: Влево 
                switch (snake.direction)
                {
                case 0:
                    sym = "^";
                    break;
                case 1:
                    sym = ">";
                    break;
                case 2:
                    sym = "v";
                    break;
                case 3:
                    sym = "<";
                    break;

                default:
                    break;
                }
                head = false;
            }
            else
            {
                sym = "0";
                //color.a = 180; // Немного прозрачности для тела
            }

            draw_text((segment.x + 1) * cell_size, (segment.y + 1) * cell_size, sym, color, cell_size);
        }
        id_snake++;
    }

// статистика внизу
    draw_text(10, (height + 2) * cell_size + 10,
             "X: " + std::to_string(model.getEaten_rabbits()) +
                 "  @: " + std::to_string(model.getEaten_apples()) +
                 "  *: " + std::to_string(model.getEaten_snowflakes()),
             sf::Color::White, 20);

    window.display();

    // добавим небольшую задержку для синхронизации
    sf::sleep(sf::milliseconds(16));  // ~60 FPS
}

// Проверка нажатия клавиши (неблокирующая)
bool SfmlView::keyPressed()
{  
    // проверяем, открыто ли окно
    if (!window.isOpen()) return true;
    
    sf::Event event;
    while (window.pollEvent(event)) // Забираем все события из очереди
    {
        if (event.type == sf::Event::Closed) // если нажат крестик окна
        {
            window.close();
            return true; // чтобы выйти
        }
        if (event.type == sf::Event::KeyPressed) // если нажата любая клавиша
        {
            return true;
        }
    }

    return false;   // нет событий - нет нажатой клавиши
}

/*
Блокирующая функция — программа останавливается и ждёт, пока что-то произойдёт 
    — игра останавливается и ждёт, пока игрок нажмёт клавишу (змейка не двигается!)
Неблокирующая функция — программа проверяет и сразу идёт дальше, даже если ничего не произошло 
    — игра постоянно спрашивает: "Есть нажатая клавиша? Если есть — обработай. Если нет — просто двигай змейку дальше"
*/


// получение кода клавиши (без защиты от повторов)
int SfmlView::getKey()
{
    if (!window.isOpen()) return 'q';

    // сначала обрабатываем системные события (чтобы окно не зависло)
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
            return 'q';
        }
    }

    // теперь опрашиваем состояние клавиш напрямую - мгновенно
    
    // управление первой змейкой (стрелки)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) return 1000;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) return 1001;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) return 1002;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) return 1003;
    
    // управление второй змейкой (wasd)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) return 'w';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) return 'a';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) return 's';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) return 'd';
    
    // дополнительные клавиши
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) return 'p';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) return 'q';
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) return 'q';

    return -1;  // ничего не нажато
}



// экран Game Over
void SfmlView::showGameOver()
{

    if (silent) 
    {
        return;  // в тихом режиме не показываем
    }

    // проверяем, открыто ли окно
    if (!window.isOpen()) return;
    
    window.clear(sf::Color::Black);

    draw_text(100, 200, "GAME OVER", sf::Color::Red, 40);
    draw_text(100, 250, "Press q/Q to quit", sf::Color::White, 20);

    window.display();

    // Ждём q/Q
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed && 
                (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape))
            {
                window.close();
                return;
            }
        }
    }
}