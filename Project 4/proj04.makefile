proj04: proj04.student.o
	g++ proj04.student.o -o proj04

	proj04.student.o: proj04.student.cpp
		g++ -Wall -c proj04.student.cpp
