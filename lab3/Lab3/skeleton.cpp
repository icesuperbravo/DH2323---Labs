#include <iostream>
#include "glm/glm.hpp"
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::ivec2;
using glm::mat3;
using glm::vec2;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
vec3 cameraPos( 0, 0, -3.001 );
int focal= 500 ;
mat3 R=glm::mat3(1.0, 0, 0,
                 0, 1.0, 0,
                 0, 0, 1.0 );

float xaw = 0;
float yaw = 0;

vector<vec3> localPos;
float depthBuffer [SCREEN_HEIGHT][SCREEN_WIDTH];
struct PIXEL
{
    ivec2 position2D;
    float zinv;
    //  vec3 illumination;
    vec3 position3D;
};

struct VERTEX
{
    //vec3 position;
    vec3 Normal;
    vec3 reflectance;
    
};


vec3 lightPos(0,-0.5,-0.7);
vec3 lightPower = 14.f*vec3( 1, 1, 1 );
vec3 indirectLightPowerPerArea = 0.5f*vec3( 1, 1, 1 );

VERTEX vertex;


// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
glm::ivec2 VertexShader( glm::vec3 vertice );
void Interpolate( PIXEL a, PIXEL b, vector<PIXEL>& result );
//void DrawLineSDL( SDL_Surface* surface, ivec2 a, ivec2 b, vec3 color );
//void DrawPolygonEdges( int indexTri);
void ComputePolygonRows(vector<PIXEL>& leftPixels, vector<PIXEL>& rightPixels,vector<PIXEL>&projectedVertice, int ROWS);
void CalculatingLeftRight(vector<PIXEL>& leftPixels, vector<PIXEL>& rightPixels, PIXEL a, PIXEL b, int j);
vec3 Rotation(vec3 vertex);


void DrawPolygonRows( int ROWS, const vector<PIXEL>& leftPixels, const vector<PIXEL>& rightPixels);
void DrawPolygon(int indexTri);
vec3 Lighting(int indexTri,int indexVer,vector<glm::vec3>& vertice);
vec3 LightingPixel(vec3 Position,VERTEX vertex);



//------------------------------------------------------------------------------
//code
int main( int argc, char* argv[] )
{
    LoadTestModel( triangles );
    screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
    t = SDL_GetTicks();	// Set start value for timer.
    
    localPos.clear();
    localPos.reserve( 5*2*3*3 );
    
    
    
    
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
    
    Uint8* keystate = SDL_GetKeyState(0);
    
    if( keystate[SDLK_UP] )
    {
        xaw += 1.5f*M_PI/(-180.0f);
        
        
        
    }
    
    if( keystate[SDLK_DOWN] )
    {
        xaw += 1.5f*M_PI/(180.0f);
        
          }
    
    if( keystate[SDLK_RIGHT] )
    {
        
        yaw += 1.5f*M_PI/(180.0f);
        
        
    }
    
    if( keystate[SDLK_LEFT] )
    {
        
        yaw += 1.5f*M_PI/(-180.0f);
        
    }
    
    if( keystate[SDLK_RSHIFT] )
    {
    
    }
    
    if( keystate[SDLK_RCTRL] )
    {
    
    }
    
    if( keystate[SDLK_w] )
    {
        lightPos.y-=0.1;
    }
    
    if( keystate[SDLK_s] )
    {
        lightPos.y+=0.1;
    }
 
    
    if( keystate[SDLK_d] )
    {
        lightPos.x+=0.1;
    }

    
    if( keystate[SDLK_a] )
    {
        lightPos.x-=0.1;
    }
    
    if( keystate[SDLK_e] )
    {
        lightPos.z+=0.1;
    }
    

    
    if( keystate[SDLK_q] )
    {
        lightPos.z-=0.1;
    }
    
}

void Draw()
{
    SDL_FillRect( screen, 0, 0 );
    
    for( int y=0; y<SCREEN_HEIGHT; ++y )
        for( int x=0; x<SCREEN_WIDTH; ++x )
            depthBuffer[y][x] = 0;
    
    
    
    if( SDL_MUSTLOCK(screen) )
        SDL_LockSurface(screen);
    int indexTri;
    for( indexTri=0; indexTri<triangles.size(); ++indexTri)
    {
        
        
        
        
        
        // 3.0 Vertices points
        //                for (int v=0; v<vertices.size(); ++v) {
        //                    ivec2 projPos;
        //                    projPos=VertexShader(vertices[v]);
        //                    vec3 color(1,1,1);
        //                    PutPixelSDL( screen, projPos.x, projPos.y, color );
        //                }
        //
        
        //3.1 Polygon Edges
        //DrawPolygonEdges(indexTri);
        
        DrawPolygon(indexTri);
        
    }
    
    if ( SDL_MUSTLOCK(screen) )
        SDL_UnlockSurface(screen);
    SDL_UpdateRect( screen, 0, 0, 0, 0 );
}


