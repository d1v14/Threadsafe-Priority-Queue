#pragma once
#include <random>
#include <thread>
#include "RetCodes.h"
#include <iostream>

/*
* Класс читателя приоритетной очереди
*/
template<typename QueueType>
class Reader
{
public:
    /*
    * Удаляем конструктор по умолчанию и создаем свой с защитой от неявных преобразований
    */
    Reader() = delete;
    explicit Reader(int id);

    ~Reader() = default;
    /*
    * Метод, запускающий чтение из приоритетной очереди
    */
    void run(QueueType& que);

private:
    /*
    * Идентификатор читателя
    */
    int id;
};