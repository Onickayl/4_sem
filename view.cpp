#include "view.h"
#include <iostream>
#include <unistd.h>
#include <termios.h>    //структура, хранящая настройки терминала (скорость, режимы ввода, и т.д.)
#include <sys/time.h>

// произвольные уникальные коды, которые мы сами придумали для обозначения стрелок, 
// чтобы они не совпадали с реальными кодами клавиш (0-255 для обычных клавиш)
#define KEY_UP    1000
#define KEY_DOWN  1001
#define KEY_RIGHT 1002
#define KEY_LEFT  1003

#define RED     31
#define GREEN   32
#define YELLOW  33
#define WHITE   37

// статическая переменная, которая хранит исходные настройки терминала
// original_tio сохраняет исходные настройки, чтобы мы могли их восстановить
static struct termios original_tio;

View::View() 
{
    struct termios new_tio;                     //копия исходных настроек, которую мы будем изменять для игры

    tcgetattr(STDIN_FILENO, &original_tio);     // функция, которая читает текущие настройки терминала и сохраняет их в структуру

    new_tio = original_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);        // Клавиши приходят сразу, не показываются 
    // выключаем ICANON и ECHO, остальные флаги оставляем как были
    // ICANON ждет Enter, можно редактировать строку. ECHO показывает нажатые клавиши на экране
    new_tio.c_cc[VMIN] = 0;                     // не ждать, читать сразу, если есть символы
    // c_cc — это массив управляющих символов. VMIN — это индекс в этом массиве, который задает минимальное количество символов для чтения.
    new_tio.c_cc[VTIME] = 0;                    // без таймаута
    // таймаут в десятых долях секунды

    // Применяем новые настройки к терминалу
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    // TCSANOW - применить изменения немедленно
    
    hideCursor();
    clearScreen();
}

View::~View() 
{
    showCursor();
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void View::clearScreen() 
{
    std::cout << "\033[2J\033[H";
    // 2J - очищает весь экран
    // H перемещает курсор в левый верхний угол (0,0)
}

void View::gotoxy(int x, int y) 
{
    std::cout << "\033[" << y << ";" << x << "H";
}

void View::hideCursor() 
{
    std::cout << "\033[?25l";
    //?25 — код, отвечающий за видимость курсора
    // l — low (установить в 0, выключить)
}

void View::showCursor() 
{
    std::cout << "\033[?25h";
    // h — high (установить в 1, включить)
}

void View::setColor(int color) 
{
    std::cout << "\033[" << color << "m";
    // все последующие символы будут печататься этим цветом
}

void View::resetColor() 
{
    std::cout << "\033[0m";
    // сбросить все атрибуты (цвет, жирность, и т.д.)
}

// функция проверки, нажата ли клавиша, без ожидания
bool View::keyPressed() 
{
    struct timeval tv = {0, 0};     //  ждать 0 секунд - проверить и сразу вернуться {сек, микросек}
    fd_set fds;                     // набор файловых дескрипторов (файлов, сокетов, терминалов)
    FD_ZERO(&fds);                  // очищает набор — убирает все дескрипторы
    FD_SET(STDIN_FILENO, &fds);                             // добавляет стандартный ввод (клавиатуру) в набор
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);        // проверяет, есть ли данные для чтения
    // (максимальный_дескриптор + 1, набор_для_чтения, набор_для_записи, набор_для_ошибок, таймаут)
    
    return FD_ISSET(STDIN_FILENO, &fds);        //проверяет, есть ли именно клавиатура в наборе дескрипторов, готовых к чтению (1/0)
}

int View::getKey() 
{
    // читаем 1 символ с клавиатуры
    char ch;
    if (read(STDIN_FILENO, &ch, 1) != 1) return -1;
    
    // Обработка escape-последовательностей (стрелки)
    /*когда жамкаем стрелки на клаве, терминал отправляет символы:
        ESC + [ + буква
        27  + [ + A (для вверх)
        27  + [ + B (для вниз)
        27  + [ + C (для вправо)
        27  + [ + D (для влево)
    */

    if (ch == 27) // код клавиши ESC
    {  
        char seq[2];

        // Читаем следующий символ '['
        if (read(STDIN_FILENO, &seq[0], 1) != 1) 
        {
            return 27;
        }

        // Читаем символ стрелки 'A, B, C, D'
        if (read(STDIN_FILENO, &seq[1], 1) != 1) 
        {
            return 27;
        }

        // Проверяем, что второй символ — это [
        if (seq[0] == '[') 
        {
            switch(seq[1]) 
            {
                case 'A': return KEY_UP;    // 1000
                case 'B': return KEY_DOWN;  // 1001
                case 'C': return KEY_RIGHT; // 1002
                case 'D': return KEY_LEFT;  // 1003
            }
        }

        return 27;      // если не распознали, возвращаем ESC
    }
    
    return ch;      // если первый символ не ESC, это обычная клавиша — возвращаем её код
}

