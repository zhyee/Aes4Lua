#include "utils.h"

void bin2hex(const unsigned char *bin, int binlen, char *out)
{   
        char *hexc = "0123456789abcdef";
        int i;
        unsigned char tmp;
        for (i = 0; i < binlen; i++)
        {   
                tmp = bin[i] >> 4;
                out[i*2] = hexc[tmp];
                tmp = bin[i] & 0x0f;
                out[i*2+1] = hexc[tmp];
        }
}

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



int hex2bin(const char *hex, int hexlen, unsigned char *bin)
{
        int i = 0, j = 0;
        unsigned char tmp1, tmp2;
        for(i = 0; i < hexlen; i+=2)
        {
                tmp1 = hexchar2int(hex[i]);
                tmp2 = hexchar2int(hex[i+1]);
                if (tmp1 < 0 || tmp2 < 0)
                {
                        return -1;
                }
                bin[j++] = tmp1 << 4 + tmp2;
        }
        return j;
}


typedef struct _base64_t
{
        unsigned char a;
        unsigned char b;
        unsigned char c;
} base64_t;


void base64_encode(const unsigned char *in, int inlen, char *out)
{
        const char *base64_char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        base64_t bt;
        unsigned char tmp;

        int i = 0, j = 0, count, extra;

        count = inlen / 3;
        extra = inlen % 3;

        for(i = 0; i < count; i++) {
                memcpy(&bt, in + (i*3), sizeof(base64_t));
                tmp = bt.a >> 2;  //取bt.a的最高6bit
                out[j++] = base64_char[tmp];

                tmp = ((bt.a & 0x03) << 4) + (bt.b >> 4); // 取bt.a的最低2bit 和 bt.b的最高4bit
                out[j++] = base64_char[tmp];

                tmp = ((bt.b & 0x0f) << 2) + (bt.c >> 6); // 取bt.b的最低4bit 和 bt.c的最高2bit
                out[j++] = base64_char[tmp];

                tmp = bt.c & 0x3f;     //取bt.c的最低6bit
                out[j++] = base64_char[tmp];
        }

        if (extra > 0)
        {
                memset(&bt, 0, sizeof(base64_t));
                memcpy(&bt, in+inlen-extra, extra);

                tmp = bt.a >> 2;  //取bt.a的最高6bit
                out[j++] = base64_char[tmp];

                tmp = ((bt.a & 0x03) << 4) + (bt.b >> 4); // 取bt.a的最低2bit 和 bt.b的最高4bit
                out[j++] = base64_char[tmp];

                if (extra == 1)
                {
                        out[j++] = '=';
                        out[j++] = '=';
                }
                else if (extra == 2)
                {
                        tmp = ((bt.b & 0x0f) << 2) + (bt.c >> 6);
                        out[j++] = base64_char[tmp];
                        out[j++] = '=';
                }
        }
}

unsigned char base64_char_to_int(const char c)
{
        if (c >= 'A' && c <= 'Z')
        {
                return c - 'A';
        }
        else if (c >= 'a' && c <= 'z')
        {
                return c - 'a' + 26;
        }
        else if (c >= '0' && c <= '9')
        {
                return c - '0' + 52;
        }
        else if (c == '+')
        {
                return 62;
        }
        else if (c == '/')
        {
                return 63;
        }
        else if(c == '=')
        {
                return 0;
        }
        return -1;
}

int base64_decode(const char *in, int inlen, unsigned char *out)
{
        int i, j, count, extra = 0, outlen = 0;
        count = inlen / 4;
        if (in[inlen-2] == '=')
        {
                extra = 2;
        }
        else if (in[inlen-1] == '=')
        {
                extra = 1;
        }

        base64_t bt;
        unsigned char tmp1, tmp2, tmp3, tmp4;
        for(i = 0, j = 0; i < count; i++)
        {
                tmp1 = base64_char_to_int(in[j++]);
        }
        else if(c == '=')
        {
                return 0;
        }
        return -1;
}

int base64_decode(const char *in, int inlen, unsigned char *out)
{
        int i, j, count, extra = 0, outlen = 0;
        count = inlen / 4;
        if (in[inlen-2] == '=')
        {
                extra = 2;
                else if(c == '=')
                {
                        return 0;
                }
                return -1;
        }

        int base64_decode(const char *in, int inlen, unsigned char *out)
        {
                int i, j, count, extra = 0, outlen = 0;
                count = inlen / 4;
                if (in[inlen-2] == '=')
                {
                        extra = 2;
                }
                else if (in[inlen-1] == '=')
                {
                        extra = 1;
                }

                base64_t bt;
                unsigned char tmp1, tmp2, tmp3, tmp4;
                for(i = 0, j = 0; i < count; i++)
                {
                        tmp1 = base64_char_to_int(in[j++]);
                        tmp2 = base64_char_to_int(in[j++]);
                        tmp3 = base64_char_to_int(in[j++]);
                        tmp4 = base64_char_to_int(in[j++]);
                        if (tmp1 < 0 || tmp2 < 0 || tmp3 < 0 || tmp4 < 0)
                        {
                                return -1; //出错
                        }
                        bt.a = (tmp1 << 2) + (tmp2 >> 4);
                        bt.b = (tmp2 << 4) + (tmp3 >> 2);
                        bt.c = (tmp3 << 6) + tmp4;
                        memcpy(out+outlen, &bt, sizeof(base64_t));
                        outlen += sizeof(base64_t);
                }
                outlen -= extra;
                return outlen;
        }

