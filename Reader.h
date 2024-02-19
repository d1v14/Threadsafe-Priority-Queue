#pragma once
#include <random>
#include <thread>
#include "RetCodes.h"
#include <iostream>

/*
* ����� �������� ������������ �������
*/
template<typename QueueType>
class Reader
{
public:
    /*
    * ������� ����������� �� ��������� � ������� ���� � ������� �� ������� ��������������
    */
    Reader() = delete;
    explicit Reader(int id);

    ~Reader() = default;
    /*
    * �����, ����������� ������ �� ������������ �������
    */
    void run(QueueType& que);

private:
    /*
    * ������������� ��������
    */
    int id;
};