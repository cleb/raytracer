debug: main.c raytracer.o texture_loader.o texture.o angle.o
	gcc main.c raytracer.o texture_loader.o texture.o angle.o -o rt -lSDL2 -lSDL2_image -lm -g -fopenmp

release: main.c raytracer.o texture_loader.o texture.o angle.o
	gcc main.c raytracer.o texture_loader.o texture.o angle.o -o release -lSDL2 -lSDL2_image -lm -O3 -fopenmp

raytracer.o: src/raytracer.c
	gcc src/raytracer.c -c -o raytracer.o -lm -g -fopenmp

texture_loader.o: src/texture_loader.c
	gcc src/texture_loader.c -c -o texture_loader.o -g

texture.o: src/texture.c
	gcc src/texture.c -c -o texture.o -g

runtest: test/test_raytracer.c raytracer.o texture.o angle.o
	gcc test/test_raytracer.c raytracer.o texture.o angle.o -o runtest -lcheck -lpthread -lm -lrt -lsubunit