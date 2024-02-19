#include "MessageQueueWithMyHeap.h"

template<typename MessageType>
MessageQueueWithMyHeap<MessageType>::MessageQueueWithMyHeap(int size, int lwm, int hwm) {
    //�������� ������ ��������� ������� ��� ������, �������� ����.
    heapQueue.reserve(size);
    sizeOfQueue = size;
    HWMLevel = hwm;
    LWMLevel = lwm;
    elementCount = 0;
    state = StateVariables::STOPPED;
}

/*
*  ����� ���������� ����.
*  �������� ������������ ����, ���� 2 ���� �������, ��������� ��, ����� ����� ������� ���� ��������� � ������������ ����.
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
* ����� ���������� �������� � �������.
* ��������� ��������� �������. ���� ������� STOPPED ��� HWM, ���������� �� ����������.
* � ���� ������ ��������� ������� � ��������� ������ �� �����.
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
        //������� ���� �� ��������� � ����������
        std::pair<MessageType, int> pair = std::make_pair(msg, priority);

        /*
        * ���� ���� ������, ��������� ���� � ������. ���� ���� �� �����, ������ ������� � ����� �������, � ����� ��������� ������ ������� � ������������� ���� ��� ������� ��������.
        */
        if (size == 0) {
            heapQueue.push_back(std::move(pair));
        }
        else {
            //��������� � ��������� �� �����
            heapQueue.push_back(std::move(pair));
            for (int parent = heapQueue.size() / 2 - 1; parent >= 0; parent--) {
                sortHeap(parent);
            }
        }
        elementCount = heapQueue.size();
        //��������� ���������� ������� HWM � LWM ����� ����������
        checkHWMLWM();
        lck.unlock();
        //����� �������� ������������� �������� ����� � ���, ��� �� �������� ��������� � �������.
        condition.notify_one();
        return RetCodes::OK;
    }
}

/*
* ����� �������� ���������� ������ HWM � LWM.
* ���� ������� �������� ������ HWM, ������ � ��� ������������ ���� �� ����� ��������� LWM.
* ���� ������� ��������� �� ������ HWM, ������ ������� ��������������� � HWM � �������� ������ �� ���������� ������ LWM.
* ��� ���������� ������ LWM ������ ������� ��������������� � LWM.
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

/*
* ����������� ����� ��������� ��������� �� �������.
* ���� ������� � ��������� STOPPED ������ �� ����������.
* ����� ����� ��������� ������.
* ���� ������� �����, ���������� ���������� ���� � ������� �� �������� ��������� � �������� ������������� �� ����� ������� � ���, ��� � ������� ������ ���������
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
