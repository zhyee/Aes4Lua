#include <ctype.h>
#include "utils.h"

/**
 * 单个十六进制字符转成相应整数
 */
int hexchartoint(char h)
{
        if (h >= '0' && h <= '9')
        {
                return h - '0';
        }
        h = toupper(h);
        return h - 55;
}

