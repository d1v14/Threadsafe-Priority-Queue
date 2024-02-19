#pragma once

/*
* Перечисление, описывающее возможные возвращаемые коды функций чтения и записи в очередь.
* HWM - очередь на HWM
* NO_SPACE - нет места
* STOPPED - очередь остановлена.
*/
enum class RetCodes : int {
    OK = 0,  
    HWM = -1,      
    NO_SPACE = -2,
    STOPPED = -3
};