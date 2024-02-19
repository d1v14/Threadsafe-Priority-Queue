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
    * ������� ����������� �� ���������
    */
    MessageQueueWithSTL() = delete;

    /*
    * ������� ��� ������� ����������� ������� �������.
    */
    MessageQueueWithSTL& operator=(const MessageQueueWithSTL&) = delete;

    MessageQueueWithSTL& operator=(const MessageQueueWithSTL&&) = delete;

    MessageQueueWithSTL(const MessageQueueWithSTL&) = delete;

    MessageQueueWithSTL(const MessageQueueWithSTL&&) = delete;

    /*
    * ������ ����������� ������������ �����������, ����������� ������ ������� � ������ HWM LWM
    */
    MessageQueueWithSTL(int size, int lwm, int hwm);

    ~MessageQueueWithSTL() = default;
        
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
    * ����� �������� ���������� ������� HWM � LWM
    */
    void checkHWMLWM();

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
    * ��������� ����� ��� ���������� ������������ ����������� ��� ������������ ������� �� STL
    */
    class Compare
    {
    public:
        bool operator() (std::pair<MessageType, int> a, std::pair<MessageType, int> b)
        {
            /*
            * �������� ��� ���� �� �������.
            * ���������� ��������� ��������� ��� �� ������� ��������(����������)
            */
            return a.second < b.second;
        }
    };
    /*
    * ������������ ������� �� STL �� ������� � ����������� ������������, �������� ���� (���������, ���������)
    */
    std::priority_queue<std::pair<MessageType, int>, std::vector<std::pair<MessageType, int>>, Compare> pq;
    int sizeOfQueue;
    /*
    * ������ HWM � LWM
    */
    int HWMLevel;
    int LWMLevel;

    int elementCount;
    /*
    * ������� ������� ��� ����������� ������������������ ��� ������ � ������ �� �������.
    */
    std::mutex mtx;
    /*
    * �������� �������������
    */
    std::condition_variable condition;
    /*
    * ���������� ��������� �������
    */
    StateVariables state;
};