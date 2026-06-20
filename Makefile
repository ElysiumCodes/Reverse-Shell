CC = gcc
CFLAGS = -Wall -Wextra -O2

# Targets
HOST = hostCode
TARGET = targetCode

all: $(HOST) $(TARGET)

$(HOST): hostCode.c
	$(CC) $(CFLAGS) -o $(HOST) hostCode.c

$(TARGET): targetCode.c
	$(CC) $(CFLAGS) -o $(TARGET) targetCode.c

clean:
	rm -f $(HOST) $(TARGET)

.PHONY: all clean
