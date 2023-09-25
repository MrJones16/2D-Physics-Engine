#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include "verlet.hpp"
#include "grid.hpp"
#include <math.h>
#include <list>

using namespace std;

// void drawCircle(uint32_t* pixels, int width, int height, int cx, int cy, int radius, uint32_t color) {
//   int x = radius;
//   int y = 0;
//   int err = 0;

//   while (x >= y) {
//     int index = (cy + y) * width + cx + x;
//     if (cx + x < width && cy + y < height) pixels[index] = color;

//     index = (cy + x) * width + cx + y;
//     if (cx + y < width && cy + x < height) pixels[index] = color;

//     index = (cy + x) * width + cx - y;
//     if (cx - y >= 0 && cy + x < height) pixels[index] = color;

//     index = (cy + y) * width + cx - x;
//     if (cx - x >= 0 && cy + y < height) pixels[index] = color;

//     index = (cy - y) * width + cx - x;
//     if (cx - x >= 0 && cy - y >= 0) pixels[index] = color;

//     index = (cy - x) * width + cx - y;
//     if (cx - y >= 0 && cy - x >= 0) pixels[index] = color;

//     index = (cy - x) * width + cx + y;
//     if (cx + y < width && cy - x >= 0) pixels[index] = color;

//     index = (cy - y) * width + cx + x;
//     if (cx + x < width && cy - y >= 0) pixels[index] = color;

//     if (err <= 0) {
//       y++;
//       err += 2 * y + 1;
//     } else {
//       x--;
//       err -= 2 * x + 1;
//     }
//   }
// }

class World{
    private:

    public:
    int Width = 192;
    int Height = 108;
    Vec2 Gravity = Vec2(0,0.03f);

    vector<VerletObject> objects;
    float radius;

    VectorGrid vectorGrid;

    int threads = 10; // max 100
    int passes = 2;

    World(int Width, int Height){
        objects = vector<VerletObject>();
        this->Width = Width;
        this->Height = Height;
        vectorGrid = VectorGrid(Width,Height, 40);
        //Gravity = Vec2(0,1);
    };
    World(int Width, int Height, float constraintRadius, int cellSize){
        objects = vector<VerletObject>();
        this->Width = Width;
        this->Height = Height;
        vectorGrid = VectorGrid(Width,Height, cellSize);
        radius = constraintRadius;
        //Gravity = Vec2(0,1);
    };

    void Update(float dt){
        int sub_steps = 5;
        float sub_dt = dt / sub_steps;
        for (int i = 0; i < sub_steps; i++){
            
            ApplyCollisions();
            ApplyBorderConstraint();
            //ApplyRadiusConstraint();
        }

        ApplyGravity();
        
        UpdatePositions(sub_dt);

        
    };

    void UpdatePositions(float dt){
        for( auto &obj : objects){
            obj.UpdatePosition(dt);
        }
    };

    void ApplyGravity(){
        for( auto &obj : objects){
            //cout << "Applying gravity to Object" << endl;
            obj.Accelerate(Gravity);
        }
    };

    void ApplyRadiusConstraint(){
        Vec2 center = Vec2(Width / 2, Height / 2);
        //float radius = 50;
        for( auto &obj : objects){
            Vec2 direction = center - obj.position_current;
            float distance = direction.magnitude();
            if (distance > radius - obj.size){
                float difference = distance - (radius - obj.size);
                Vec2 change = direction.normalized() * difference;
                obj.position_current = obj.position_current + change;
            }
        }
    };

    void ApplyBorderConstraint(){
        for (auto &obj : objects){
            float dif;
            if (obj.position_current.x < (0 + obj.size)){
                dif = obj.position_current.x - (0 + obj.size);
                obj.position_current.x -= dif;
            }
            if (obj.position_current.x > (Width - obj.size)){
                dif = obj.position_current.x - (Width - obj.size);
                obj.position_current.x -= dif;
            }
            if (obj.position_current.y < (0 + obj.size)){
                dif = obj.position_current.y - (0 + obj.size);
                obj.position_current.y -= dif;
            }
            if (obj.position_current.y > (Height - obj.size)){
                dif = obj.position_current.y - (Height - obj.size);
                obj.position_current.y -= dif;
            }
        }
    };

