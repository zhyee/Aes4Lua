local AES = require "AES"

local td = AES.mcrypt_module_open("rijndael-128", "ecb") --用指定算法和模式打开加密模块

local ok, err= AES.mcrypt_generic_init(td, "1234567890123456", "") --初始化加密所需的缓冲区
print(ok, err)

local encrypt,err = AES.mcrypt_generic(td, "abcdefghijklmnopqrst") --加密
print(AES.bintohex(encrypt))

local text = AES.hextobin("7a3c1b91c67558a0229072e2f2ad8a38cee21e69187f07a5ed9803104a2252fe") --解密
print(AES.mdecrypt_generic(td, encrypt))

AES.mcrypt_generic_deinit(td)  --释放加密缓冲区 之后可以用新的key和iv来初始化

AES.mcrypt_module_close(td)  -- 关闭加密模块

