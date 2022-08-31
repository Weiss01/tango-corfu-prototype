build-storage:
	g++ sockets/mysocket.cpp corfu/storage.cpp -std=c++17 -g -Wall -Wextra -lpthread -o storage

build-scheduler:
	g++ sockets/mysocket.cpp corfu/scheduler.cpp -std=c++17 -g -Wall -Wextra -lpthread -o scheduler

build-lrucache:
	g++ sockets/mysocket.cpp corfu/client.cpp corfu/lrucache_demo.cpp corfu/LRU/lrucache.cpp corfu/LRU/node.cpp -std=c++17 -g -Wall -Wextra -o lrucache_demo

clean:
	-rm -r sharedlogs
	-mkdir sharedlogs
	-touch sharedlogs/ph.aux
	clear

build:
	make clean
	make build-storage
	make build-scheduler
	make build-lrucache