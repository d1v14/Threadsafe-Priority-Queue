#pragma once
#include <random>
#include <thread>
#include "RetCodes.h"
#include <iostream>
/*
* Класс писателя в приоритетную очередь
*/
template<typename QueueType>
class Writer
{
public:

    /*
    * Удаляем конструктор по умолчанию и создаем свой с защитой от неявных преобразований
    */
    Writer() = delete;
    explicit Writer(int id);
    /*
    * Метод, запускающий запись в приоритетную очередь
    */
    void run(QueueType &que);

private:
    /*
    * Идентификатор писателя
    */
    int id;
};

