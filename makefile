c: main.cpp
	g++ main.cpp -o main -O3 -fopenmp

r:
	make c && ./main

o:
	make r && eog grafika.ppm