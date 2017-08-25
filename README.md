# AES4Lua
用C实现的一个基于Libmcrypt开源库的Lua AES加解密模块

国人知名的openresty项目下的 [lua-resty-string](https://github.com/openresty/lua-resty-string) 模块也包含了AES加解密的功能，但是主要是配合ngx-lua模块用于web开发，并且对于指定的密钥是先进行hash运算，把散列运算的结果再作为密钥，这对于先加密再解密的场景是没有问题的，也推荐你使用该模块，但如果别人提供的近是密钥和密文，让我们用lua来解密，这时这个模块将显得很难用，所以自己编写了这个lua模块。
