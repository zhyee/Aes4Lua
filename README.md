# AES4Lua
用C实现的一个基于Libmcrypt开源库的Lua AES加解密模块

国内大神的openresty项目下的 [lua-resty-string](https://github.com/openresty/lua-resty-string) 模块也包含了AES加解密的功能，但是主要是配合ngx-lua模块用于web开发，并且对于指定的密钥是先进行若干轮hash运算，把hash运算的结果再作为key，这对于加密和解密都是用这个模块的场景是没有问题的，这种场景也推荐你使用该模块，但如果别人提供的仅是密钥和加密好的密文，让我们用lua来解密，这时这个模块将显得不太好用了，所以用C编写了这个lua AES加解密模块。
