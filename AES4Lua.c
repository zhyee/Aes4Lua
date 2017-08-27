#include <lauxlib.h>
#include <mcrypt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static int push_error(lua_State *L, const char *errmsg)
{			
	lua_pushnil(L);
	lua_pushstring(L, errmsg);
	return 2;	
}

/**
 * 打开指定加密模块
 */
static int lua_mcrypt_module_open(lua_State *L)
{
	char *algorithm = (char *)lua_tostring(L, 1);
	char *mode = (char *)lua_tostring(L, 2);

	MCRYPT td;
	td = mcrypt_module_open(algorithm, NULL, mode, NULL);

	if (MCRYPT_FAILED == td)
	{
		return push_error(L, "mcrypt_module_open: open module fail");
	}

	MCRYPT *p = (MCRYPT *)lua_newuserdata(L, sizeof(MCRYPT));

	memcpy(p, &td, sizeof(p));
	return 1;
}


/**
 * 获取打开的加密算法对应的向量长度
 */
static int lua_mcrypt_enc_get_iv_size(lua_State *L)
{
	MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);
	
	if (NULL == td)
	{
		return push_error(L, "mcrypt_enc_get_iv_size: param is not type of userdata");
	}
	
	int len = mcrypt_enc_get_iv_size(*td);
	
	lua_pushinteger(L, len);

	return 1;	
}

/**
 * 获取打开的加密算法支持的密钥长度
 */
static int lua_mcrypt_enc_get_key_size(lua_State *L)
{
	MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);
	
	if (NULL == td)
	{
		return push_error(L, "mcrypt_enc_get_key_size: param is not type of userdata");
	}
	int len = mcrypt_enc_get_key_size(*td);

	lua_pushinteger(L, len);

	return 1;
}

/**
 * 获取打开的加密算法加密块大小
 */
static int lua_mcrypt_enc_get_block_size(lua_State *L)
{
	MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);
	
	if (NULL == td)
	{
		return push_error(L, "mcrypt_enc_get_block_size: param is not type of userdata");
	}

	int len = mcrypt_enc_get_block_size(*td);
	lua_pushinteger(L, len);
	
	return 1;
}

/**
 * 初始化缓冲区
 */
static int lua_mcrypt_generic_init(lua_State *L)
{
	MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);
        
        if (NULL == td)
        {       
                return push_error(L, "mcrypt_generic_init: param is not type of userdata");
        }
	
	size_t keysize, ivsize;
	char *key = (char *)lua_tolstring(L, 2, &keysize);

	if (keysize > mcrypt_enc_get_key_size(*td))
	{
		return push_error(L, "mcrypt_generic_init: the length of key exceeds limit");
	}

	char *iv = (char *)lua_tolstring(L, 3, &ivsize);

	if (ivsize > mcrypt_enc_get_iv_size(*td))
	{
		return push_error(L, "mcrypt_generic_init: the length of iv exceeds limit");
	}

	int ret = mcrypt_generic_init(*td, key, keysize, iv);
	if (ret < 0)
	{
		char errmsg[512] = {'\0'};
		sprintf(errmsg, "%s: %s", "mcrypt_generic_init", mcrypt_strerror(ret));
		return push_error(L, errmsg);
	}

	lua_pushinteger(L, 1);
	return 1;
}

/**
 * 清理缓冲区
 */
static int lua_mcrypt_generic_deinit(lua_State *L)
{
	MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);

        if (NULL == td)
        {       
                return push_error(L, "mcrypt_generic_deinit: param is not type of userdata");
        }

	int ret = mcrypt_generic_deinit(*td);

	if (ret < 0)
	{
		return push_error(L, "mcrypt_generic_deinit: deinit fail");
	}

	lua_pushinteger(L, 1);
		
	return 1;
}


/**
 * 加密
 */
static int lua_mcrypt_generic(lua_State *L)
{
	MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);
	if (NULL == td)
	{
		return push_error(L, "mcrypt_generic: param is not type of userdata");	
	}

	size_t textlen;
	const char *text = lua_tolstring(L, 2, &textlen);

	int encrylen = textlen;
	int blocksize = mcrypt_enc_get_block_size(*td);  //获取加密块大小
	int mod = textlen % blocksize;	
	if (mod > 0)
	{
		encrylen = textlen - mod + blocksize;
	}

	char *buffer = (char *)calloc(encrylen, 1);
	memcpy(buffer, text, textlen);	

	int ret = mcrypt_generic(*td, buffer, encrylen);

	if (ret != 0)
	{
		char errmsg[512] = {'\0'};
		sprintf(errmsg, "%s: %s", "mcrypt_generic", mcrypt_strerror(ret));
		free(buffer);
		return push_error(L, errmsg);
	}
	else
	{	
		lua_pushlstring(L, buffer, encrylen);
		free(buffer);
		return 1;
	}
}

/**
 * 解密
 */
static int lua_mdecrypt_generic(lua_State *L)
{
	MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);
        if (NULL == td)
        {       
                return push_error(L, "mdecrypt_generic: param is not type of userdata");
        }

        size_t textlen;
        const char *text = lua_tolstring(L, 2, &textlen);

        int blocksize = mcrypt_enc_get_block_size(*td);  //获取加密块大小
        
	if (textlen % blocksize != 0)
        {
                return push_error(L, "The length of param is not equal to blocksize * k");
        }

        char *buffer = (char *)calloc(textlen, 1);
        memcpy(buffer, text, textlen);

        int ret = mdecrypt_generic(*td, buffer, textlen);

        if (ret != 0)
        {
                char errmsg[512] = {'\0'};
                sprintf(errmsg, "%s: %s", "mdecrypt_generic", mcrypt_strerror(ret));
                free(buffer);
                return push_error(L, errmsg);
        }
        else
        {
                lua_pushlstring(L, buffer, textlen);
                free(buffer);
                return 1;
        }	
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
 * 单个十六进制字符转成相应整数
 */
static int hexchartoint(char h)
{
	if (h >= '0' && h <= '9')
	{
		return h - '0';
	}
	h = toupper(h);
	return h - 55;
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


/**
 * 关闭加密模块
 */
static int lua_mcrypt_module_close(lua_State *L)
{
        MCRYPT *td = (MCRYPT *)lua_touserdata(L, 1);
 
        if (NULL == td)
        {
		return push_error(L, "mcrypt_module_close: param is not type of userdata");
        }
	mcrypt_module_close(*td);
	lua_pushinteger(L, 1);
	return 1;
}

static const struct luaL_Reg funcs[] = {
        {"mcrypt_module_open", lua_mcrypt_module_open},
	{"mcrypt_enc_get_iv_size", lua_mcrypt_enc_get_iv_size},
	{"mcrypt_enc_get_key_size", lua_mcrypt_enc_get_key_size},
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