glm::ivec2 VertexShader(glm::vec3 vertice)
{
    
    
    vec3 locVer=vertice-cameraPos;
    
    ivec2 projPosition;
    
    
    projPosition.x = focal*locVer.x/locVer.z+SCREEN_WIDTH/2;
    
    projPosition.y = focal*locVer.y/locVer.z+SCREEN_HEIGHT/2;
    
    
    return projPosition;
    
}



void Interpolate( PIXEL a, PIXEL b, vector<PIXEL>& result)
{
    
    int N = result.size();
    
    vec2 step1 = vec2(b.position2D-a.position2D) / float(max(N-1,1));
    vec2 current1= vec2(a.position2D);
    
    float step2 = (b.zinv-a.zinv)/float(max(N-1,1));
    float current2 = a.zinv;
    
    //    vec3 step3 = (b.illumination - a.illumination)/ float(max(N-1,1));
    //    vec3 current3 = a.illumination;
    
    vec3 step4 =(b.position3D - a.position3D)/ float(max(N-1,1));
    vec3 current4= a.position3D;
    
    
    for( int i=0; i<N; ++i )
    {
        result[i].position2D =ivec2(current1);
        result[i].zinv = current2;
        //        result[i].illumination = current3;
        result[i].position3D = current4;
        current1 = step1+current1;
        current2 = step2+current2;
        //        current3 = step3+current3;
        current4 = step4+current4;
    }
}


//void DrawLineSDL( SDL_Surface* surface, ivec2 a, ivec2 b, vec3 color )
//{
//    ivec2 delta = glm::abs( a - b );
//    int pixels = glm::max( delta.x, delta.y ) + 1;
//    vector<ivec2> line (pixels);
//    Interpolate( a, b, line );
//    for(int i=0; i<pixels; ++i)
//    {
//        PutPixelSDL( screen, line[i].x, line[i].y, color);
//    }
//
//}


//void DrawPolygonEdges(int indexTri)
//{
//    vector<glm::ivec2> projectedVertice(3);
//    for( int indexVer=0; indexVer<3; ++indexVer )
//    {
//        projectedVertice[indexVer]= VertexShader(indexTri, indexVer);
//    }
//    for( int i=0; i<3; ++i )
//    {
//        int j = (i+1)%3; // The next vertex
//        vec3 color(1,1,1);
//        DrawLineSDL(screen, projectedVertice[i], projectedVertice[j], color);
//
//    }
//
//}


void ComputePolygonRows(vector<PIXEL>& leftPixels, vector<PIXEL>& rightPixels,vector<PIXEL>&projectedVertice, int ROWS)
{
    
    int R;
    for(R=0; R<ROWS; ++R)
        
    {     for( int i=0; i<3; ++i )
    {
        int j = (i+1)%3; // The next vertex
        CalculatingLeftRight(leftPixels, rightPixels,projectedVertice[i],projectedVertice[j],R);
    }
    }

}


void CalculatingLeftRight(vector<PIXEL>& leftPixels, vector<PIXEL>& rightPixels, PIXEL a, PIXEL b, int j)
{
    ivec2 delta = glm::abs( a.position2D - b.position2D );
    int pixels = glm::max( delta.x, delta.y ) + 1;
    vector<PIXEL> line (pixels);
    Interpolate(a , b, line);
    
    for (int i=0; i<line.size(); ++i)
    {
        if(line[i].position2D.y == leftPixels[j].position2D.y)
        {
            if(line[i].position2D.x<leftPixels[j].position2D.x)
            {
                leftPixels[j]=line[i];
            }
            if(line[i].position2D.x>rightPixels[j].position2D.x)
            {
                rightPixels[j]=line[i];
            }
        }
        
    }
    
    
}




