local AES = require "AES"

local td = AES.mcrypt_module_open("rijndael-128", "ecb")

local IVlen, err = AES.mcrypt_enc_get_iv_size(td)

if not IVlen then
	print(err)
else
	print(IVlen)
end

local keyLen, err = AES.mcrypt_enc_get_key_size(td)

print(keyLen)

local blocksize, err = AES.mcrypt_enc_get_block_size(td)

print(blocksize)

AES.mcrypt_module_close(td)


