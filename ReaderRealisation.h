#include "Reader.h"
#include "MessageQueueWithMyHeap.h"
#include "MessageQueueWithSTL.h"
#include <string>

template<typename QueueType>
Reader<QueueType>::Reader(int id) : id(id) {
}

/*
* Метод читающий сообщения из очереди.
* Принимаем ссылку на объект очереди.
* Пока очередь не пуста читаем сообщение.
*/
template<typename QueueType>
void Reader<QueueType>::run(QueueType& que)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    while (!que.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::string msg;
        que.get(msg);
        std::cout << msg << std::endl;
    }
}
