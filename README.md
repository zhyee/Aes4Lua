# AES4Lua
用C实现的一个基于Libmcrypt开源库的Lua AES加解密模块

国内大神的openresty项目下的 [lua-resty-string](https://github.com/openresty/lua-resty-string) 模块也包含了AES加解密的功能，但是主要是配合ngx-lua模块用于web开发，并且对于指定的密钥是先进行若干轮hash运算，把hash运算的结果再作为key，这对于加密和解密都是用这个模块的场景是没有问题的，这种场景也推荐你使用该模块，但如果别人提供的仅是密钥和加密好的密文，让我们用lua来解密，这时这个模块将显得不太好用了，所以用C编写了这个lua AES加解密模块。

## Installation
1. 安装libmcrypt开源库
``` 
wget  https://jaist.dl.sourceforge.net/project/mcrypt/Libmcrypt/2.5.8/libmcrypt-2.5.8.tar.gz
tar zxvf libmcrypt-2.5.8.tar.gz
cd libmcrypt-2.5.8
./configure
make
make install
```
2. 编译安装本模块

把项目克隆到本地，修改Makefile中的**LIBLUAPATH**，**LIBMCRYPTPATH**，**INSTALLPATH** 三个路径为你当前环境的值
 
```
LIBLUAPATH = /path/to/liblua.so  #liblua.so 路径 
LIBMCRYPTPATH = /path/to/libmcrypt.so #libmcrypt.so 路径
INSTALLPATH = /usr/lib64/lua/5.1  #你要安装动态库的路径
```
然后 make  make install 即可， 如果没有报错说明安装成功, 可以键入命令make test 来看动态库加载是否正确，会输出类似以下信息：
```
[root@localhost AES4Lua]# make test
ldd AES.so
	linux-vdso.so.1 =>  (0x00007ffdc939a000)
	liblua-5.1.so => /usr/lib64/liblua-5.1.so (0x00007f6f238f3000)
	libmcrypt.so.4 => /usr/lib64/libmcrypt.so.4 (0x00007f6f236c1000)
	libc.so.6 => /lib64/libc.so.6 (0x00007f6f2332c000)
	libm.so.6 => /lib64/libm.so.6 (0x00007f6f230a8000)
	libdl.so.2 => /lib64/libdl.so.2 (0x00007f6f22ea4000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f6f23d2d000)
```

如果显示有**NOT FOUND** 之类的信息，你可能需要手动把该库复制到 /lib/ 或 /lib64/ 目录下。

## Functions
```
AES.mcrypt_module_open(algorithm, mode)
 
    用指定算法（algorithm）和指定模式（mode）打开加密模块，
    调用成功返回一个句柄，调用失败返回nil以及一个描述错误信息的字符串

支持的加密算法有以几种，默认的AES标准是使用 rijndael-128 算法
* blowfish
* des
* tripledes
* threeway
* gost
* safer-sk64
* safer-sk128
* cast-128
* xtea
* rc2
* twofish
* cast-256
* saferplus
* loki97
* serpent
* rijndael-128
* rijndael-192
* rijndael-256
* enigma
* arcfour
* wake
 
加密模式有以下几种，具体模式的区别请参考相关资料：
* cbc
* ecb
* cfb
* ofb
* nofb
* stream
```

```
AES.mcrypt_enc_get_key_size(td)
 
    获取打开的加密模块支持的key的最大长度，参数为调用AES.mcrypt_module_open返回的句柄，
    调用成功返回一个整数，失败返回nil和一个描述错误信息的字符串。
```

```
AES.mcrypt_enc_get_iv_size(td)
 
    返回打开的加密模块支持的向量最大长度，用法同AES.mcrypt_enc_get_key_size
```

```
AES.mcrypt_enc_get_block_size(td)
 
    对于分块加密的模式，返回每次加密块的长度，用法同AES.mcrypt_enc_get_key_size
```

```
AES.mcrypt_generic_init(td, key, iv)
 
    用指定秘钥key和向量iv初始化加密模块，调用成功返回1，否则返回nil和一个描述错误信息的字符串
```

```
AES.mcrypt_generic_deinit(td)
 
    清理加密缓冲区，之后仍可以用AES.mcrypt_generic_init重新初始化
```

```
AES.mcrypt_generic(td, text)
 
    加密字符串，调用成功返回加密后的字符串，否则返回nil和一个描述错误信息的字符串
```

```
AES.mdecrypt_generic(td, text)
 
    解密字符串， 用法同AES.mcrypt_generic
```

```
AES.bintohex(bin_string)
 
    二进制数据转换为十六进制表示的字符串，成功返回转换后的字符串，失败返回nil和一个描述错误信息的字符串，
    可以用该方法转换加密后的数据为人类可读的形式
```

```
AES.hextobin(hex_string)
 
    AES.bintohex的逆操作
```

```
AES.mcrypt_module_close(td)
 
    关闭打开的加密模块
```

## Examples

```
local AES = require "AES"
 
local td = AES.mcrypt_module_open("rijndael-128", "ecb") --用指定算法和模式打开加密模块
 
local ok, err= AES.mcrypt_generic_init(td, "1234567890123456", "") --初始化加密所需的缓冲区, ecb模式不需要向量参数
print(ok, err)
 
local encrypt,err = AES.mcrypt_generic(td, "abcdefghijklmnopqrst") --加密
print(AES.bintohex(encrypt))
 
local text = AES.hextobin("7a3c1b91c67558a0229072e2f2ad8a38cee21e69187f07a5ed9803104a2252fe") --解密
print(AES.mdecrypt_generic(td, encrypt))
 
AES.mcrypt_generic_deinit(td)  --释放加密缓冲区 之后可以用新的key和iv来初始化
 
AES.mcrypt_module_close(td)  -- 关闭加密模块
```

## Performance
*todo*
