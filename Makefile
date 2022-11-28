all:
	gcc studentsseq.c -o seq.exe -lm -fopenmp
	gcc studentspar.c -o par.exe -lm -fopenmp

run:
	./seq.exe > seq.out

clean:
	rm *.exe *.out

valgrind:
	valgrind --leak-check=full ./seq.exe
	valgrind --leak-check=full ./par.exe