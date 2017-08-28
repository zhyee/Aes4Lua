LIBLUAPATH = /usr/lib64/liblua.so
LIBMCRYPTPATH = /usr/lib64/libmcrypt.so
INSTALLPATH = /usr/local/lib/lua/5.1
TARGET = AES.so

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

$(TARGET): $(OBJ)
	cc -shared -o $@ $^ $(LIBLUAPATH) $(LIBMCRYPTPATH)

%.o: %.c
	cc -fPIC -c $^

.PHONY: install test clean

install:
	rm -f $(INSTALLPATH)/$(TARGET)
	cp $(TARGET) $(INSTALLPATH)/

test:
	ldd $(TARGET)

clean:
	rm -f *.o *.so