    void ApplyBruteForceCollisions(){
        // for (auto &obj : objects){
        //     for (auto &other : objects){
        //         if (&obj == &other) continue;
        //         Vec2 obj_to_other = other.position_current - obj.position_current;
        //         if (obj_to_other.magnitude() < obj.size + other.size){
        //             float difference = obj_to_other.magnitude() - (obj.size + other.size);
        //             difference /= 2;
        //             other.position_current += obj_to_other.normalized() * -difference;
        //             obj.position_current += obj_to_other.normalized() * difference;
        //         }
        //     }
        // }
        for (auto &obj1 : objects){
            for (auto &obj2 : objects){
                if (&obj1 == &obj2) continue;
                Vec2 collision_axis = obj1.position_current - obj2.position_current;
                float dist = collision_axis.magnitude();
                float min_dist = obj1.size + obj2.size;
                if (dist < min_dist){
                    Vec2 n = collision_axis / dist;
                    float delta = min_dist - dist;
                    obj1.position_current += n * (0.5f * delta);
                    obj2.position_current -= n * (0.5f * delta);

                }

            }
        }
    };


    void ApplyCollisions(){
        //cout << "ApplyCollisions Called " << endl;
        //clear the grid
        vectorGrid.clear();
        //cout << "Cell Size: " << vectorGrid.cellSize << endl;

        //place every object into a cell
        for (int i = 0; i < objects.size(); i++){
            //cout << "I: " << i << endl;
            int x = objects[i].position_current.x / vectorGrid.cellSize;
            int y = objects[i].position_current.y / vectorGrid.cellSize;
            //cout << "Added index to a cell pre check" << endl;
            //cout << "X: " << x << endl;
            //cout << "Y: " << y << endl;
            if (x < 0 || x >= vectorGrid.width || y < 0 || y >= vectorGrid.height)continue;
            vectorGrid.get(x, y).Add(i);
            //cout << "Added index to a cell" << endl;
        }

        //multithread going through the cells
        vector<thread> ThreadVector;
        for (int p = 0; p < passes; p++){
            for (int t = 0; t < threads; t++){
                int tt = t;
                int tp = p;
                ThreadVector.emplace_back([this, tt, tp](){ThreadableCellIteration(tt,tp);});
                //this_thread::sleep_for(std::chrono::milliseconds(1));
                //std::thread tr (ThreadableCellIteration, this, tt, tp);
                //tr.join();
                //threadVector.emplace_back(tr);
            }
            for (auto &t: ThreadVector){
                if (t.joinable())
                t.join();
            }
        }
        
        //loop over each cell, skipping outside edges, and check collisions between surrounding cells
        // for (int x = 0; x < vectorGrid.width ; x++){
        //     for (int y = 0; y < vectorGrid.height ; y++){  //start at one and to end - 1
        //         //iterate over every cell thats not on the edge
        //         //cout << "Iterating on cell" << endl;
        //         CellSurroundingCollisions(x,y);
        //     }
        // }
    }

    void ThreadableCellIteration(int thread_num, int pass_num){
        float lowerBound = ((float)thread_num / (float)threads) * (float)vectorGrid.width;
        float upperBound = (((float)thread_num + 1) / (float)threads) * (float)vectorGrid.width;
        //cout << "((" << thread_num << " + 1) / " << threads << ") * " << (float)vectorGrid.width << endl;
        float delta = upperBound - lowerBound;
        float lowerx = lowerBound + (((float)pass_num/(float)passes)*delta);
        float upperx = lowerBound + ((((float)pass_num + 1) / (float)passes)*delta);

        //cout << "T: " << thread_num << ",P: " << pass_num << ",xr: " << lowerx << "," << upperx << ")" << endl;
        //cout << "   Threaded y bounds: (" << 0 << "," << vectorGrid.height << ")" << endl;
        for (int x = lowerx; x < upperx; x++){
            for (int y = 0; y < vectorGrid.height; y++){
                CellSurroundingCollisions(x, y);
            }
        }
    }

