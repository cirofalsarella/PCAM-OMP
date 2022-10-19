seq:
	gcc studentsseq.c -o seq.exe -lm

run:
	./seq.exe > seq.out

clean:
	rm *.exe *.out

valgrind:
	valgrind --leak-check=full ./seq.exe