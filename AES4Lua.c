#include <lauxlib.h>
#include <mcrypt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "AES4Lua.h"

#define PADDING_MODE_ZERO   0
#define PADDING_MODE_PKCS5  1
#define PADDING_MODE_PKCS7  2

/**
 * 数据长度不是加密块的整数倍时，可选的填充模式
 */
typedef enum _padding_mode {
    NO_PADDING = 0,  //无填充
    ZERO_PADDING = 1,  //数据补0
    PKCS5_PADDING = 2, // 直接使用 PKCS7_PADDING
    PKCS7_PADDING = 3  // PKCS5_PADDING 的升级版，对数据块的长度没有限制
} padding_mode;

typedef struct _lua_crypt {
    padding_mode padmode;
    MCRYPT  td;
} lua_crypt;


/**
 * 打开指定加密模块
 */
static int lua_mcrypt_module_open(lua_State *L)
{
	char *algorithm = (char *)lua_tostring(L, 1);
	char *mode = (char *)lua_tostring(L, 2);

	lua_crypt *lcp = (lua_crypt *)lua_newuserdata(L, sizeof(lua_crypt));

	MCRYPT td = mcrypt_module_open(algorithm, NULL, mode, NULL);

	if (MCRYPT_FAILED == td)
	{
		return push_error(L, "mcrypt_module_open: open module fail");
	}

    lcp->td = td;
    
	return 1;
}


/**
 * 获取打开的加密算法对应的向量长度
 */
static int lua_mcrypt_enc_get_iv_size(lua_State *L)
{
	lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
	
	if (NULL == lcp)
	{
		return push_error(L, "mcrypt_enc_get_iv_size: param is not type of userdata");
	}
	
	int len = mcrypt_enc_get_iv_size(lcp->td);
	
	lua_pushinteger(L, len);

	return 1;	
}

/**
 * 获取打开的加密算法支持的密钥长度
 */
static int lua_mcrypt_enc_get_key_size(lua_State *L)
{
	lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
	
	if (NULL == lcp)
	{
		return push_error(L, "mcrypt_enc_get_key_size: param is not type of userdata");
	}
	int len = mcrypt_enc_get_key_size(lcp->td);

	lua_pushinteger(L, len);

	return 1;
}


/**
 * 验证使用的加密模式是否支持分组模式
 */
static int lua_mcrypt_enc_is_block_algorithm_mode(lua_State *L)
{
    lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
    if (NULL == lcp)
    {
        return push_error(L, "mcrypt_enc_is_block_algorithm_mode: param is not type of userdata");
    }
    int is_block = mcrypt_enc_is_block_algorithm_mode(lcp->td);
    lua_pushboolean(L, is_block);
    return 1;
}

/**
 * 获取打开的加密算法加密块大小
 */
static int lua_mcrypt_enc_get_block_size(lua_State *L)
{
	lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
	
	if (NULL == lcp)
	{
		return push_error(L, "mcrypt_enc_get_block_size: param is not type of userdata");
	}

	int len = mcrypt_enc_get_block_size(lcp->td);
	lua_pushinteger(L, len);
	
	return 1;
}

/**
 * 初始化缓冲区
 */
