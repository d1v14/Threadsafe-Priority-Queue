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
* ����� ���������� �������� � �������.
* ��������� ��������� �������. ���� ������� STOPPED ��� HWM, ���������� �� ����������.
* � ���� ������ ��������� �������.
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
* ����������� ����� ��������� ��������� �� �������.
* ���� ������� � ��������� STOPPED ������ �� ����������.
* ����� ����� ��������� ������.
* ���� ������� �����, ���������� ���������� ���� � ������� �� �������� ��������� � ������ ������������� �� ����� ������� � ���, ��� � ������� ������ ���������
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
* ����� �������� ���������� ������ HWM � LWM.
* ���� ������� �������� ������ HWM, ������ � ��� ������������ ���� �� ����� ��������� LWM.
* ���� ������� ��������� �� ������ HWM, ������ ������� ��������������� � HWM � �������� ������ �� ���������� ������ LWM.
* ��� ���������� ������ LWM ������ ������� ��������������� � LWM.
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
            * ���� �� �� ��������� �� ������ HWM ��� LWM, �� �� ����� ��� ��������� ������� HWM, ��������� ������ ���������, ����� ����������� ������ ������ �� �������, ���� �� ����� ���������  LWM
            */
            ;
        if (state == StateVariables::OK)
            /*
           * ���� �� �� ��������� �� ������ HWM ��� LWM � �� ����� �� ��� ��������� �� ���� �� �������, ��������� ������ ���������, ����� ����������� � ������ � ������.
           */
            ;
        if (state == StateVariables::LWM)
            /*
           * ���� �� �� ��������� �� ������ HWM ��� LWM, �� �� ����� ��� ��������� ������� LWM, ������������� ��������� StateVariables::OK, ����� ����������� � ������ � ������.
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