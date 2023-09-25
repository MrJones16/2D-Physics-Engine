#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <SDL2/SDL.h>
#include <math.h>
#include <ctime>
#include <chrono>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "verlet.hpp"
#include "world.hpp"



using namespace std;

const int Width = 192*5, Height = 108*5;
const int FPS = 1000;
const float Radius = (Height/2)-1; // bounding radius:
const int objRadius = 5;
//char* pngString = "Venus.png";

int main(int argc, char *argv[])
{
    
    
    SDL_Init(SDL_INIT_EVERYTHING);
    float scaleX = 2;
    float scaleY = 2;
    SDL_Window *window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width * scaleX, Height * scaleY, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    
    

    SDL_RenderSetLogicalSize(renderer, Width*scaleX, Height*scaleY);
    //int SDL_RenderSetScale(SDL_Renderer * renderer, float scaleX, float scaleY);

    //setting background color to black
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Event event;

    SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, Width, Height);

    uint32_t * pixels = new Uint32[Width * Height];
    memset(pixels, 0, Width * Height * sizeof(Uint32));

    //set the vector of all game objects
    //vector<VerletObject*> *objects = new vector<VerletObject*>();
    World world = World(Width,Height, Radius, objRadius * 2);

    //VerletObject *obj = world.CreateObject(Width / 2 +1 ,Height / 2, 5);
    //VerletObject *obj = &(world.objects.back());
    //obj->color = 0x123456FF;

    int spamcount = 0;
    bool spamON = false;
    int PictureCounter = 0;
    bool PictureRespawn = false;
    vector<VerletObject> objects = vector<VerletObject>();
    

    bool showDebug = false;

    //input variables
    int mouseX = 0, mouseY = 0;
    bool LMB = false;
    bool RMB = false;
    bool SMB = false;
    bool BMB = false;
    bool FMB = false;
    bool ScrollingUp = false;
    bool Scrollingdown = false;

    clock_t dtclock;
    float dt = 0.0f;
    clock_t t;
    bool update = true;
    bool quit = false;
    while (!quit){
        dtclock = clock();
        t = clock();
        while (SDL_PollEvent(&event)){
            switch (event.type)
            {
            case SDL_QUIT://quitting the program
                quit = true;
                break;
            case SDL_KEYDOWN://on key downs
                if (SDLK_SPACE == event.key.keysym.sym){
                    if (update) {
                        update = false;
                    }else{
                        update = true;
                    }
                }
                if (SDLK_ESCAPE == event.key.keysym.sym){
                    quit = true;
                }
                if (SDLK_RIGHT == event.key.keysym.sym){
                    world.Update(dt);
                    //clear screen
                    memset(pixels, 0, Width * Height * sizeof(Uint32));
                    //draw objects to screen
                    world.DrawToPixels(pixels, Width, Height);
                }
                if (SDLK_f == event.key.keysym.sym){
                    //thanks chatgpt:
                    int imgWidth, imgHeight, channels;
                    unsigned char* image_data = stbi_load("Nebula.png", &imgWidth, &imgHeight, &channels, 0);
                    if (image_data == nullptr) {
                        std::cerr << "Error loading image." << std::endl;
                        return 1;
                    }
                    // Process image here...
                    for (auto &obj : world.objects){
                        //position x in 0-1:
                        //float xPos = obj.position_current.x - (Width/2) + Radius;
                        //xPos /= (Radius * 2);
                        //float yPos = obj.position_current.y - (Height/2) + Radius;
                        //yPos /= (Radius*2);
                        float xPos = obj.position_current.x / Width;
                        float yPos = obj.position_current.y / Height;
                        xPos *= imgWidth;
                        yPos *= imgHeight;
                        xPos = SDL_clamp(xPos, 0, imgWidth);
                        yPos = SDL_clamp(yPos, 0, imgHeight);
                        //cout << "object's x and y: " << xPos << " : " << yPos << endl;
                        uint32_t R = image_data[((int)yPos * imgWidth + (int)xPos)*4];
                        uint32_t G = image_data[((int)yPos * imgWidth + (int)xPos)*4+1];
                        uint32_t B = image_data[((int)yPos * imgWidth + (int)xPos)*4+2];
                        uint32_t A = image_data[((int)yPos * imgWidth + (int)xPos)*4+3];
                        obj.color = (A) << 24 | (R) << 16 | (G) << 8 | (B);
                    }
                    stbi_image_free(image_data);
                    //I now have a vector of all the objects with their colors, i just need to copy the vector,
                    // and re-spawn them using that vector
                    objects = world.objects;
                    world.DeleteAllObjects();

                    PictureRespawn = true;

                }
                if (SDLK_v == event.key.keysym.sym){
                    if (spamON){
                        spamON = false;
                    }
                    else{
                        spamON = true;
                    }
                }
                
                break;
            case SDL_KEYUP:// key ups
                if (SDLK_a == event.key.keysym.sym){
                    
                }
                break;
            case SDL_MOUSEMOTION:
                SDL_GetMouseState(&mouseX, &mouseY);
                mouseX/=scaleX;
                mouseY/=scaleY;
                if (showDebug) cout << mouseX << " : " << mouseY << endl;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (SDL_BUTTON_LEFT == event.button.button){
                    LMB = true;
                    if (showDebug) cout << "Left Mouse Button Down" << endl;

                    VerletObject *vobj = world.CreateObject(Width / 2,Height / 2, objRadius);
                    vobj->Accelerate((rand() % 3 + 0), (rand() % 3 + 0));
                    vobj->color = 0xFF000000 | (rand() % 254 + 0) << 16 | (rand() % 254 + 0) << 8 | (rand() % 254 + 0);

                    //cout << "Color: " << vobj.color << endl;
                    //char hex_string[20];
                    //sprintf(hex_string, "%X", vobj->color);
                    //cout << hex_string << endl;


                }
                else if (SDL_BUTTON_RIGHT == event.button.button){
                    RMB = true;
                    if (showDebug) cout << "Right Mouse Button Down" << endl;

                    world.DeleteObject(0);

                }
                else if (SDL_BUTTON_MIDDLE == event.button.button){
                    SMB = true;
                    if (showDebug) cout << "Middle Mouse Button Down" << endl;
                }
                else if (SDL_BUTTON_X1 == event.button.button){
                    BMB = true;
                    if (showDebug) cout << "Back Mouse Button Down" << endl;
                }
                else if (SDL_BUTTON_X2 == event.button.button){
                    FMB = true;
                    if (showDebug) cout << "Forward Mouse Button Down" << endl;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (SDL_BUTTON_LEFT == event.button.button){
                    LMB = false;
                    if (showDebug) cout << "Left Mouse Button Up" << endl;
                }
                else if (SDL_BUTTON_RIGHT == event.button.button){
                    RMB = false;
                    if (showDebug) cout << "Right Mouse Button Up" << endl;
                }
                else if (SDL_BUTTON_MIDDLE == event.button.button){
                    SMB = false;
                    if (showDebug) cout << "Middle Mouse Button Up" << endl;
                }
                else if (SDL_BUTTON_X1 == event.button.button){
                    BMB = false;
                    if (showDebug) cout << "Back Mouse Button Up" << endl;
                }
                else if (SDL_BUTTON_X2 == event.button.button){
                    FMB = false;
                    if (showDebug) cout << "Forward Mouse Button Up" << endl;
                }
                break;
            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0){
                    ScrollingUp = true;
                    //cout << "Mouse Wheel Scrolling Up" << endl;
                }else{ScrollingUp = false;}
                if (event.wheel.y < 0){
                    Scrollingdown = true;
                    //cout << "Mouse Wheel Scrolling Down" << endl;
                }else {Scrollingdown = false;}
                break;
            default:
                break;
            }
            //Make the mouse positions locked to the window size - 1 (since array is 0 - 999):
            
        }

        //binding mouse to array bounds
        if (mouseX > Width-1) mouseX = Width-1;
        if (mouseX < 0) mouseX = 0;
        if (mouseY > Height-1) mouseY = Height-1;
        if (mouseY < 0) mouseY = 0;
        
        //physics
        if (update){
            world.Update(dt);
        }
        //clear screen
        memset(pixels, 0, Width * Height * sizeof(Uint32));
        //draw objects to screen
        world.DrawToPixels(pixels, Width, Height);
        

        int spawnPerFrame = 2;

        if (spamON && !PictureRespawn)
        if (spamcount < 20000){
            for (int i = 0; i < spawnPerFrame; i++){
                VerletObject *objspam = world.CreateObject((Width / 2) + (i * objRadius * 2),(Height / 2) - (Height / 3), objRadius);
                objspam->Accelerate(1, -2);
                objspam->color = 0x88000000 + 0x00111111 * spamcount;
                if (objspam->color < 0x77000000 || (objspam->color & 0x00FFFFFF) < 100) objspam->color = 0xFFFFFFFF;
                spamcount++;
            }
        }
        if (PictureRespawn){
            if (PictureCounter < objects.size()){
                for (int i = 0; i < spawnPerFrame; i++){
                    VerletObject *objspam = world.CreateObject((Width / 2) + (i * objRadius * 2),(Height / 2) - (Height / 3), objRadius);
                    objspam->Accelerate(1, -2);
                    objspam->color = objects[PictureCounter].color;
                    PictureCounter++;
                }
            }
        }
        
        
        
        //drawing with left and right click
        if (LMB){
            // VerletObject *objspam = world.CreateObject(Width / 2,(Height / 2) - (Height / 4), 3);
            // objspam->Accelerate((rand() % 4)-2, (rand() % 4)-2);
            // objspam->color = 0xFFFFFFFF;

            // world.CreateObject(140,100, 5);
            //pixels[mouseY * Width + mouseX] = 0xFF1188FF;
        }
        if (RMB){
            //pixels[mouseY * Width + mouseX] = 0;
        }

        //rendering to the screen
        SDL_UpdateTexture(texture, NULL, pixels, Width * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);


        //slowing program to set FPS
        clock_t newT;
        newT = clock() - t;
        float milli = 1 / (float)FPS;
        milli *= 1000.0f;
        int f = milli - newT;
        if (f > 0) {
            this_thread::sleep_for(std::chrono::milliseconds(f));
        }
            // else{
            // this_thread::sleep_for(std::chrono::milliseconds(1));
            // }
        t = clock() - t;
        //cout << "waited for " << f << "milliseconds" << endl;
        //cout << "Program took " << t << " ticks, " << ( (float)t / CLOCKS_PER_SEC) << " seconds" << endl;

        dt = (float)(clock() - dtclock)/CLOCKS_PER_SEC;
        if (showDebug) cout << "dt: " << dt << endl;
        if (showDebug) cout << "FPS: " << 1.0f / dt << endl;
        //cout << "FPS: " << 1.0f / dt << endl;

        

        }


    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
