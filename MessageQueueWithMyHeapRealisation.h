#include "MessageQueueWithMyHeap.h"

template<typename MessageType>
MessageQueueWithMyHeap<MessageType>::MessageQueueWithMyHeap(int size, int lwm, int hwm) {
    //Выделяем память заданного размера под вектор, хранящий кучу.
    heapQueue.reserve(size);
    sizeOfQueue = size;
    HWMLevel = hwm;
    LWMLevel = lwm;
    elementCount = 0;
    state = StateVariables::STOPPED;
}

/*
*  Метод сортировки кучи.
*  Получаем родительский узел, ищем 2 узла потомка, сортируем их, чтобы самый большой узел находился в родительском узле.
*/
template<typename MessageType>
void MessageQueueWithMyHeap<MessageType>::sortHeap(int parent) {
    int size = heapQueue.size();

    int largest = parent;
    int lChild = 2 * parent + 1;
    int rChild = 2 * parent + 2;
    if (lChild < size && heapQueue[lChild].second > heapQueue[largest].second)
        largest = lChild;
    if (rChild < size && heapQueue[rChild].second > heapQueue[largest].second)
        largest = rChild;

    if (largest != parent) {
        std::swap(heapQueue[parent], heapQueue[largest]);
        sortHeap(largest);
    }
}

/*
* Метод добавления элемента в очередь.
* Проверяем состояние очереди. Если очередь STOPPED или HWM, добавления не происходит.
* В ином случае добавляем элемент и сортируем дерево до корня.
*/
template<typename MessageType>
RetCodes MessageQueueWithMyHeap<MessageType>::put(MessageType msg, int priority) {

    std::unique_lock<std::mutex> lck(mtx);
    switch (state) {
    case StateVariables::HWM:
        condition.notify_one();
        lck.unlock();
        return RetCodes::HWM;

    case StateVariables::STOPPED:
        lck.unlock();
        return RetCodes::STOPPED;

    default:
        int size = heapQueue.size();
        //Создаем пару из сообщения и приоритета
        std::pair<MessageType, int> pair = std::make_pair(msg, priority);

        /*
        * Если куча пустая, добавляем пару в корень. Если куча не пуста, кладем элемент в конец вектора, а после сортируем дерево начиная с родительского узла для данного элемента.
        */
        if (size == 0) {
            heapQueue.push_back(std::move(pair));
        }
        else {
            //Добавляем и сортируем до корня
            heapQueue.push_back(std::move(pair));
            for (int parent = heapQueue.size() / 2 - 1; parent >= 0; parent--) {
                sortHeap(parent);
            }
        }
        elementCount = heapQueue.size();
        //Проверяем достижение уровней HWM и LWM после добавления
        checkHWMLWM();
        lck.unlock();
        //Через примитив синхронизации извещаем поток о том, что мы положили сообщение в очередь.
        condition.notify_one();
        return RetCodes::OK;
    }
}

/*
* Метод проверки достижения уровня HWM и LWM.
* Если очередь достигла уровня HWM, запись в нее прекрашается пока не будет достигнут LWM.
* Если очередь заполнена до уровня HWM, статус очереди устанавливается в HWM и снимется только по достижению уровня LWM.
* При достижении уровня LWM статус очереди устанавливается в LWM.
*/
template<typename MessageType>
void MessageQueueWithMyHeap<MessageType>::checkHWMLWM() {
    int size = heapQueue.size();
    if (size >= HWMLevel)
        state = StateVariables::HWM;
    if (size <= LWMLevel)
        state = StateVariables::LWM;
    if (size > LWMLevel && size < HWMLevel) {
        if (state == StateVariables::HWM)
            /*
            * Если мы не находимся на уровне HWM или LWM, но до этого был достигнул уровень HWM, оставляем данное состояние, чтобы происходило только чтение из очереди, пока не будет достигнут  LWM
            */
            ;
        if (state == StateVariables::OK)
            /*
           * Если мы не находимся на уровне HWM или LWM и до этого не был достигнут ни один из уровней, оставляем данное состояние, чтобы происходила и запись и чтение.
           */
            ;
        if (state == StateVariables::LWM)
            /*
           * Если мы не находимся на уровне HWM или LWM, но до этого был достигнул уровень LWM, устанавливаем состояние StateVariables::OK, чтобы происходила и запись и чтение.
           */
            state = StateVariables::OK;

    }
}

/*
* Блокирующий метод получения сообщения из очереди.
* Если очередь в состоянии STOPPED чтение не происходит.
* Иначе метод выполянет чтение.
* Если очередь пуста, происходит блокировка пока в очереди не появится сообщение и примитив синхронизации не будет извещен о том, что в очередь пришло сообщение
*/
template<typename MessageType>
RetCodes MessageQueueWithMyHeap<MessageType>::get(MessageType& msg) {

    std::unique_lock<std::mutex> lck(mtx);
    switch (state)
    {
    case StateVariables::STOPPED:
        lck.unlock();
        return RetCodes::STOPPED;

    default:
        while (heapQueue.empty()) {
            condition.wait(lck);
        }

        int size = heapQueue.size();
        msg = heapQueue[0].first;
        std::swap(heapQueue[0], heapQueue[size - 1]);

        heapQueue.pop_back();
        for (int parent = heapQueue.size() / 2 - 1; parent >= 0; parent--) {
            sortHeap(parent);
        }

        elementCount = heapQueue.size();
        checkHWMLWM();
        lck.unlock();
        return RetCodes::OK;
    }
}

template<typename MessageType>
bool MessageQueueWithMyHeap<MessageType>::empty() {
    return heapQueue.empty();
}

template<typename MessageType>
void MessageQueueWithMyHeap<MessageType>::stop() {
    state = StateVariables::STOPPED;
}

template<typename Messagetype>
void MessageQueueWithMyHeap<Messagetype>::run(){
    state = StateVariables::STARTED;
}
