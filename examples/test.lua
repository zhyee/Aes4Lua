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
