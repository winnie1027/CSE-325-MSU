proj07: proj07.student.o
		g++ proj07.student.o -o proj07

proj07.student.o: proj07.student.cpp
		g++ -Wall -c proj07.student.cpp