    void CellSurroundingCollisions(int x, int y){
        for (int dx = -1; dx <= 1; dx++){
            for (int dy = -1; dy <= 1; dy++){
                if (dx == 0 && dy == 0) continue;
                int x2 = x + dx;
                int y2 = y + dy;
                if (x2 < 0 || x2 >= vectorGrid.width) continue;
                if (y2 < 0 || y2 >= vectorGrid.height) continue;
                //cout << "dx:" << dx << " dy: " << dy << endl;
                //cout << "vector grid coord: (" << x << ", " << y << ")" << endl;
                Cell_Collisions(vectorGrid.get(x, y), vectorGrid.get(x2, y2));
                //cout << "Calling cell collisions on (" << x << ", " << y << "), (" << dx << ", " << dy << ")" << endl;
            }
        }
    }

    void Cell_Collisions(Cell one, Cell two){
        
        if (one.count == 0) return;
        //if (one.count > 0) cout << "one.count: " << one.count << endl;
        //if (two.count > 0) cout << "two.count: " << two.count << endl;
        for (int i = 0; i < one.count; i++){
            for (int j = 0; j < two.count; j++){
                ApplyCollision(objects[one.objects[i]], objects[two.objects[j]]);
                //cout << "Attempted collisions in seperate cells" << endl;
            }
        }
        for (int i = 0; i < one.count; i++){
            for (int j = 0; j < one.count; j++){
                if (i == j) continue;
                ApplyCollision(objects[one.objects[i]], objects[one.objects[j]]);
                //cout << "Applied a collision in own cell" << endl;
            }
        }
    }

    static void ApplyCollision(VerletObject &obj1, VerletObject &obj2){
        // << "Colliding Objects" << endl;
        if (&obj1 == &obj2) return;
        Vec2 collision_axis = obj1.position_current - obj2.position_current;
        float dist = collision_axis.magnitude();
        float min_dist = obj1.size + obj2.size;
        if (dist < min_dist){
            Vec2 n = collision_axis / dist;
            float delta = min_dist - dist;
            obj1.position_current += n * (0.5f * delta);
            obj2.position_current -= n * (0.5f * delta);
        };

    };


    void AddObject(VerletObject *obj){objects.emplace_back(*obj);}
    VerletObject* CreateObject(){
        VerletObject res = VerletObject();
        objects.emplace_back(res);
        return &(objects.back());
    };
    VerletObject* CreateObject(float xPos,float yPos){
        VerletObject res = VerletObject(xPos,yPos);
        objects.emplace_back(res);
        return &(objects.back());
    };
    VerletObject* CreateObject(float xPos,float yPos, float size){
        VerletObject res = VerletObject(xPos,yPos, size);
        objects.emplace_back(res);
        return &(objects.back());
    };
    void DeleteObject(int index){
        if (index < 0 || index >= objects.size()) return;
        std::swap(objects[index], objects.back());
        objects.pop_back();
    };
    void DeleteAllObjects(){
        while (objects.size() > 0) objects.pop_back();
    };

