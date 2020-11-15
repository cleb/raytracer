debug: main.c raytracer.o
	gcc main.c raytracer.o -o rt -lSDL2 -lm -g

rt: main.c raytracer.o
	gcc main.c raytracer.o -o rt -lSDL2 -lm -fopenmp

raytracer.o: src/raytracer.c
	gcc src/raytracer.c -c -o raytracer.o -lm -g