#include "map.h"
#include "stdlib.h"
#include "texture_loader.h"

Polygon_2D *create_square_polygon(Line l1, Line l2, Line l3, Line l4) {
    Line *lines = (Line *)malloc(4 * sizeof(Line));
    lines[0] = l1;
    lines[1] = l2;
    lines[2] = l3;
    lines[3] = l4;

    Polygon_2D *polygon = (Polygon_2D *)malloc(sizeof(Polygon_2D));
    polygon->lines = lines;
    polygon->numLines = 4;
    return polygon;
}

Map *load_map(char *filename) {
    //stub, all geometry is contained here, no loading implemented

    Texture *wall = load_texture("./wall.jpg");
    Texture *texture_floor = load_texture("./floor.jpg");
    Texture *grass = load_texture("./grass.jpg");
    Texture *glass = load_texture("./glass.png");
    Texture *wood = load_texture("./wood.jpg");
    Texture *flowers = load_texture("./flowers.jpg");
    Texture **textures = (Texture **)malloc(6 * sizeof(Texture *));
    textures[0] = wall;
    textures[1] = texture_floor;
    textures[2] = grass;
    textures[3] = glass;
    textures[4] = wood;
    textures[5] = flowers;

    //map geometry
    Point p1 = {.x = -400, .y = 400};
    Point p2 = {.x = 400, .y = 400};
    Line l1 = {.start = p1, .end = p2};

    Point p3 = {.x = -400, .y = 800};
    Point p4 = {.x = 400, .y = 800};
    Line l2 = {.start = p3, .end = p4};

    Point p5 = {.x = -400, .y = 400};
    Point p6 = {.x = -400, .y = 800};
    Line l3 = {.start = p5, .end = p6};

    Point p7 = {.x = 400, .y = 400};
    Point p8 = {.x = 400, .y = 800};
    Line l4 = {.start = p7, .end = p8};

    Point fp1 = {.x = -1000, .y = -1000};
    Point fp2 = {.x = 1000, .y = -1000};
    Line fl1 = {.start = fp1, .end = fp2};

    Point fp3 = {.x = -1000, .y = 400};
    Point fp4 = {.x = 1000, .y = 400};
    Line fl2 = {.start = fp3, .end = fp4};

    Point fp5 = {.x = -1000, .y = -1000};
    Point fp6 = {.x = -1000, .y = 400};
    Line fl3 = {.start = fp5, .end = fp6};

    Point fp7 = {.x = 1000, .y = -1000};
    Point fp8 = {.x = 1000, .y = 400};
    Line fl4 = {.start = fp7, .end = fp8};

    Point f2p1 = {.x = 400, .y = -1000};
    Point f2p2 = {.x = 1000, .y = -1000};
    Line f2l1 = {.start = f2p1, .end = f2p2};

    Point f2p3 = {.x = 400, .y = 1000};
    Point f2p4 = {.x = 1000, .y = 1000};
    Line f2l2 = {.start = f2p3, .end = f2p4};

    Point f2p5 = {.x = 400, .y = -1000};
    Point f2p6 = {.x = 400, .y = 1000};
    Line f2l3 = {.start = f2p5, .end = f2p6};

    Point f2p7 = {.x = 1000, .y = -1000};
    Point f2p8 = {.x = 1000, .y = 1000};
    Line f2l4 = {.start = f2p7, .end = f2p8};

    Point f3p1 = {.x = -1000, .y = -1000};
    Point f3p2 = {.x = 400, .y = -1000};
    Line f3l1 = {.start = f3p1, .end = f3p2};

    Point f3p3 = {.x = -1000, .y = 1000};
    Point f3p4 = {.x = 400, .y = 1000};
    Line f3l2 = {.start = f3p3, .end = f3p4};

    Point f3p5 = {.x = -1000, .y = -1000};
    Point f3p6 = {.x = -1000, .y = 1000};
    Line f3l3 = {.start = f3p5, .end = f3p6};

    Point f3p7 = {.x = 400, .y = -1000};
    Point f3p8 = {.x = 400, .y = 1000};
    Line f3l4 = {.start = f3p7, .end = f3p8};

    Point gp1 = {.x = 100, .y = 0};
    Point gp2 = {.x = 200, .y = 0};
    Point gp3 = {.x = 300, .y = 100};
    Point gp4 = {.x = 300, .y = 200};
    Point gp5 = {.x = 200, .y = 300};
    Point gp6 = {.x = 100, .y = 300};
    Point gp7 = {.x = 0, .y = 200};
    Point gp8 = {.x = 0, .y = 100};

    Line gl1 = {.start=gp1, .end=gp2};
    Line gl2 = {.start=gp2, .end=gp3};
    Line gl3 = {.start=gp3, .end=gp4};
    Line gl4 = {.start=gp5, .end=gp4};
    Line gl5 = {.start=gp6, .end=gp5};
    Line gl6 = {.start=gp7, .end=gp6};
    Line gl7 = {.start=gp8, .end=gp7};
    Line gl8 = {.start=gp8, .end=gp1};

    Wall gw1 = {.line = gl1, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    Wall gw2 = {.line = gl2, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    Wall gw3 = {.line = gl3, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    Wall gw4 = {.line = gl4, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    Wall gw5 = {.line = gl5, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    Wall gw6 = {.line = gl6, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    Wall gw7 = {.line = gl7, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    Wall gw8 = {.line = gl8, .bottom = 0, .top = 20, .texture=wood, .reflexivity = 0};
    


    

    Wall w1 = {.line = l1, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w2 = {.line = l2, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall w3 = {.line = l3, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w4 = {.line = l4, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall *walls = (Wall *)malloc(12 *sizeof(Wall));
    walls[0] = w1;
    walls[1] = w2;
    walls[2] = w3;
    walls[3] = w4;
    walls[4] = gw1;
    walls[5] = gw2;
    walls[6] = gw3;
    walls[7] = gw4;
    walls[8] = gw5;
    walls[9] = gw6;
    walls[10] = gw7;
    walls[11] = gw8;

    Polygon_2D *garden_polygon = (Polygon_2D *)malloc(sizeof(Polygon_2D));
    Line *garden_lines = (Line *)malloc(8 * sizeof(Line));
    garden_lines[0] = gl1;
    garden_lines[1] = gl2;
    garden_lines[2] = gl3;
    garden_lines[3] = gl4;
    garden_lines[4] = gl5;
    garden_lines[5] = gl6;
    garden_lines[6] = gl7;
    garden_lines[7] = gl8;

    garden_polygon->lines = garden_lines;
    garden_polygon->numLines = 8;

    Surface garden = {.polygon = garden_polygon, .texture = flowers, .z = 20};

    Surface floor_inside = {.polygon = create_square_polygon(l1,l2,l3,l4), .texture = texture_floor, .z = 0};

    Surface ceiling_inside = {.polygon = create_square_polygon(l1,l2,l3,l4), .texture = texture_floor, .z = 200};

    Surface floor_outside = {.polygon = create_square_polygon(fl1,fl2,fl3,fl4), .texture = grass, .z = 0};

    Surface floor_outside_2 = {.polygon = create_square_polygon(f2l1,f2l2,f2l3,f2l4), .texture = grass, .z = 0};

    Surface floor_outside_3 = {.polygon = create_square_polygon(f3l1,f3l2,f3l3,f3l4), .texture = grass, .z = 0};

    Surface * surfaces = (Surface *)malloc(6 * sizeof(Surface));
    surfaces[0] = floor_outside;
    surfaces[1] = floor_outside_2;
    surfaces[2] = floor_outside_3;
    surfaces[3] = floor_inside;
    surfaces[4] = ceiling_inside;
    surfaces[5] = garden;

    Map *map = (Map *)malloc(sizeof(Map));
    map->textures = textures;
    map->num_textures = 6;
    map->walls = walls;
    map->num_walls = 12;
    map->surfaces = surfaces;
    map->num_surfaces = 6;
    return map;
}

void destroy_map(Map *map) {
    for(int i = 0; i < map->num_textures; i++) {
        destroy_texture(map->textures[i]);
    }
    free(map->textures);
    for(int i = 0; i < map->num_surfaces; i++) {
        free(map->surfaces[i].polygon);
    }
    free(map->surfaces);
    free(map->walls);
}
