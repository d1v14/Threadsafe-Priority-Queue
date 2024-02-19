
#include <string>
#include <thread>
#include <future>
#include "MessageQueueWithMyHeap.h"
#include "MessageQueueWithSTL.h"
#include "Writer.h"
#include "Reader.h"
#include "MessageQueueWithMyHeapRealisation.h"
#include "MessageQueueWithSTLRealisation.h"
#include "WriterRealisation.h"
#include "ReaderRealisation.h"

int main()
{
    //Создаем очередь с типом сообщений string
    MessageQueueWithMyHeap<std::string> que(10,2,8);

    //Создаем объекты читателей и писателей
    Writer<MessageQueueWithMyHeap<std::string>> writer1(1);
    Writer<MessageQueueWithMyHeap<std::string>> writer2(2);
    Reader<MessageQueueWithMyHeap<std::string>> reader1(1);
    //Запускаем очередь
    que.run();

    //Запускаем запись и чтение в разных потоках через std::async
    auto future1 = std::async(std::launch::async, [&writer1, &que] {writer1.run(que); });
    auto future2 = std::async(std::launch::async, [&writer2, &que] {writer2.run(que); });

    auto future3 = std::async(std::launch::async, [&reader1, &que] {reader1.run(que); });


    //Либо напрямую создаем потоки и джоиним до окончания операций чтения и записи

    /*std::thread th1([&writer1, &que] {writer1.run(que); });
    std::thread th2([&writer2, &que] {writer2.run(que); });

    std::thread th3([&reader1, &que] {reader1.run(que); });

    th1.join();
    th2.join();
    th3.join();*/


    //Блок кода, соответствующий тем же действиям, что выше, но с очередью, реализованой на приоритетной очереди из STL

   /* MessageQueueWithSTL<std::string> que1(10,2,8);
    Writer<MessageQueueWithSTL<std::string>> writer1(1);
    Writer<MessageQueueWithSTL<std::string>> writer2(2);
    Reader<MessageQueueWithSTL<std::string>> reader1(1);

    que1.run();

    auto future1 = std::async(std::launch::async, [&writer1, &que1] {writer1.run(que1); });
    auto future2 = std::async(std::launch::async, [&writer2, &que1] {writer2.run(que1); });

    auto future3 = std::async(std::launch::async, [&reader1, &que1] {reader1.run(que1); });*/

    return 0;
}


