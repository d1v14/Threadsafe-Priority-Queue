#include "Writer.h"
#include "MessageQueueWithMyHeap.h"
#include "MessageQueueWithSTL.h"
#include <string>

template<typename QueueType>
Writer<QueueType>::Writer(int id) : id(id){
}

/*
* ћетод записывающий сообщени€ в очередь.
* ѕринимаем ссылку на объект очереди.
*  аждый шаг цикла формируем сообщение со случайным приоритетом от 0 до 10
* ≈сли возвращаемое после записи значение соответствует состо€нию HWM или STOPPED, сообщение пропускаетс€.
*/
template<typename QueueType>
void Writer<QueueType>::run(QueueType& que)
{
    for (int i = 0; i < 20; i++) {
        int priority = rand() % 10;
        std::string base = "Message from writer " + std::to_string(id) + " with priority " + std::to_string(priority);
        auto ret = que.put(base, priority);
        switch (ret)
        {
        case RetCodes::HWM:
            std::cout << "[WRITER " << id << "] Queue on HWM, message sendind aborted" << std::endl;
            break;
        case RetCodes::STOPPED:
            std::cout << "[WRITER " << id << "] Queue stopped, message sendind aborted" << std::endl;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}