void View::render(const Model& model) 
{
    buffer.clear();     //Очищаем строку-буфер
    
    int width = model.getWidth();
    int height = model.getHeight();
    
    // Рамка
    for (int y = 0; y <= height + 1; y++) 
    {
        for (int x = 0; x <= width + 1; x++) 
        {
            if (y == 0 || y == height + 1) 
            {
                buffer += "-";
            } 
            else if (x == 0 || x == width + 1) 
            {
                buffer += "|";
            } 
            else 
            {
                buffer += " ";      // внутри поля
            }
        }

        buffer += "\n";     // конец строки
    }
    

    // Кролики
    for (const auto& rabbit : model.getRabbits()) 
    {
        if (rabbit.isAlive) 
        {
            int x = rabbit.pos.x + 1;
            int y = rabbit.pos.y + 1;

            buffer[(y * (width + 3)) + x] = 'X';
        }
    }
    

    // Змейка
    int id_snake = 0;
    for (auto& snake : model.getSnakes())
    {

        if (!snake.isAlive) 
        {
            continue;
        }
        
        bool first = true;        
        
        for (const auto& segment : snake.body) 
        {
            int x = segment.x + 1;
            int y = segment.y + 1;
            
            if (first) 
            {
                int dir = model.getDirection(id_snake);
                switch(dir) 
                {
                    case 0: buffer[(y * (width + 3)) + x] = '^'; break;
                    case 1: buffer[(y * (width + 3)) + x] = '>'; break;
                    case 2: buffer[(y * (width + 3)) + x] = 'v'; break;
                    case 3: buffer[(y * (width + 3)) + x] = '<'; break;
                }

                first = false;
            } 
            else 
            {
                buffer[(y * (width + 3)) + x] = 'o';
            }
        }

        id_snake++;
    }

    
    gotoxy(0, 0);
    std::cout << buffer;

    // очищаем всё, что ниже рамки
    std::cout << "\033[J";  // очистить от курсора до конца экрана
    std::cout.flush();
}

void View::showRules() 
{
    clearScreen();
    
    setColor(YELLOW);
    
    std::cout << "==========================================\n";
    std::cout << "             SNAKE GAME\n";
    std::cout << "==========================================\n\n";
    
    setColor(GREEN);
    std::cout << "Управление:\n";
    resetColor();
    
    std::cout << "  \033[32mСтрелки\033[0m или \033[32mWASD\033[0m - движение змейки\n";
    std::cout << "  \033[33m  ↑\033[0m или \033[33mW\033[0m - вверх\n";
    std::cout << "  \033[33m  ↓\033[0m или \033[33mS\033[0m - вниз\n";
    std::cout << "  \033[33m  ←\033[0m или \033[33mA\033[0m - влево\n";
    std::cout << "  \033[33m  →\033[0m или \033[33mD\033[0m - вправо\n\n";
    
    std::cout << "  \033[32mP\033[0m - пауза\n";
    std::cout << "  \033[31mQ\033[0m - выход из игры\n\n";
    
    std::cout << "\033[32mПравила:\033[0m\n";
    std::cout << "  • Змейка движется по полю\n";
    std::cout << "  • Ешьте \033[31mX\033[0m (кроликов) чтобы расти\n";
    std::cout << "  • Не врезайтесь в стены и в свой хвост\n";
    std::cout << "  • Чем длиннее змейка, тем сложнее\n\n";
    
    setColor(YELLOW);
    std::cout << "==========================================\n";
    std::cout << "      Нажмите любую клавишу для старта\n";
    std::cout << "==========================================\n";
    resetColor();

    // Ждем любую клавишу
    while (!keyPressed()) 
    {
        usleep(10000);
    }

    getKey();  // съедаем клавишу
}


void View::showGameOver()
{
    clearScreen();
    
    setColor(RED);
    
    std::cout << "==========================================\n";
    std::cout << "             GAME OVER\n";
    std::cout << "==========================================\n\n";

    resetColor();

    std::cout << "please press q/Q\n";
    std::cout.flush();   // сразу на экран
    

}

