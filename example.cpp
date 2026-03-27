#include <iostream>
#include <string>
 
class Person 
{
private:
    std::string name;
    unsigned age;
public:
    void print() 
    {
        std::cout << "Name: " << name << "\tAge: " << age << std::endl;
    }
    Person(std::string p_name, unsigned p_age)
    { 
        name = p_name; 
        age = p_age;
    }
};
int main()
{
    Person tom{"Tom", 38};
    // функция print общедоступная
    tom.print();    // Name: Tom   Age: 22
 
    // поля name и age вне класса недоступны
    // tom.name = "";  
    // tom.age = 1001;
}