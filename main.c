#include <stdio.h>
#include <SDL.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FIELD_OF_VIEW 45
#define PI 3.1415926535

SDL_Window *G_Window = NULL;
SDL_Renderer *G_Renderer = NULL;
SDL_Surface *G_Surface = NULL;
SDL_Texture *G_Texture = NULL;

typedef struct
{
    double x;
    double y;
    double z;
} Point3D;

struct cube_t
{
    //struct Point3D GetPosition();

    Point3D position;
    Point3D speed;
    Point3D rotation;

    Point3D points[8];
};
struct cube_t cube;

double Z0;

double getZ0();
Point3D Create(double x, double y, double z);
Point3D Translate(Point3D original, Point3D translation);
Point3D Rotate(Point3D original, Point3D rotation);
Point3D ApplyPerspective(Point3D original);
Point3D CenterScreen(Point3D original);

void init();
void draw();
void close();

int main(int argc, char* argv[])
{

    int fps = 60;
    int desiredDelta = 1000/fps;

    //Gameloop flag
    bool is_running = true;

    //Init everything
    init();

    //Infinite loop
    while (is_running)
    {
        int startTime = SDL_GetTicks();

        //Rotate Cube
        cube.rotation.x += 0.02;
        cube.rotation.y += 0.02;
        cube.rotation.z += 0.02;

        //Draw
        draw();

        int delta = SDL_GetTicks() - startTime;
        if(delta < desiredDelta)
        {
            SDL_Delay(desiredDelta - delta);
        }
    }

    //Close everything
    close();

    return 0;
}

void init()
{
    //Init SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Could not init SDL2! Error: %s", SDL_GetError());
        return;
    }

    //Create Window
    G_Window = SDL_CreateWindow("SDL2 3D cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (G_Window == NULL)
    {
        printf("Could not init SDL2! Error: %s", SDL_GetError());
        return;
    }

    //Create Renderer
    G_Renderer = SDL_CreateRenderer(G_Window, -1, SDL_RENDERER_ACCELERATED);
    if (G_Renderer == NULL)
    {
        printf("Could not create a Renderer! Error: %s", SDL_GetError());
        return;
    }

    Z0 = (SCREEN_WIDTH/2.0)/tan((FIELD_OF_VIEW/2.0)*PI/180.0);

    cube.position = Create(0, 0, 400);
    cube.rotation = Create(0, 0, 0);

    cube.points[0] = Create(-200, -200, -200);
    cube.points[1] = Create(-200, 200, -200);
    cube.points[2] = Create(200, 200, -200);
    cube.points[3] = Create(200, -200, -200);
    cube.points[4] = Create(-200, -200, 200);
    cube.points[5] = Create(-200, 200, 200);
    cube.points[6] = Create(200, 200, 200);
    cube.points[7] = Create(200, -200, 200);

}

//Draw
void draw()
{
    Point3D drawPoints[8];

    //Rotate
    for(int i=0; i < 8; i++)
    {
        drawPoints[i] = Rotate(cube.points[i], cube.rotation);
    }

    //Translate
    for(int i=0; i < 8; i++)
    {
        drawPoints[i] = Translate(drawPoints[i], cube.position);
    }

    //Apply Perspective
    for(int i=0; i < 8; i++)
    {
        drawPoints[i] = ApplyPerspective(drawPoints[i]);
    }

    //Center Screen
    for(int i=0; i < 8; i++)
    {
        drawPoints[i] = CenterScreen(drawPoints[i]);
    }

    SDL_SetRenderDrawColor(G_Renderer, 0, 0, 0, 255);
    SDL_RenderClear(G_Renderer);

    //Draw Lines
    SDL_SetRenderDrawColor(G_Renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(G_Renderer, drawPoints[0].x, drawPoints[0].y, drawPoints[1].x, drawPoints[1].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[1].x, drawPoints[1].y, drawPoints[2].x, drawPoints[2].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[2].x, drawPoints[2].y, drawPoints[3].x, drawPoints[3].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[3].x, drawPoints[3].y, drawPoints[0].x, drawPoints[0].y);

    SDL_RenderDrawLine(G_Renderer, drawPoints[4].x, drawPoints[4].y, drawPoints[5].x, drawPoints[5].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[5].x, drawPoints[5].y, drawPoints[6].x, drawPoints[6].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[6].x, drawPoints[6].y, drawPoints[7].x, drawPoints[7].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[7].x, drawPoints[7].y, drawPoints[4].x, drawPoints[4].y);

    SDL_RenderDrawLine(G_Renderer, drawPoints[0].x, drawPoints[0].y, drawPoints[4].x, drawPoints[4].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[1].x, drawPoints[1].y, drawPoints[5].x, drawPoints[5].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[2].x, drawPoints[2].y, drawPoints[6].x, drawPoints[6].y);
    SDL_RenderDrawLine(G_Renderer, drawPoints[3].x, drawPoints[3].y, drawPoints[7].x, drawPoints[7].y);

    SDL_RenderPresent(G_Renderer);
}

//Close everything
void close()
{
    SDL_DestroyRenderer(G_Renderer);
    SDL_DestroyWindow(G_Window);
    SDL_Quit();
}

//Get the Z0 value
double getZ0()
{
    return Z0;
}

//Create a vertex
Point3D Create(double x, double y, double z)
{
        Point3D toReturn;
        toReturn.x = x;
        toReturn.y = y;
        toReturn.z = z;
        return toReturn;
}

//Translate coordinates
Point3D Translate(Point3D original, Point3D translation)
{
    Point3D toReturn;
    toReturn.x = original.x + translation.x;
    toReturn.y = original.y + translation.y;
    toReturn.z = original.z + translation.z;
    return toReturn;
}

//Rotate a 3D object
Point3D Rotate(Point3D original, Point3D rotation)
{
    Point3D toReturn;
    toReturn.x = original.x * (cos(rotation.z) * cos(rotation.y)) +
				 original.y * (cos(rotation.z) * sin(rotation.y) * sin(rotation.x) - sin(rotation.z) * cos(rotation.x)) +
				 original.z * (cos(rotation.z) * sin(rotation.y) * cos(rotation.x) + sin(rotation.z) * sin(rotation.x));

    toReturn.y = original.x * (sin(rotation.z) * cos(rotation.y)) +
				 original.y * (sin(rotation.z) * sin(rotation.y) * sin(rotation.x) + cos(rotation.z) * cos(rotation.x)) +
				 original.z * (sin(rotation.z) * sin(rotation.y) * cos(rotation.x) - cos(rotation.z) * sin(rotation.x));

    toReturn.z = original.x * (- sin(rotation.y)) +
				 original.y * (cos(rotation.y) * sin(rotation.x)) +
				 original.z * (cos(rotation.y) * cos(rotation.x));


    return toReturn;
}

//Apply Perspective in the projection
Point3D ApplyPerspective(Point3D original)
{
    Point3D toReturn;
    toReturn.x = original.x * Z0/(Z0+original.z);
    toReturn.y = original.y * Z0/(Z0+original.z);
    toReturn.z = original.z;
    return toReturn;
}

//Center Screen
Point3D CenterScreen(Point3D original)
{
    Point3D toReturn;
    toReturn.x = original.x + SCREEN_WIDTH/2;
    toReturn.y = original.y + SCREEN_HEIGHT/2;
    toReturn.z = original.z;
    return toReturn;
}
