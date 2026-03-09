CC = g++
# point these at your OpenSSL installation (optional)
OPENSSL_INC =
OPENSSL_LIB =

# include path; only add OpenSSL include directory if provided
CFLAGS = -std=c++17 -I. $(if $(OPENSSL_INC),-I$(OPENSSL_INC))

# link flags; link libcrypto only when library path supplied
ifeq ($(OPENSSL_LIB),)
LDFLAGS =
else
LDFLAGS = -lcrypto -L$(OPENSSL_LIB)
endif

SRCS = main.cpp HospitalSystem.cpp CryptoUtils.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = HAMS.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lcrypto

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

test: $(TARGET)
	./$(TARGET)   # run the program (manually inspect output)

clean:
	rm -f $(OBJS) $(TARGET) hams_data.bin
