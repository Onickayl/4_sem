#include "model.h"
#include "view.h"
#include "controller.h"

int main() 
{
    // Создаем поле 20x20
    Model model(50, 20);
    View view;

    // Показываем правила перед началом
    view.showRules();

    
    Controller controller(model, view);
    
    // Запускаем игру
    controller.run();
    
    return 0;
}