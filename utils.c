#include "utils.h"

/**
 * 单个十六进制字符转成相应10进制整数
 */
int hexchartoint(char h)
{
        if (h >= '0' && h <= '9')
        {
                return h - '0';
        }
        else if (h >= 'A' && h <= 'F')
        {
            return h - 'A' + 10;
        }
        else if (h >= 'a' && h <= 'f')
        {
            return h - 'a' + 10;
        }
        return -1;
}
