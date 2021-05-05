proj08: proj08.student.o
	g++ proj08.student.o -o proj08

proj08.student.o: proj08.student.cpp
	g++ -Wall -c proj08.student.cpp
