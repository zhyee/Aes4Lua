#ifndef _AES4LUA_H_
#define _AES4LUA_H_

static int lua_mcrypt_module_open(lua_State *L);
static int lua_mcrypt_module_close(lua_State *L);

static int lua_mcrypt_enc_is_block_algorithm_mode(lua_State *L);

static int lua_mcrypt_enc_get_iv_size(lua_State *L);
static int lua_mcrypt_enc_get_key_size(lua_State *L);
static int lua_mcrypt_enc_get_block_size(lua_State *L);

static int lua_mcrypt_generic_init(lua_State *L);
static int lua_mcrypt_generic_deinit(lua_State *L);

static int lua_mcrypt_generic(lua_State *L);
static int lua_mdecrypt_generic(lua_State *L);


static int push_error(lua_State *L, const char *errmsg);
static int bintohex(lua_State *L);
static int hextobin(lua_State *L);



#endif
