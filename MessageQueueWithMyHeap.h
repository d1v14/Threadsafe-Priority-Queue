#pragma once
#include "RetCodes.h"
#include "StateVariables.h"
#include <vector>
#include <mutex>
#include <condition_variable>
#include <iostream>

/*
    ��������� ����� ������������ ������� ������������� �� �������� ����, ���������� � ����������� std::vector. 
    ������ �������� �������� ����: � �������� ������� � �������� n ��� �������, ������� �������� � �������� 2n+1 � 2n+2, �������� ������� � �������� 0
    ������ ������ ��������� ������ ����������� �������� ����� ������� �� O(logN)

*/

template<typename MessageType>
class MessageQueueWithMyHeap {
public:
    /*
    * ������� ����������� �� ���������
    */
    MessageQueueWithMyHeap() = delete;

    /*
    * ������� ��� ������� ����������� ������� �������
    */
    MessageQueueWithMyHeap& operator=(const MessageQueueWithMyHeap&) = delete;

    MessageQueueWithMyHeap& operator=(const MessageQueueWithMyHeap&&) = delete;

    MessageQueueWithMyHeap(const MessageQueueWithMyHeap&) = delete;

    MessageQueueWithMyHeap(const MessageQueueWithMyHeap&&) = delete;

    MessageQueueWithMyHeap(int size, int lwm, int hwm);

    ~MessageQueueWithMyHeap() = default;

    /*
    * ����� ���������� ��������� ������ �� ������� �������� ���� (����������)
    */
    void sortHeap(int parent);

    /*
    * ����� �������� ���������� ������� HWM � LWM.
    */
    void checkHWMLWM();

    /*
    * ����� ���������� ��������� � �������.
    * �������� ��������� � ��� ���������
    * ���������� ���, ��������������� ���������� ����������.
    */
    RetCodes put(MessageType msg, int priority);

    /*
    * ����� ������ ��������� �� ������� 
    * �������� ������ �� ������ ���� ���������,���� ���������� ����� �������� ���������.
    * ���������� ���, ��������������� ���������� ������ �� �������. 
    */
    RetCodes get(MessageType& msg);

    /*
    * ����� ��������������� ������ �������.
    * �������� ���������� ��������� ������� state �� StateVariable::STOPPED, ��� ����� ������ ������ � ���������� �� ������ �������������� �� �������.
    */
    void stop();
    /*
    * ����� ��������� ������ �������.
    * �������� ���������� ��������� ������� state �� StateVariable::STARTED, ��� ����� ������ ������ � ���������� ������ �������������� �� �������
    */
    void run();
    /* 
    * ����� �������� ������� �������
    */
    bool empty();

private:
    /*
    * std::vector, �������� �������� ���� �� ��� (���������, ���������)
    */
    std::vector<std::pair<MessageType, int>> heapQueue;

    /*
    * ������� ������� ��� ����������� ������������������ ��� ������ � ������ �� �������.
    */
    std::mutex mtx;

    /*
    * �������� �������������
    */
    std::condition_variable condition;
    /*
    * ������ HWM � LWM
    */
    int HWMLevel;
    int LWMLevel;
    int elementCount;
    int sizeOfQueue;
    /*
    * ���������� ��������� ������
    */
    StateVariables state;
};
