proj11.client: proj11.client.o
	g++ proj11.client.o -o proj11.client

proj11.client.o: proj11.client.cpp
	g++ -Wall -c proj11.client.cpp