    void DrawToPixels(uint32_t * pixels, int Width, int Height){

        //fill with gray pixels
        // for (int i = 0; i < Width; i++){
        //     for (int j = 0; j < Height; j++){
        //         pixels[j * Width + i] = 0x88888888;
        //     }
        // }

        //fill radius with black

        //Vec2 center = Vec2(Width / 2, Height / 2);
        // //float radius = 50;
        // for (int x = center.x - radius - 10; x < center.x + radius + 10; x++){
        //    for (int y = center.y - radius - 10; y < center.y + radius + 10; y++){
        //        if (!((x < 0) || (x >= Width) || (y < 0 ) || (y >= Height))){
        //            if (sqrt(((x - center.x)*(x - center.x)) + ((y - center.y)*(y - center.y))) < radius)
        //            pixels[y * Width + x] = 0x00000000;
        //        }
        //    }
        // }

        //DEBUGGING
        // for (int x = 0; x < Width; x++){
        //     for (int y = 0; y < Height; y++){
        //         int x2 = x / vectorGrid.cellSize;
        //         int y2 = y / vectorGrid.cellSize;
        //         if (x2 % 2 == 0) {pixels[y * Width + x] += 0xFF222222;}else{pixels[y * Width + x] += 0xFF000000;}
        //         if (y2 % 2 == 0) {pixels[y * Width + x] += 0xFF000000;}else{pixels[y * Width + x] += 0xFF222222;}
        //     }
        // }

        //draw pixel for objects

        // for( auto obj : objects){
        //     Vec2 position = obj.position_current;
        //     if (!(((int)position.x < 0) || ((int)position.x >= Width) || ((int)position.y < 0 ) || ((int)position.y >= Height)))
        //     pixels[(int)position.y * Width + (int)position.x] = 0xFFFFFFFF;
        // }


        //draw circle for objects
        // clock_t C = clock();
        // for (auto &obj : objects){
        //     for (int x = obj.position_current.x - obj.size; x < obj.position_current.x + obj.size; x++){
        //         for (int y = obj.position_current.y - obj.size; y < obj.position_current.y + obj.size; y++){
        //             if (!((x < 0) || (x >= Width) || (y < 0 ) || (y >= Height))){
        //                 if (sqrt(((x - obj.position_current.x)*(x - obj.position_current.x)) + ((y - obj.position_current.y)*(y - obj.position_current.y))) < obj.size)
        //                 pixels[y * Width + x] = obj.color;
        //                 //char hex_string[20];
        //                 //sprintf(hex_string, "%X", obj.color);
        //                 //cout << "Color is: 0x" << hex_string << endl;
        //             }
        //         }
        //     }
        // }
        // cout << "Circles took " << (float)clock() - (float)C << " ticks" << endl;

        //clock_t C = clock();
        for (auto &obj : objects) {
            int r = obj.size;
            int r2 = r * r;
            
            int x_min = std::max(0, (int)(obj.position_current.x - r));
            int x_max = std::min(Width - 1, (int)(obj.position_current.x + r));
            int y_min = std::max(0, (int)(obj.position_current.y - r));
            int y_max = std::min(Height - 1, (int)(obj.position_current.y + r));

            for (int y = y_min; y <= y_max; ++y) {
                int dy = y - obj.position_current.y;
                int dy2 = dy * dy;
                for (int x = x_min; x <= x_max; ++x) {
                    int dx = x - obj.position_current.x;
                    int dx2 = dx * dx;
                    if (dx2 + dy2 < r2) {
                        pixels[y * Width + x] = obj.color;
                    }
                }
            }
        }
        //cout << "Circles took " << (float)clock() - (float)C << " ticks" << endl;

        // clock_t C = clock();
        // for (auto &obj: objects){
        //     int cx = obj.position_current.x;
        //     int cy = obj.position_current.y;
        //     int x = obj.size;
        //     int y = 0;
        //     int err = 0;

        //     while (x >= y) {
        //         for (int i = cx - x; i <= cx + x; i++) {
        //             for (int j = cy - y; j <= cy + y; j++) {
        //                 if (i >= 0 && i < Width && j >= 0 && j < Height) {
        //                     int index = j * Width + i;
        //                     *(pixels + index) = obj.color;
        //                 }
        //             }
        //         }

        //         for (int i = cx - y; i <= cx + y; i++) {
        //             for (int j = cy - x; j <= cy + x; j++) {
        //                 if (i >= 0 && i < Width && j >= 0 && j < Height) {
        //                     int index = j * Width + i;
        //                     *(pixels + index) = obj.color;
        //                 }
        //             }
        //         }

        //         if (err <= 0) {
        //             y++;
        //             err += 2 * y + 1;
        //         } else {
        //             x--;
        //             err -= 2 * x + 1;
        //         }
        //     }
        // }
        // cout << "Circles took " << (float)clock() - (float)C << " ticks" << endl;
    };
};
