# the compiler
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

TARGET = yaws_server client

all: $(TARGET)

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client

yaws_server: main.o server.o
	$(CC) $(CFLAGS) -o yaws_server main.o server.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

server.o: server.cpp server.h
	$(CC) $(CFLAGS) -c server.cpp

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
clean:
	$(RM) $(TARGET) *.o *~
