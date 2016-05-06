// Introduction lab that covers:
// * C++
// * SDL
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation
// * glm::vec3 and std::vector

#include "SDL.h"
#include <iostream>
#include "glm/glm.hpp"
#include <vector>
#include "SDLauxiliary.h"

using namespace std;
using glm::vec3;


// --------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Surface* screen;
vector<vec3> stars( 1000 );
int  t ;
float dt;
vec3 color;



// --------------------------------------------------------
// FUNCTION DECLARATIONS
void initial_draw();
void  Draw();
float random_number();
float random_number1();
void Update();
vec3 Interpolate( vec3 a, vec3 b,int i, vector<vec3>& result );

// --------------------------------------------------------
// FUNCTION DEFINITIONS



int main( int argc, char* argv[] )
{
    screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
//    t=SDL_GetTicks();
    
    //Lab 1.3 Starfield
//    initial_draw();
    
    
    while( NoQuitMessageSDL() )
    {
    //Lab 1.3 Starfield
        Draw();
//        Update();
        
    }
    SDL_SaveBMP( screen, "screenshot.bmp" );
    
    
    
    return 0;
}

void Update()
{
    int t2 = SDL_GetTicks();
    dt = float(t2-t);
    t = t2;
    for( int s=0; s<stars.size(); ++s )
    {    // Add code for update of stars
        float V = 0.00025;
        stars[s].z= stars[s].z-V*dt;
        if( stars[s].z <= 0 )
            stars[s].z += 1;
        if( stars[s].z > 1 )
            stars[s].z -= 1;
        color = 0.2f * vec3(1,1,1) / (stars[s].z*stars[s].z);
        //cout<<"z="<<stars[s].z<<endl;
        //cout<<"dt="<<dt<<endl;
    }
}


void Draw()
{
   // Lab 1.2 Bilinear Interpolation
      if( SDL_MUSTLOCK(screen) )
       SDL_LockSurface( screen );
    vector<vec3> result_x1(SCREEN_WIDTH);
    vector<vec3> result_x2(SCREEN_WIDTH);
    vector<vec3> result(SCREEN_HEIGHT);
    	for( int y=0; y<SCREEN_HEIGHT; ++y )
    	{
    
    		for( int x=0; x<SCREEN_WIDTH; ++x )
    		{
                vec3 topLeft(1,0,0); // red
                vec3 topRight(0,0,1); // blue
                vec3 bottomLeft(0,1,0); // green
                vec3 bottomRight(1,1,0); // yellow
    
                vec3 color1 = Interpolate(topLeft, topRight ,x , result_x1);
                vec3 color2 = Interpolate(bottomRight, bottomLeft ,x , result_x2);
                vec3 color = Interpolate(color1, color2, y, result);
    			PutPixelSDL( screen, x, y, color );
    		}
    	}
    
    	if( SDL_MUSTLOCK(screen) )
    		SDL_UnlockSurface(screen);
    
    	SDL_UpdateRect( screen, 0, 0, 0, 0 );
    
    
//    //Lab 1.3 Starfield
//    SDL_FillRect( screen, 0, 0 );
//    if( SDL_MUSTLOCK(screen) )
//        SDL_LockSurface(screen);
//    
//    for( size_t s=0; s<stars.size(); ++s )
//    {
//        
//        
//        float f= float(SCREEN_HEIGHT)/2;
//        float u= f*(stars[s].x/stars[s].z)+SCREEN_WIDTH/2;
//        float v= f*(stars[s].y/stars[s].z)+SCREEN_HEIGHT/2;
//        PutPixelSDL( screen, u, v, color );
//        
//        
//        
//    }
//    if( SDL_MUSTLOCK(screen) )
//        SDL_UnlockSurface(screen);
//    SDL_UpdateRect( screen, 0, 0, 0, 0 );
//    
    
    
}

// Linear Interpolation
vec3 Interpolate( vec3 a, vec3 b, int i, vector<vec3>& result )
{
    
    float fraction= double(i)/SCREEN_WIDTH;
    
    float RED1 = a.x;
    float GREEN1 = a.y;
    float BLUE1 = a.z;
    
    float RED2 = b.x;
    float GREEN2 = b.y;
    float BLUE2 = b.z;
    
    
    float DELTA_RED = RED2 - RED1;
    float DELTA_GREEN = GREEN2 - GREEN1;
    float DELTA_BLUE = BLUE2 - BLUE1;
    
    
    result[i].x = RED1 + (DELTA_RED * fraction);
    result[i].y = GREEN1 + (DELTA_GREEN * fraction);
    result[i].z = BLUE1 + (DELTA_BLUE * fraction);
    
    
    
    return result[i];
    
}

////Produce float random number from 0 to +1
float random_number()
{
    float r = float(rand()) / float(RAND_MAX);
    return r;
}

//Produce float random number from -1 to +1
float random_number1()
{
    double m = (rand()%2?-1:1)*((rand()%1)+(rand()%100)*0.01);
    return m;
    
}

void initial_draw()
{
    
    for( size_t s=0; s<stars.size(); ++s )
    {
        
        // Add code for projecting and drawing each star
        stars[s].x= random_number1();
        stars[s].y= random_number1();
        stars[s].z= random_number();
        //                        cout<<"x="<<stars[s].x<<endl;
        //                        cout<<"y="<<stars[s].y<<endl;
        //                        cout<<"z0="<<stars[s].z<<endl;
        
    }
}

