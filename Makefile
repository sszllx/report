
CLI_SRC := \
	update-daemon.c \
	client.c \
	report_socket.c \
	event_loop.c

CFLAGS := \
	-Wall \
	-Werror \
	-g

all: \
	client server

client: $(CLI_SRC)
	gcc $(CFLAGS) -o $@ $^ -lpthread

SRV_SRC := \
	server.c \
	report_socket.c \
	event_loop.c \
	task.c \
	notify_fs.c

server: $(SRV_SRC)
	gcc $(CFLAGS) -o $@ $^ -lpthread

clean:
	rm -f client server
