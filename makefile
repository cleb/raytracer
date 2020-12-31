CC=gcc
release: main.c raytracer.o texture_loader.o texture.o angle.o render_canvas.o render_line.o render_polygon_2d.o map.o
	gcc main.c raytracer.o texture_loader.o texture.o angle.o render_canvas.o render_line.o render_polygon_2d.o map.o -o release -lSDL2 -lSDL2_image -lm -g -O3 -fopenmp -march=native

raytracer.o: src/raytracer.c
	gcc src/raytracer.c -c -o raytracer.o -lm -g -fopenmp -O3 -march=native

texture_loader.o: src/texture_loader.c
	gcc src/texture_loader.c -c -o texture_loader.o -g -O3 -march=native

texture.o: src/texture.c
	gcc src/texture.c -c -o texture.o -g -O3 -march=native

render_canvas.o: src/render_canvas.c
	gcc src/render_canvas.c -c -o render_canvas.o -g -O3 -march=native

render_line.o: src/render_line.c
	gcc src/render_line.c -c -o render_line.o -g -O3 -march=native

render_polygon_2d.o: src/render_polygon_2d.c
	gcc src/render_polygon_2d.c -c -o render_polygon_2d.o -g -O3 -march=native

angle.o: src/angle.c 
	gcc src/angle.c -c -o angle.o -g -O3 -march=native

map.o: src/map.c 
	gcc src/map.c -c -o map.o -g -O3 -march=native


#tests

runtest: test/test.c test/test_raytracer.o test/test_render_line.o test/test_render_polygon_2d.o raytracer.o texture.o angle.o render_canvas.o render_line.o render_polygon_2d.o
	gcc test/test.c test/test_raytracer.o test/test_render_line.o test/test_render_polygon_2d.o raytracer.o texture.o angle.o render_canvas.o render_line.o render_polygon_2d.o -o runtest -lcheck -lpthread -lm -lrt -lsubunit

test_raytracer.o: test/test_raytracer.c 
	gcc test/test_raytracer.c -c -o test_raytracer.o

test_render_line.o: test/test_render_line.c 
	gcc test/test_render_line.c -c  -o test_render_line.o

test_render_polygon_2d.o: test/test_render_polygon_2d.c 
	gcc test/test_render_polygon_2d.c -c  -o test_render_polygon_2d.o