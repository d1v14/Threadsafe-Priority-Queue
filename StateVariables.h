#pragma once

/*
* ������������ ����������� ��������� ��������� �������.
* STOPPED - ������� �����������.
* STARTED - ������� ������ ������.
* OK - ������� ��������� � ���������, ����� ��������� LWM, �� �� ����� ��� �� ���������� � ��������� HWM
* HWM - ������� �������� ������ HWM
* LWM - ������� �������� ������ LWM
*/
enum class StateVariables : int {
    STARTED,
    STOPPED,
    OK,
    HWM,
    LWM
};