static int lua_mcrypt_generic_init(lua_State *L)
{
	lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
        
    if (NULL == lcp)
    {       
            return push_error(L, "mcrypt_generic_init: param is not type of userdata");
    }

	size_t keysize, ivsize;
	char *key = (char *)lua_tolstring(L, 2, &keysize);

	if (keysize > mcrypt_enc_get_key_size(lcp->td))
	{
		return push_error(L, "mcrypt_generic_init: the length of key exceeds limit");
	}

	char *iv = (char *)lua_tolstring(L, 3, &ivsize);

	if (ivsize > mcrypt_enc_get_iv_size(lcp->td))
	{
		return push_error(L, "mcrypt_generic_init: the length of iv exceeds limit");
	}

    int pad_mode = lua_tointeger(L, 4);

    if (pad_mode != NO_PADDING && pad_mode != ZERO_PADDING && pad_mode != PKCS5_PADDING && pad_mode != PKCS7_PADDING)
    {
        return push_error(L, "not surpport the padding mode");
    }

    lcp->padmode = pad_mode;

	int ret = mcrypt_generic_init(lcp->td, key, keysize, iv);
	if (ret < 0)
	{
		char errmsg[512] = {'\0'};
		sprintf(errmsg, "%s: %s", "mcrypt_generic_init", mcrypt_strerror(ret));
		return push_error(L, errmsg);
	}

	lua_pushboolean(L, 1);
	return 1;
}

/**
 * 清理缓冲区
 */
static int lua_mcrypt_generic_deinit(lua_State *L)
{
	lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);

    if (NULL == lcp)
    {       
            return push_error(L, "mcrypt_generic_deinit: param is not type of userdata");
    }

	int ret = mcrypt_generic_deinit(lcp->td);

	if (ret < 0)
	{
		return push_error(L, "mcrypt_generic_deinit: deinit fail");
	}

	lua_pushboolean(L, 1);
		
	return 1;
}


/**
 * 加密
 */
static int lua_mcrypt_generic(lua_State *L)
{
	lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
	if (NULL == lcp)
	{
		return push_error(L, "mcrypt_generic: param is not type of userdata");	
	}

    int blocksize, mod, i, ret;
	size_t textlen, paddinglen, totallen;
    unsigned char  paddingchar;
	const char *text = lua_tolstring(L, 2, &textlen);

	blocksize = mcrypt_enc_get_block_size(lcp->td);  //获取加密块大小
	mod = textlen % blocksize;	

    paddinglen = 0;
    switch(lcp->padmode)
    {
            case NO_PADDING:
                    if(mod > 0)
                    {
                            return push_error(L, "mcrypt_generic: use NO_PADDING the data length must be blocksize * k");
                    }
                    break;
            case ZERO_PADDING:
                    if (mod > 0){
                            paddinglen = blocksize - mod;
                            paddingchar = '\0';
                    }
                    break;
            case PKCS5_PADDING:
            case PKCS7_PADDING:
                    paddinglen = blocksize - mod;
                    paddingchar = (unsigned char)paddinglen;
                    break;

    }

    totallen = textlen + paddinglen;
	char *buffer = (char *)calloc(totallen, 1);
	memcpy(buffer, text, textlen);
    if (paddinglen > 0)
    {
       memset(buffer+textlen, paddingchar, paddinglen); 
    }

    i = 0;
    while((i * blocksize) < totallen)
    {    
	    ret = mcrypt_generic(lcp->td, buffer + (i * blocksize), blocksize);
        if (ret != 0)
        {
                char errmsg[512] = {'\0'};
                sprintf(errmsg, "%s: %s", "mcrypt_generic", mcrypt_strerror(ret));
                free(buffer);
                return push_error(L, errmsg);
        }
        i++;
    }

		lua_pushlstring(L, buffer, totallen);
		free(buffer);
		return 1;
}

/**
 * 解密
 */
static int lua_mdecrypt_generic(lua_State *L)
{
	lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
        if (NULL == lcp)
        {       
                return push_error(L, "mdecrypt_generic: param is not type of userdata");
        }

        size_t textlen, paddinglen;
        int blocksize, i, ret;
        const char *text = lua_tolstring(L, 2, &textlen);

        blocksize = mcrypt_enc_get_block_size(lcp->td);  //获取加密块大小
        
	if (textlen % blocksize != 0)
        {
                return push_error(L, "The length of param is not equal to blocksize * k");
        }

        char *buffer = (char *)calloc(textlen, 1);
        memcpy(buffer, text, textlen);

        i = 0;
        while((i * blocksize) < textlen)
        {
            ret = mdecrypt_generic(lcp->td, buffer + (i * blocksize), blocksize);       
            if (ret != 0)
            {
                    char errmsg[512] = {'\0'};
                    sprintf(errmsg, "%s: %s", "mdecrypt_generic", mcrypt_strerror(ret));
                    free(buffer);
                    return push_error(L, errmsg);
            }
            i++;
        }

        paddinglen = 0;
        if (lcp->padmode == PKCS5_PADDING || lcp->padmode == PKCS7_PADDING)
        {
            paddinglen = buffer[textlen-1];
        }

        lua_pushlstring(L, buffer, textlen - paddinglen);
        free(buffer);
        return 1;	
}

