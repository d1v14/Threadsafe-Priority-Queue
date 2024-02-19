#pragma once
#include "RetCodes.h"
#include "StateVariables.h"
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>

template<typename MessageType>
class MessageQueueWithSTL {
public:
    /*
    * ”дал€ем конктруктор по умолчанию
    */
    MessageQueueWithSTL() = delete;

    /*
    * ”дал€ем все способы копировани€ объекта очереди.
    */
    MessageQueueWithSTL& operator=(const MessageQueueWithSTL&) = delete;

    MessageQueueWithSTL& operator=(const MessageQueueWithSTL&&) = delete;

    MessageQueueWithSTL(const MessageQueueWithSTL&) = delete;

    MessageQueueWithSTL(const MessageQueueWithSTL&&) = delete;

    /*
    * «адаем собственный единственный конструктор, описывающий размер очереди и уровни HWM LWM
    */
    MessageQueueWithSTL(int size, int lwm, int hwm);

    ~MessageQueueWithSTL() = default;
        
    /*
    * ћетод добавлени€ сообщени€ в очередь.
    * ѕередаем сообщение и его приоритет
    * ¬озвращаем код, соответствующий результату добавлени€.
    */
    RetCodes put(MessageType msg, int priority);

    /*
    * ћетод чтени€ сообщени€ из очереди
    * ѕередаем ссылку на объект типа сообщени€,куда необходимо будет записать сообщение.
    * ¬озвращаем код, соответствующий результату чтени€ из очереди.
    */
    RetCodes get(MessageType& msg);

    /*
    * ћетод проверки достижени€ уровней HWM и LWM
    */
    void checkHWMLWM();

    /*
    * ћетод останавливающий работу очереди.
    * »змен€ет переменную состо€ни€ очереди state на StateVariable::STOPPED, тем самым методы чтени€ и добавлени€ не смогут воздействовать на очередь.
    */
    void stop();

    /*
    * ћетод запускает работу очереди.
    * »змен€ет переменную состо€ни€ очереди state на StateVariable::STARTED, тем самым методы чтени€ и добавлени€ смогут воздействовать на очередь
    */
    void run();

    /*
    * ћетод проверки пустоты очереди
    */
    bool empty();

private:

    /*
    * ѕриватный класс дл€ реализации собственного компаратора дл€ приоритетной очереди из STL
    */
    class Compare
    {
    public:
        bool operator() (std::pair<MessageType, int> a, std::pair<MessageType, int> b)
        {
            /*
            * ѕолучаем две пары из очереди.
            * ¬озвращаем результат сравнени€ пар по второму элементу(приоритету)
            */
            return a.second < b.second;
        }
    };
    /*
    * приоритетна€ очередь из STL на векторе с собственным компаратором, хран€ща€ пары (сообщение, приоритет)
    */
    std::priority_queue<std::pair<MessageType, int>, std::vector<std::pair<MessageType, int>>, Compare> pq;
    int sizeOfQueue;
    /*
    * ”ровни HWM и LWM
    */
    int HWMLevel;
    int LWMLevel;

    int elementCount;
    /*
    * ћьютекс очереди дл€ обеспечени€ потокобезопасности дл€ записи и чтени€ из очереди.
    */
    std::mutex mtx;
    /*
    * ѕримитив синхронизации
    */
    std::condition_variable condition;
    /*
    * ѕеременна€ состо€ни€ очереди
    */
    StateVariables state;
};