proj12.client: proj12.client.o
	g++ proj12.client.o -o proj12.client

proj12.client.o: proj12.client.cpp
	g++ -Wall -c proj12.client.cpp