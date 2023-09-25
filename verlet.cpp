#include "verlet.hpp"
#include <iostream>

VerletObject::VerletObject(){
    this->position_current = Vec2(0,0);
    this->position_previous = Vec2(0,0);
    this->acceleration = Vec2(0,0);
    this->size = 10.0f;
    this->color = 0xFFFFFFFF;
};
VerletObject::VerletObject(float xpos, float ypos){
    this->position_current = Vec2(xpos,ypos);
    this->position_previous = Vec2(xpos,ypos);
    this->acceleration = Vec2(0,0);
    this->size = 10.0f;
    this->color = 0xFFFFFFFF;
};
VerletObject::VerletObject(float xpos, float ypos, float size){
    this->position_current = Vec2(xpos,ypos);
    this->position_previous = Vec2(xpos,ypos);
    this->acceleration = Vec2(0,0);
    this->size = size;
    this->color = 0xFFFFFFFF;
};

void VerletObject::UpdatePosition(float dt){
    //std::cout << "Updating Position" << std::endl;
    //calculate the velocity
    Vec2 velocity = position_current - position_previous;
    //std::cout << "Previous: " << position_previous.x << " , " << position_previous.y << std::endl;
    //std::cout << "Current: " << position_current.x << " , " << position_current.y << std::endl;
    //std::cout << "Velocitiy: " << velocity.x << " , " << velocity.y << std::endl;
    //std::cout << "Acceleration: " << acceleration.x << " , " << acceleration.y << std::endl;
    //set the previous
    position_previous = position_current;
    //update the current position
    position_current = position_current + velocity + acceleration;// * (dt * dt);
    //std::cout << "New Position: " << position_current.x << " , " << position_current.y <<  std::endl;
    //reset the acceleration
    acceleration = Vec2(0,0);
};
void VerletObject::Accelerate(Vec2 acc){
    //std::cout << "acc: (" << acc.x << ", " <<acc.y<<")"<<std::endl;
    acceleration = acceleration + acc;
    //std::cout << "acceleration: (" << acceleration.x << ", " <<acceleration.y<<")"<<std::endl;
};
void VerletObject::Accelerate(float x, float y){
    acceleration.x += x;
    acceleration.y += y;
    //std::cout << "new acceleration: (" << acceleration.x << ", " <<acceleration.y<<")"<<std::endl;
};