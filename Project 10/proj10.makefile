proj10: proj10.student.o
	g++ proj10.student.o -o proj10

proj10.student.o: proj10.student.cpp
	g++ -Wall -c proj10.student.cpp
