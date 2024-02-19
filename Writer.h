#pragma once
#include <random>
#include <thread>
#include "RetCodes.h"
#include <iostream>
/*
* ����� �������� � ������������ �������
*/
template<typename QueueType>
class Writer
{
public:

    /*
    * ������� ����������� �� ��������� � ������� ���� � ������� �� ������� ��������������
    */
    Writer() = delete;
    explicit Writer(int id);
    /*
    * �����, ����������� ������ � ������������ �������
    */
    void run(QueueType &que);

private:
    /*
    * ������������� ��������
    */
    int id;
};

