local AES = require "AES"

local td = AES.mcrypt_module_open("rijndael-128", "ecb") --用指定算法和模式打开加密模块

AES.mcrypt_generic_init(td, "1234567890123456", "") --初始化加密所需的缓冲区
local encrypt,err = AES.mcrypt_generic(td, "We meet at old place!") --加密
print(AES.bintohex(encrypt))

AES.mcrypt_generic_init(td, "1234567890123456", "") --初始化加密所需的缓冲区
print(AES.mdecrypt_generic(td, encrypt)) --解密 

AES.mcrypt_generic_deinit(td)  --释放加密缓冲区 之后可以用新的key和iv来初始化

AES.mcrypt_module_close(td)  -- 关闭加密模块


print("\n--------------------------cbc模式--------------------------\n")

td = AES.mcrypt_module_open("rijndael-128", "cbc")  -- 以cbc模式加密

AES.mcrypt_generic_init(td, "1234567890123456", "abcdefghijklmnop") -- cbc模式必须提供一个向量
encrypt,err = AES.mcrypt_generic(td, "Very important message")
print(AES.bintohex(encrypt))

AES.mcrypt_generic_init(td, "1234567890123456", "abcdefghijklmnop") -- cbc模式必须提供一个向量
print(AES.mdecrypt_generic(td, encrypt))

AES.mcrypt_generic_deinit(td)
AES.mcrypt_module_close(td)
