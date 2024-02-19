#pragma once
#include "RetCodes.h"
#include "StateVariables.h"
#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>

/*
    Шаблонный класс приоритетной очереди реализованный на бинарной куче, хранящийся в конетейнере std::vector. 
    Способ хранения бинарной кучи: в элемента вектора с индексом n два потомка, которые хранятся в индексах 2n+1 и 2n+2, корневой элемент с индексом 0
    Данный способ позволяет быстро сортировать элементы после вставки за O(logN)

*/

template<typename MessageType>
class MessageQueueWithMyHeap {
public:
    /*
    * Удаляем конктруктор по умолчанию
    */
    MessageQueueWithMyHeap() = delete;

    /*
    * Удаляем все способы копирования объекта очереди
    */
    MessageQueueWithMyHeap& operator=(const MessageQueueWithMyHeap&) = delete;

    MessageQueueWithMyHeap& operator=(const MessageQueueWithMyHeap&&) = delete;

    MessageQueueWithMyHeap(const MessageQueueWithMyHeap&) = delete;

    MessageQueueWithMyHeap(const MessageQueueWithMyHeap&&) = delete;

    MessageQueueWithMyHeap(int size, int lwm, int hwm);

    ~MessageQueueWithMyHeap() = default;

    /*
    * Метод сортировки бинарного дерева по второму элементы пары (приоритету)
    */
    void sortHeap(int parent);

    /*
    * Метод проверки достижения уровней HWM и LWM.
    */
    void checkHWMLWM();

    /*
    * Метод добавления сообщения в очередь.
    * Передаем сообщение и его приоритет
    * Возвращаем код, соответствующий результату добавления.
    */
    RetCodes put(MessageType msg, int priority);

    /*
    * Метод чтения сообщения из очереди 
    * Передаем ссылку на объект типа сообщения,куда необходимо будет записать сообщение.
    * Возвращаем код, соответствующий результату чтения из очереди. 
    */
    RetCodes get(MessageType& msg);

    /*
    * Метод останавливающий работу очереди.
    * Изменяет переменную состояния очереди state на StateVariable::STOPPED, тем самым методы чтения и добавления не смогут воздействовать на очередь.
    */
    void stop();
    /*
    * Метод запускает работу очереди.
    * Изменяет переменную состояния очереди state на StateVariable::STARTED, тем самым методы чтения и добавления смогут воздействовать на очередь
    */
    void run();
    /* 
    * Метод проверки пустоты очереди
    */
    bool empty();

private:
    /*
    * std::vector, хранящий бинарную кучу из пар (сообщение, приоритет)
    */
    std::vector<std::pair<MessageType, int>> heapQueue;

    /*
    * Мьютекс очереди для обеспечения потокобезопасности для записи и чтения из очереди.
    */
    std::mutex mtx;

    /*
    * Примитив синхронизации
    */
    std::condition_variable condition;
    /*
    * Уровни HWM и LWM
    */
    int HWMLevel;
    int LWMLevel;
    int elementCount;
    int sizeOfQueue;
    /*
    * Переменная состояния очеред
    */
    StateVariables state;
};
