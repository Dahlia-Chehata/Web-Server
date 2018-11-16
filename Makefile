# the compiler
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -std=c++11

TARGET = yaws_server yaws_client

# http_server directory and files
HTTP_SRV_DIR = http_server
HTTP_SRV_DIR_FILES = $(HTTP_SRV_DIR)/conn_main_handler.o $(HTTP_SRV_DIR)/http_req_handler.o $(HTTP_SRV_DIR)/sock_RAII.o

all: $(TARGET)

yaws_client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o yaws_client
	
yaws_server: main.o yaws_server.o conn_worker.o conn_pool.o $(HTTP_SRV_DIR_FILES)
	$(CC) $(CFLAGS) -o yaws_server main.o yaws_server.o conn_worker.o conn_pool.o $(HTTP_SRV_DIR_FILES)

# ------------------------------------------------------------------------------

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

yaws_server.o: yaws_server.cpp yaws_server.h
	$(CC) $(CFLAGS) -c yaws_server.cpp

conn_worker.o: conn_worker.cpp conn_worker.h
	$(CC) $(CFLAGS) -c conn_worker.cpp

conn_pool.o: conn_pool.cpp conn_pool.h
	$(CC) $(CFLAGS) -c conn_pool.cpp

# ------------------------------------------------------------------------------ http_server

http_req_handler.o: $(HTTP_SRV_DIR)/http_req_handler.cpp $(HTTP_SRV_DIR)/http_req_handler.h
	$(CC) $(CFLAGS) -c $(HTTP_SRV_DIR)/http_req_handler.cpp

conn_main_handler.o:  $(HTTP_SRV_DIR)/conn_main_handler.cpp $(HTTP_SRV_DIR)/conn_main_handler.h
	$(CC) $(CFLAGS) -c $(HTTP_SRV_DIR)/conn_main_handler.cpp

sock_RAII.o:  $(HTTP_SRV_DIR)/sock_RAII.cpp $(HTTP_SRV_DIR)/sock_RAII	.h
	$(CC) $(CFLAGS) -c $(HTTP_SRV_DIR)/sock_RAII.cpp

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
clean:
	$(RM) $(TARGET) *.o *~
	$(RM) -r $(HTTP_SRV_DIR)/*.o	
	$(RM) -r *dSYM
