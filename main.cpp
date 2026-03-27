#include "model.h"
#include "view.h"
#include "controller.h"
#include <sys/ioctl.h>
#include <unistd.h>

struct winsize w;

int main() 
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Создаем поле на весь терминал
    Model model(w.ws_col - 2, w.ws_row - 3);
    View view;

    // Показываем правила перед началом
    view.showRules();

    Controller controller(model, view);
    
    // Запускаем игру
    controller.run();
    
    return 0;
}