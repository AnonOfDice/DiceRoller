#include "Definitions.h"


int main( int argc, char* args[] )

{
    srand(time(NULL));


    printf("\n");
    printf("\u250F\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501");
    printf("\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2513\n");
    printf("\u2503                                                          \u2503\n");
    printf("\u2503                    CUSTOM DICE ROLLER                    \u2503\n");
    printf("\u2503                     for you, by Anon                     \u2503\n");
    printf("\u2503                                                          \u2503\n");
    printf("\u2503                          v1.1.4                          \u2503\n");
    printf("\u2503                                                          \u2503\n");
    printf("\u2517\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501");
    printf("\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u251B\n");

    getchar();
    clearscreen();

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() ); sdlLoaded=false;};
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) ){printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );sdlLoaded = false;};
    totalpix=(int)countpictures();

    programstate* mystate = new programstate;
    mystate->initialize();

    mystate->loop();

    delete mystate;
    SDL_Quit();



    return 0;
};
