#pragma once
#include <vector>
#include <iostream>
#include "verlet.hpp"

using namespace std;

struct Cell{
    int objects[10];
    int count = 0;
    Cell(){
        count = 0;
        //cout << "Cell Created" << endl;
        for (int i = 0; i < 10; i++){
            objects[i] = -1;
        }
    }
    void Add(int x){
        //cout << "Count: " << count << endl;
        if (count < 10) {
            objects[count] = x;
            //cout << "incrementing count" << endl;
            count++;
        }
    }
    void reset(){
        count = 0;
        for (int i = 0; i < 10; i++){
            objects[i] = -1;
        }
    }
};

class VectorGrid{
    public:
    int width;
    int height;
    int cellSize;

    vector<Cell> grid;
    VectorGrid(){
        width = 0;
        height = 0;
        cellSize = 0;
        grid = vector<Cell>();
    }
    VectorGrid(int width, int height, int cellSize){
        this->width = ((float)width / (float)cellSize) + 0.5f;
        this->height = ((float)height / (float)cellSize) + 0.5f;
        this->cellSize = cellSize;
        grid = vector<Cell>(this->width * this->height);
    }

    Cell& get(int x, int y){
        return grid[y * width + x];
    }

    void clear(){
        for (int x = 0; x < width; x++){
            for (int y = 0; y < height; y++){
                get(x, y).reset();
            }
        }
    }

    void FindCollisions(){
        
    }

};