/**
 * 关闭加密模块
 */
static int lua_mcrypt_module_close(lua_State *L)
{
        lua_crypt *lcp = (lua_crypt *)lua_touserdata(L, 1);
 
        if (NULL == lcp)
        {
		return push_error(L, "mcrypt_module_close: param is not type of userdata");
        }
	mcrypt_module_close(lcp->td);
	lua_pushboolean(L, 1);
	return 1;
}

/**
 * 出错返回
 */
static int push_error(lua_State *L, const char *errmsg)
{			
	lua_pushnil(L);
	lua_pushstring(L, errmsg);
	return 2;	
}


/**
 * 二进制转十六进制形式字符串
 */
static int bintohex(lua_State *L)
{
	int i;
	size_t len;
	const unsigned char *bin = lua_tolstring(L, 1, &len);
	char *hex = calloc(1, len * 2 + 1);
	char buf[2];
	for(i = 0; i < len; i++)
	{	
		memset(buf, 0, 2);
		sprintf(buf, "%x", bin[i]);
		if (buf[1] == '\0')
		{
			buf[1] = buf[0];
			buf[0] = '0';
		}
		memcpy(hex+(i * 2), buf, 2);			
	}
	lua_pushlstring(L, hex, len * 2 + 1);
	free(hex);
	return 1;
}


/**
 * 十六进制字符串转二进制
 */
static int hextobin(lua_State *L)
{
	int i;
	char c;
	size_t hexlen, binlen;
	const char *hex = lua_tolstring(L, 1, &hexlen);
	if (hexlen % 2 != 0)
	{
		return push_error(L, "hextobin: the length of param is not even");
	}

	binlen = hexlen / 2;
	char *bin = calloc(binlen, 1);
	for(i = 0; i < hexlen; i++)
	{
		if (i % 2 == 0)
		{
			c = 0;
			c += hexchartoint(hex[i]) * 16;		
		}	
		else
		{
			c += hexchartoint(hex[i]);
			bin[i/2] = c;
		}
	}
	lua_pushlstring(L, bin, binlen);
	free(bin);
	return 1;
}


static const struct luaL_Reg funcs[] = {
    {"mcrypt_module_open", lua_mcrypt_module_open},
	{"mcrypt_enc_get_iv_size", lua_mcrypt_enc_get_iv_size},
	{"mcrypt_enc_get_key_size", lua_mcrypt_enc_get_key_size},
    {"mcrypt_enc_is_block_algorithm_mode", lua_mcrypt_enc_is_block_algorithm_mode},
	{"mcrypt_enc_get_block_size", lua_mcrypt_enc_get_block_size},
	{"mcrypt_generic_init", lua_mcrypt_generic_init},
	{"mcrypt_generic_deinit", lua_mcrypt_generic_deinit},
	{"mcrypt_generic", lua_mcrypt_generic},
	{"mdecrypt_generic", lua_mdecrypt_generic},
	{"mcrypt_module_close", lua_mcrypt_module_close},
	{"bintohex", bintohex},
	{"hextobin", hextobin},
    {NULL, NULL}
};

int luaopen_AES(lua_State *L)
{
        luaL_register(L, "AES", funcs);
        return 1;
}

