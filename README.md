# AES4Lua
用C实现的一个基于Libmcrypt开源库的Lua AES加解密模块

国内大神的openresty项目下的 [lua-resty-string](https://github.com/openresty/lua-resty-string) 模块也包含了AES加解密的功能，但是主要是配合ngx-lua模块用于web开发，并且对于指定的密钥是先进行若干轮hash运算，把hash运算的结果再作为key，这对于加密和解密都是用这个模块的场景是没有问题的，这种场景也推荐你使用该模块，但如果别人提供的仅是密钥和加密好的密文，让我们用lua来解密，这时这个模块显得不太好用，所以用C编写了这个lua AES加解密模块。

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
2. 安装lua5.1
3. 编译安装本模块

把项目克隆到本地，修改Makefile中的**LIBLUAPATH**，**LIBMCRYPTPATH**，**INSTALLPATH** 三个路径为你当前环境的值
 
```
LIBLUAPATH = /path/to/liblua.so  #liblua.so 路径 
LIBMCRYPTPATH = /path/to/libmcrypt.so #libmcrypt.so 路径
INSTALLPATH = /usr/lib64/lua/5.1  #你要安装动态库的路径
```
然后 make && make install 即可， 如果没有报错说明安装成功, 可以键入命令make test 来看动态库加载是否正确，会输出类似以下信息：
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
AES.mcrypt_enc_get_key_size(lcp)
 
    获取打开的加密模块支持的key的最大长度，参数为调用AES.mcrypt_module_open返回的句柄，
    调用成功返回一个整数，失败返回nil和一个描述错误信息的字符串。
```

```
AES.mcrypt_enc_get_iv_size(lcp)
 
    返回打开的加密模块支持的向量最大长度，用法同AES.mcrypt_enc_get_key_size
```

```
AES.mcrypt_enc_is_block_algorithm_mode(lcp)
	检测该加密模式是否支持分组加密
```

```
AES.mcrypt_enc_get_block_size(lcp)
 
    对于分块加密的模式，返回加密块的长度，用法同AES.mcrypt_enc_get_key_size
```

```
AES.mcrypt_generic_init(lcp, key, iv, padding_mode)
 
    用指定秘钥key、向量iv和填充模式padding_mode 初始化加密模块，调用成功返回true，否则返回nil和一个描述错误信息的字符串
	其中padding_mode 可以取 0 1 2 3：
	0：无填充
	1：用 '\0' 填充
	2：pkcs5 padding
	3：pkcs7 padding
    每次加密和解密前都应先调用此方法
```

```
AES.mcrypt_generic_deinit(lcp)
 
    清理加密缓冲区，之后仍可以用AES.mcrypt_generic_init重新初始化
```

```
AES.mcrypt_generic(lcp, text)
 
    加密字符串，调用成功返回加密后的字符串，否则返回nil和一个描述错误信息的字符串
```

```
AES.mdecrypt_generic(lcp, text)
 
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
AES.mcrypt_module_close(lcp)
 
    关闭加密模块
```

## Examples

```
local AES = require "AES"

print("\n--------------------------ecb模式--------------------------\n")

local lcp = AES.mcrypt_module_open("rijndael-128", "ecb") --用指定算法和模式打开加密模块

AES.mcrypt_generic_init(lcp, "1234567890123456", "", 3) --初始化加密所需的缓冲区, 使用 PKCS7_PADDING
local encrypt,err = AES.mcrypt_generic(lcp, "We meet at old place!") --加密

print(AES.bintohex(encrypt));


AES.mcrypt_generic_init(lcp, "1234567890123456", "", 3) --初始化加密所需的缓冲区
print(AES.mdecrypt_generic(lcp, encrypt)) --解密 


AES.mcrypt_generic_init(lcp, "1234567890123456", "", 1) --初始化加密所需的缓冲区, 使用 Zero_PADDING
encrypt = "8c38c930acc6de42dfb4a5ce2724e98ad8b59848c7670c94b29b54d2379e2e7a"
print(AES.mdecrypt_generic(lcp, AES.hextobin(encrypt))) --解密 

AES.mcrypt_generic_deinit(lcp)  --释放加密缓冲区 之后可以用新的key和iv来初始化
AES.mcrypt_module_close(lcp)  -- 关闭加密模块


print("\n--------------------------cbc模式--------------------------\n")

lcp = AES.mcrypt_module_open("rijndael-128", "cbc")  -- 以cbc模式加密

AES.mcrypt_generic_init(lcp, "1234567890123456", "abcdefghijklmnop", 1) -- cbc模式必须提供一个向量
encrypt,err = AES.mcrypt_generic(lcp, "We meet at old place!")

print(AES.bintohex(encrypt))

AES.mcrypt_generic_init(lcp, "1234567890123456", "abcdefghijklmnop", 1) -- cbc模式必须提供一个向量
print(AES.mdecrypt_generic(lcp, encrypt))

AES.mcrypt_generic_deinit(lcp)
AES.mcrypt_module_close(lcp)

```

