#include "MessageQueueWithSTL.h"

template <typename MessageType>
MessageQueueWithSTL<MessageType>::MessageQueueWithSTL(int size, int lwm, int hwm) {
    sizeOfQueue = size;
    HWMLevel = hwm;
    LWMLevel = lwm;
    elementCount = 0;
    state = StateVariables::STOPPED;
}

/*
* Метод добавления элемента в очередь.
* Проверяем состояние очереди. Если очередь STOPPED или HWM, добавления не происходит.
* В ином случае добавляем элемент.
*/
template <typename MessageType>
RetCodes MessageQueueWithSTL<MessageType>::put(MessageType msg, int priority) {
    std::unique_lock<std::mutex> lck(mtx);
    switch (state) {
    case StateVariables::HWM:
        lck.unlock();
        condition.notify_one();
        return RetCodes::HWM;
    case StateVariables::STOPPED:
        lck.unlock();
        return RetCodes::STOPPED;

    default:
        std::pair<MessageType, int> pair = std::make_pair(msg, priority);
        pq.push(std::move(pair));
        elementCount = pq.size();
        checkHWMLWM();
        lck.unlock();
        condition.notify_one();
        return RetCodes::OK;
    }
}

/*
* Блокирующий метод получения сообщения из очереди.
* Если очередь в состоянии STOPPED чтение не происходит.
* Иначе метод выполянет чтение.
* Если очередь пуста, происходит блокировка пока в очереди не появится сообщение и примит синхронизации не будет извещен о том, что в очередь пришло сообщение
*/
template <typename MessageType>
RetCodes MessageQueueWithSTL<MessageType>::get(MessageType& msg) {
    std::unique_lock<std::mutex> lck(mtx);
    switch (state)
    {
    case StateVariables::STOPPED:
        lck.unlock();
        return RetCodes::STOPPED;

    default:
        while (pq.empty()) {
            condition.wait(lck);
        }
        msg = pq.top().first;
        pq.pop();
        elementCount = pq.size();
        checkHWMLWM();
        return RetCodes::OK;
    }
}

/*
* Метод проверки достижения уровня HWM и LWM.
* Если очередь достигла уровня HWM, запись в нее прекрашается пока не будет достигнут LWM.
* Если очередь заполнена до уровня HWM, статус очереди устанавливается в HWM и снимется только по достижению уровня LWM.
* При достижении уровня LWM статус очереди устанавливается в LWM.
*/
template <typename MessageType>
void MessageQueueWithSTL<MessageType>::checkHWMLWM() {
    int size = pq.size();
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

template <typename MessageType>
bool MessageQueueWithSTL<MessageType>::empty() {
    return pq.empty();
}


template<typename MessageType>
void MessageQueueWithSTL<MessageType>::stop() {
    state = StateVariables::STOPPED;
}

template<typename Messagetype>
void MessageQueueWithSTL<Messagetype>::run() {
    state = StateVariables::STARTED;
}