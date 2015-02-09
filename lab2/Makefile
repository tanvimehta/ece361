default: server deliver

all:server deliver

server: server.c 
	gcc -o bin/server server.c

deliver: deliver.c 
	gcc -o deliver deliver.c 

clean: 
	rm -f bin/server deliver 

