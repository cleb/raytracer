debug: main.c raytracer.o texture_loader.o texture.o angle.o render_canvas.o
	gcc main.c raytracer.o texture_loader.o texture.o angle.o render_canvas.o -o rt -lSDL2 -lSDL2_image -lm -g -fopenmp

release: main.c raytracer.o texture_loader.o texture.o angle.o render_canvas.o render_line.o
	gcc main.c raytracer.o texture_loader.o texture.o angle.o render_canvas.o render_line.o -o release -lSDL2 -lSDL2_image -lm -O3 -fopenmp

raytracer.o: src/raytracer.c
	gcc src/raytracer.c -c -o raytracer.o -lm -g -fopenmp

texture_loader.o: src/texture_loader.c
	gcc src/texture_loader.c -c -o texture_loader.o -g

texture.o: src/texture.c
	gcc src/texture.c -c -o texture.o -g

render_canvas.o: src/render_canvas.c
	gcc src/render_canvas.c -c -o render_canvas.o -g

render_line.o: src/render_line.c
	gcc src/render_line.c -c -o render_line.o -g

#tests

runtest: test/test.c test/test_raytracer.o test/test_render_line.o raytracer.o texture.o angle.o render_canvas.o render_line.o
	gcc test/test.c test/test_raytracer.o test/test_render_line.o raytracer.o texture.o angle.o render_canvas.o render_line.o -o runtest -lcheck -lpthread -lm -lrt -lsubunit

test_raytracer.o: test/test_raytracer.c 
	gcc test/test_raytracer.c -c -o test_raytracer.o

test_render_line.o: test/test_render_line.c 
	gcc test/test_ender_line.c -c  -o test_render_line.o
