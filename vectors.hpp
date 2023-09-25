#pragma once
#include <iostream>

struct IVec2
{
    int x;
    int y;
    IVec2(){
        x = 0;
        y = 0;
    }
    IVec2(int xpos, int ypos){
        x = xpos;
        y = ypos;
    }
    IVec2 operator + (IVec2 const &obj);
    IVec2 operator - (IVec2 const &obj);
    void operator += (IVec2 const &obj);
    void operator -= (IVec2 const &obj);
    float magnitude ();
    
};

struct Vec2
{
    float x;
    float y;
    Vec2(){
        x = 0;
        y = 0;
    }
    Vec2(float xpos, float ypos){
        x = xpos;
        y = ypos;
    }
    Vec2 operator + (Vec2 const &obj);
    Vec2 operator - (Vec2 const &obj);
    Vec2 operator * (const float val);
    Vec2 operator / (const float val);
    void operator += (Vec2 const &obj);
    void operator -= (Vec2 const &obj);
    void operator /= (const float val);
    float magnitude ();
    Vec2 normalized ();
    
};