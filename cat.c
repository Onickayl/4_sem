#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cat.h"    // включение заголовочного файла


// определения функций

cat* cat_create()
{
    //создаём кошку с именем busya

    cat* kitten = (cat*)malloc(sizeof(cat));

    if (kitten == NULL) 
    {
        return NULL;
    }
    
    kitten->name = (char*)malloc(strlen("Busya") + 1);

    if (kitten->name == NULL) 
    {
        free(kitten);
        return NULL;
    }
    
    strcpy(kitten->name, "Busya");
    return kitten;
} 
    
cat* cat_create_with_name(const char* name)
{
    //создаём кошку с другим именем

    cat* kitten = (cat*)malloc(sizeof(cat));

    if (kitten == NULL) 
    {
        return NULL;
    }
    
    kitten->name = (char*)malloc(strlen(name) + 1);

    if (kitten->name == NULL) 
    {
        free(kitten);
        return NULL;
    }
    
    strcpy(kitten->name, name);
    return kitten;
}

void cat_voice(cat* kitten)
{
    if (kitten)  
    {
        printf("мяу! from %s\n", kitten->name);
    }
}

void cat_destroy(cat* kitten)
{
    //уничтожаем кошку

    if (kitten)  
    {
        free(kitten->name);
        free(kitten);
    }
}
