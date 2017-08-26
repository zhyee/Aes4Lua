LIBLUAPATH = /usr/lib64/liblua.so
LIBMCRYPTPATH = /usr/local/lib/libmcrypt.so
INSTALLPATH = /usr/lib64/lua/5.1
TARGET = AES.so

$(TARGET): AES4Lua.o
	cc -shared -o $@ $< $(LIBLUAPATH) $(LIBMCRYPTPATH)

AES4Lua.o : AES4Lua.c
	cc -fPIC -c $^

.PHONY: install test clean

install:
	rm -f $(INSTALLPATH)/$(TARGET)
	cp $(TARGET) $(INSTALLPATH)/

test:
	ldd $(TARGET)

clean:
	rm -f *.o *.so
