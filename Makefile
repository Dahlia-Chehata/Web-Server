# the compiler
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

TARGET = yaws_server yaws_client

all: $(TARGET)

yaws_client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o yaws_client

yaws_server: main.o yaws_server.o conn_worker.o conn_pool.o
	$(CC) $(CFLAGS) -o yaws_server main.o yaws_server.o conn_worker.o conn_pool.o

# ------------------------------------------------------------------------------

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

yaws_server.o: yaws_server.cpp yaws_server.h
	$(CC) $(CFLAGS) -c yaws_server.cpp

conn_worker.o: conn_worker.cpp conn_worker.h
	$(CC) $(CFLAGS) -c conn_worker.cpp

conn_pool.o: conn_pool.cpp conn_pool.h
	$(CC) $(CFLAGS) -c conn_pool.cpp

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
clean:
	$(RM) $(TARGET) *.o *~
	$(RM) -r *dSYM
