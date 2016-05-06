#include <iostream>
#include "glm/glm.hpp"
#include <SDL.h>
#include <algorithm>
#include "SDLauxiliary.h"
#include "TestModel.h"


using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES
// Animation const
const int SCREEN_WIDTH = 100;
const int SCREEN_HEIGHT = 100;


//Image const
//const int SCREEN_WIDTH = 500;
//const int SCREEN_HEIGHT = 500;

SDL_Surface* screen;
int t;
vector<Triangle> triangles;
float focalLength = 1.2f*SCREEN_HEIGHT;
vector<vec3> solutions(30);
vec3 cameraPos(0,0,- ((2 * focalLength / SCREEN_HEIGHT) + 1));
vec3 lightPos( 0, -0.5, -0.7);
vec3 lightColor = 14.f * vec3( 1, 1, 1 );


vec3 v0 ;
vec3 v1;
vec3 v2 ;
vec3 e1 ;
vec3 e2 ;
vec3 b ;


struct Intersection
{
    vec3 position;
    float distance;
    int triangleIndex;
};


// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
void CalculateIntersaction(vector<Triangle>& triangles, vec3 start, vec3 dir,  vector<vec3>& solutions);
bool ClosestIntersection(vector<vec3>& solutions , Intersection& closestIntersection, int ignoreindex );
vec3 Light( const Intersection& intersecting );



int main( int argc, char* argv[] )
{
    screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
    t = SDL_GetTicks();	// Set start value for timer.
    
    LoadTestModel (triangles);
    
    while( NoQuitMessageSDL() )
    {
        Update();
        Draw();
    }
    
    SDL_SaveBMP( screen, "screenshot.bmp" );
    return 0;
}

void Update()
{
    // Compute frame time:
    int t2 = SDL_GetTicks();
    float dt = float(t2-t);
    t = t2;
    cout << "Render time: " << dt << " ms." << endl;
    
    Uint8* keystate = SDL_GetKeyState( 0 );
    mat3 R;
    float xaw;
    float yaw;
    vec3 right( R[0][0], R[0][1], R[0][2] );
    vec3 down( R[1][0], R[1][1], R[1][2] );
    vec3 forward( R[2][0], R[2][1], R[2][2] );
    
    
    
    if( keystate[SDLK_UP] )
    {
        // Move camera forward
        xaw = 5.0f*M_PI/(-180.0f);
        right=glm::vec3(1,0,0);
        down= glm::vec3(0,cos(xaw),-sin(xaw));
        forward=glm::vec3(0,sin(xaw),cos(xaw));
        R=glm::mat3(right,down,forward);
        cameraPos=R*cameraPos;
    }
    if( keystate[SDLK_DOWN] )
    {
        // Move camera backward
        xaw = 5.0f*M_PI/180.0f;
        right=glm::vec3(1,0,0);
        down= glm::vec3(0,cos(xaw),-sin(xaw));
        forward=glm::vec3(0,sin(xaw),cos(xaw));
        R=glm::mat3(right,down,forward);
        cameraPos=R*cameraPos;
    }
    if( keystate[SDLK_LEFT] )
    {
        // Move camera to the left
        yaw = 5.0f*M_PI/180.0f;
        right=glm::vec3(cos(yaw),0,sin(yaw));
        down= glm::vec3(0,1,0);
        forward=glm::vec3(-sin(yaw),0,cos(yaw));
        R=glm::mat3(right,down,forward);
        cameraPos=R*cameraPos;
        
    }
    if( keystate[SDLK_RIGHT] )
    {
        // Move camera to the right
        yaw = 5.0f*M_PI/(-180.0f);
        right=glm::vec3(cos(yaw),0,sin(yaw));
        down= glm::vec3(0,1,0);
        forward=glm::vec3(-sin(yaw),0,cos(yaw));
        R=glm::mat3(right,down,forward);
        cameraPos=R*cameraPos;
        
    }
    
    if( keystate[SDLK_w] )
    {
        glm::vec3 forward(0,0,-0.1);
        if(lightPos.z>-1.0)
            lightPos += forward;
        
    }
    
    
    
    if( keystate[SDLK_s] )
    {
        glm::vec3 back(0,0,0.1);
        
        if(lightPos.z<1.0)
            lightPos += back;
        
    }
    
    
    if( keystate[SDLK_a] )
    {
        
        glm::vec3 left(-0.1,0,0);
        if(lightPos.x> -1.0)
            lightPos += left;
        
    }
    
    
    
    
    if( keystate[SDLK_d] )
    {
        glm::vec3 right(0.1,0,0);
        if(lightPos.x<1.0)
            lightPos += right;
        
    }
    
    
    if( keystate[SDLK_q] )
    {
        glm::vec3 up(0,-0.1,0);
        if(lightPos.y> -1.0)
            lightPos += up;
        
    }
    
    
    if( keystate[SDLK_e] )
    {
        glm::vec3 down(0,0.1,0);
        if(lightPos.y< 1.0)
            lightPos += down;
        
    }
    
    
    
}


