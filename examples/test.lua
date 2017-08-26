local AES = require "AES"

local td = AES.mcrypt_module_open("rijndael-128", "ecb")

local blocksize, err = AES.mcrypt_enc_get_block_size(td)

print(blocksize, err)

local ok, err= AES.mcrypt_generic_init(td, "1234567890", "")

print(ok, err)

local encrypt,err = AES.mcrypt_generic(td, "abcdefghijklmnopqrst");

print(AES.bintohex(encrypt), err)

AES.mcrypt_module_close(td)
