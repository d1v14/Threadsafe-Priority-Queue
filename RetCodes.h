#pragma once

/*
* ������������, ����������� ��������� ������������ ���� ������� ������ � ������ � �������.
* HWM - ������� �� HWM
* NO_SPACE - ��� �����
* STOPPED - ������� �����������.
*/
enum class RetCodes : int {
    OK = 0,  
    HWM = -1,      
    NO_SPACE = -2,
    STOPPED = -3
};