void Draw()
{
    if( SDL_MUSTLOCK(screen) )
        SDL_LockSurface(screen);
    
    
    
    for( int y=0; y<SCREEN_HEIGHT; ++y )
    {   cout<<"y="<<y<<endl;
        for( int x=0; x<SCREEN_WIDTH; ++x )
        {
            
            
            
            vec3 direction(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2, focalLength);
            Intersection closestintersection;
            int Duplicateindex = 31;
            
            
            CalculateIntersaction(triangles, cameraPos, direction, solutions);
            
            
            if (ClosestIntersection(solutions, closestintersection, Duplicateindex) == true)
            {
                
                
                
                vec3 colorx = Light(closestintersection);
                int index = closestintersection.triangleIndex;
                vec3 colory = triangles[index].color;
                
//add the diffuse effect
               vec3 color = colorx * colory;
                PutPixelSDL( screen, x, y, color);
            }
            else
            {
                vec3 color2(0,0,0);
                
                
                PutPixelSDL( screen, x, y, color2);
            }
            
            
            
        }
    }
    
    if( SDL_MUSTLOCK(screen) )
        SDL_UnlockSurface(screen);
    
    SDL_UpdateRect( screen, 0, 0, 0, 0 );
}


void CalculateIntersaction(vector<Triangle>& triangles, vec3 start, vec3 dir, vector<vec3>& solutions)
{
    
    for( size_t i=0; i<triangles.size(); ++i )
    {
        //  cout<<"no="<<i<<endl;
        v0 = triangles[i].v0;
        v1 = triangles[i].v1;
        v2 = triangles[i].v2;
        e1 = v1 - v0;
        e2 = v2 - v0;
        b = start - v0;
        mat3 A( -dir, e1, e2 );
        
        
        //
        //        cout<<"e1=("<<e1.x<<", "<<e1.y<<", "<<e1.z<<")"<<endl;
        //        cout<<"e2=("<<e2.x<<", "<<e2.y<<", "<<e2.z<<")"<<endl;
        //        cout<<"b=("<<b.x<<", "<<b.y<<", "<<b.z<<")"<<endl;
        
        //solutions[i] = glm::inverse( A ) * b;
        solutions[i] = glm::inverse( A ) * b;
        
        
        
        
    }
    
}

bool ClosestIntersection(vector<vec3> &solutions , Intersection& closestIntersection ,int ignoreindex)
{
    closestIntersection.distance = std::numeric_limits<float>::max();
    bool flag = 0;
    for( size_t i=0; i<solutions.size(); ++i )
    {
        
        
        
        if(solutions[i].x > 0 && solutions[i].y >= 0 && solutions[i].z >= 0 && solutions[i].y+solutions[i].z <= 1 && ignoreindex != i)
        {
            if(solutions[i].x<closestIntersection.distance)
            {
                closestIntersection.distance=solutions[i].x;
                
                closestIntersection.triangleIndex = i;
                
                glm::vec3 e1 = triangles[i].v1-triangles[i].v0;
                glm::vec3 e2 = triangles[i].v2-triangles[i].v0;
                
                closestIntersection.position = triangles[i].v0+solutions[i].y*e1+solutions[i].z*e2;
            }
            
            
            
            flag = 1;
            
            
            
        }
        
        
        
    }
    return flag;
}



//return the color after direct illumination and indirect illumination
//add the effect of direct shadow
vec3 Light( const Intersection& in )
{
    
    vec3 Normal = triangles[in.triangleIndex].normal;
    vec3 Radium =glm::normalize(lightPos - in.position);
    float a=glm::dot(Normal, Radium);
    float b=0.0;
    float RR = glm::distance(lightPos, in.position);
    float d= 1.0f/(4.0f*M_PI*RR*RR);
    vec3 directlight = lightColor* d * max(a,b);
    
    vec3 indirectLight = 0.5f*vec3( 1, 1, 1 );
    
    
    
 //   direct shadow
    Intersection closestintersection2;
    vector<vec3> solutions2(30);
    CalculateIntersaction(triangles, in.position, Radium, solutions2);
    
    
    if (ClosestIntersection(solutions2 , closestintersection2, in.triangleIndex)==true)
        
    {
        
        
        
        if (closestintersection2.distance < RR  )
            
            directlight= glm::vec3(0,0,0);
    }
    
    
    
    
   return directlight+indirectLight;
   // return directlight;
}
