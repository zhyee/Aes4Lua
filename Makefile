LIBLUAPATH = /usr/lib64/liblua.so
LIBMCRYPTPATH = /usr/lib64/libmcrypt.so
INSTALLPATH = /usr/local/lib/lua/5.1
TARGET = AES.so

$(TARGET): AES4Lua.o
	cc -shared -o $@ $< $(LIBLUAPATH) $(LIBMCRYPTPATH)

AES4Lua.o : AES4Lua.c
	cc -fPIC -c $^

.PHONY: install clean

install:
	rm -f $(INSTALLPATH)/$(TARGET)
	cp $(TARGET) $(INSTALLPATH)/

clean:
	rm -f *.o *.so
