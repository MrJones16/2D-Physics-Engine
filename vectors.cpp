#include "vectors.hpp"
#include <math.h>

IVec2 IVec2::operator+(IVec2 const &obj){
    IVec2 res;
    res.x = x + obj.x;
    res.y = y + obj.y;
    return res;
}

IVec2 IVec2::operator-(IVec2 const &obj){
    IVec2 res;
    res.x = x - obj.x;
    res.y = y - obj.y;
    return res;
}
void IVec2::operator+=(IVec2 const &obj){
    x = x + obj.x;
    y = y + obj.y;
}
void IVec2::operator-=(IVec2 const &obj){
    x = x - obj.x;
    y = y - obj.y;
}


//floating points:

Vec2 Vec2::operator+(Vec2 const &obj){
    Vec2 res;
    res.x = x + obj.x;
    res.y = y + obj.y;
    return res;
}

Vec2 Vec2::operator-(Vec2 const &obj){
    Vec2 res;
    res.x = x - obj.x;
    res.y = y - obj.y;
    return res;
}

Vec2 Vec2::operator*(const float val){
    Vec2 res;
    res.x = x * val;
    res.y = y * val;
    return res;
}
Vec2 Vec2::operator/(const float val){
    Vec2 res;
    res.x = x / val;
    res.y = y / val;
    return res;
}

void Vec2::operator+=(Vec2 const &obj){
    x = x + obj.x;
    y = y + obj.y;
}

void Vec2::operator-=(Vec2 const &obj){
    x = x - obj.x;
    y = y - obj.y;
}

void Vec2::operator/=(const float val){
    x = x / val;
    y = y / val;
}

float Vec2::magnitude(){
    return sqrt(x * x + y * y);
}

Vec2 Vec2::normalized(){
    Vec2 res;
    float mag = magnitude();
    res.x = x / mag;
    res.y = y / mag;
    return res;
}