void DrawPolygonRows( int ROWS, const vector<PIXEL>& leftPixels, const vector<PIXEL>& rightPixels)
{
    
    for(int i=0; i<ROWS; ++i)
    {
        int SIZE = rightPixels[i].position2D.x-leftPixels[i].position2D.x+1;
        
        vector<PIXEL> RowPos(SIZE);
        
        
        Interpolate (leftPixels[i], rightPixels[i], RowPos);
        
        for(int j=0; j<SIZE; ++j)
        {
            
            
            
            int x =RowPos[j].position2D.x;
            int y =RowPos[j].position2D.y;
            
            
            
            
            glm::vec3 color = LightingPixel(RowPos[j].position3D/RowPos[j].zinv,vertex);
            
            //vec3 color =  RowPos[j].illumination;
            
            if(x<=499 && x>=0 && y<=499 && y>=0)
            {
                if(RowPos[j].zinv > depthBuffer[y][x])
                {
                    depthBuffer[y][x] = RowPos[j].zinv;
                    PutPixelSDL(screen, x, y, color);
                }
            }
            
            else
                
                PutPixelSDL(screen, x, y, vec3(0,0,0));
        }
    }
}




void DrawPolygon(int indexTri)
{
    vector<glm::vec3> vertice(3);
    vertice[0]=triangles[indexTri].v0;
    vertice[1]=triangles[indexTri].v1;
    vertice[2]=triangles[indexTri].v2;
    
    
    
    vector<PIXEL> projectedVertice(3);
    
    
    
    vertex.Normal = triangles[indexTri].normal;
    vertex.reflectance = triangles[indexTri].color;
    
    for( int indexVer=0; indexVer<3; ++indexVer )
    {
        
        vertice[indexVer]= Rotation(vertice[indexVer]);
        
        projectedVertice[indexVer].position2D= VertexShader(vertice[indexVer]);
        projectedVertice[indexVer].zinv = 1/(vertice[indexVer].z-cameraPos.z);
        projectedVertice[indexVer].position3D = (vertice[indexVer]-cameraPos)*projectedVertice[indexVer].zinv;
        
        
        
        //projectedVertice[indexVer].illumination = Lighting(indexTri, indexVer,vertice);
        
    }
    
    int MAX=max(max(projectedVertice[0].position2D.y, projectedVertice[1].position2D.y), projectedVertice[2].position2D.y);
    int MIN=min(min(projectedVertice[0].position2D.y, projectedVertice[1].position2D.y),projectedVertice[2].position2D.y);
    
    int ROWS = MAX-MIN+1;
    
    
    
    vector<PIXEL> leftPixels(ROWS);
    vector<PIXEL> rightPixels(ROWS);
    
    // vec3 currentcolor=triangles[indexTri].color;
    
    
    for( int i=0; i<ROWS; ++i )
    {
        leftPixels[i].position2D=ivec2(+numeric_limits<int>::max(),MIN+i);
        rightPixels[i].position2D=ivec2(-numeric_limits<int>::max(),MIN+i);
    }
    
    
    
    ComputePolygonRows(leftPixels, rightPixels, projectedVertice, ROWS);
    
    
    
    
    DrawPolygonRows(ROWS, leftPixels, rightPixels);
    
}


//vec3 Lighting(int indexTri,int indexVer,vector<glm::vec3>& vertice)
//{
//
//    VERTEX vertex;
//   // vertex.position = vertice[indexVer];
//    vertex.Normal = triangles[indexTri].normal;
//    vertex.reflectance = triangles[indexTri].color;
//
//  //  vec3 Radium =glm::normalize(lightPos - vertex.position);
//  //  float a=glm::dot(vertex.Normal, Radium);
//    float b=0.0;
// //   float RR = glm::distance(lightPos, vertex.position);
//  //  float d= 1.0f/(4.0f*M_PI*RR*RR);
//  //  vec3 illumination =vertex.reflectance *(lightPower* d * max(a,b)+indirectLightPowerPerArea);
//
//
//
// //   return illumination;
//}




vec3 LightingPixel(vec3 Position,VERTEX vertex)
{
    vec3 locLight = lightPos-cameraPos;
    vec3 Radium =glm::normalize(locLight- Position);
    float a=glm::dot(vertex.Normal, Radium);
    float b=0.0;
    float RR = glm::distance(locLight, Position);
    float d= 1.0f/(4.0f*M_PI*RR*RR);
    vec3 illumination =vertex.reflectance *(lightPower*d * max(a,b)+indirectLightPowerPerArea);
    
    
    return illumination;
}



vec3 Rotation(vec3 vertex)
{
    
    R = glm::mat3(
                  1.0, 0, 0,
                  0, cos(xaw), -sin(xaw),
                  0, sin(xaw), cos(xaw)
                  );
    
    vertex=vertex*R;
    
    
    R= glm::mat3(
                 cos(yaw), 0, sin(yaw),
                 0, 1.0, 0,
                 -sin(yaw), 0, cos(yaw)
                 );
    
    vertex = vertex*R;
    
    
    
    return vertex;
}





