#include <cstdio>
#include "raylib.h"

int main() {

    const int fps{60};
    const int w_width{512};
    const int w_height{380};

    InitWindow(w_width, w_height, "Dasher");
    SetTargetFPS(fps);
    
    int velocity{0};
    const int gravity{1200}; // pixels per second
    const int jump_velocity{-650}; // pixels per second
    bool airborne = false;


    // scarfy stuff
    const int scarfy_frame_nbr{6};
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    Rectangle scarfy_rec;
    scarfy_rec.width = scarfy.width/6;
    scarfy_rec.height = scarfy.height;
    scarfy_rec.x = 0;
    scarfy_rec.y = 0;

    Vector2 scarfy_pos;
    // exact center
    scarfy_pos.x = w_width/2 - scarfy_rec.width/2;
    scarfy_pos.y = w_height - scarfy_rec.height;
    int scarfy_frame{};

    // nebula stuff
    const int nebula_animation_frames{8};
    const int nebula_velocity{-400}; // pixels per second
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png"); // 8x8 spritesheet
    Rectangle nebula_rec{0.0, 0.0, nebula.width/8, nebula.height/8};
    Vector2 nebula_pos{w_width, w_height - nebula_rec.height};
    int nebula_frame{};
    const float nebula_update_time{1.0 /(fps/(nebula_animation_frames - 1))};
    float nebula_running_time{};

    // amount of time before it should update the animation frame
    const float scarfy_update_time{1.0 /(fps/(scarfy_frame_nbr - 1))}; // fps/amount of frames in the animation
    float scarfy_running_time{};

    while (!WindowShouldClose())
    {
        const float delta_time{GetFrameTime()}; // time since last frame
        BeginDrawing();
        ClearBackground(WHITE);
        
        // logics

        // ground check
        if(scarfy_pos.y >= w_height - scarfy_rec.height)  {
            velocity = 0;
            airborne = false;
        // rec in air
        } else {
            // gravity pulls down (positive)
            velocity += (gravity * delta_time);
            airborne = true;
        }

        // jumpers
        if(IsKeyDown(KEY_SPACE) && !airborne) {
            velocity += jump_velocity;
        }
        // move nebuls
        nebula_pos.x += (nebula_velocity * delta_time);
        if(nebula_pos.x < 0 - nebula_rec.width) {
            nebula_pos.x = w_width;
        }

        // update nebula animation
        nebula_running_time += delta_time;
        if(nebula_running_time >= nebula_update_time) {
            nebula_rec.x = nebula_frame * (nebula.width/nebula_animation_frames);
            nebula_frame++;
            if(nebula_frame == nebula_animation_frames) {
                nebula_frame = 0;
            }
            nebula_running_time = 0;
        }


        scarfy_pos.y += (velocity * delta_time);

        // animate scarfy
        // no need to animate when in air
        if(!airborne) {
            scarfy_running_time += delta_time;
            if(scarfy_running_time >= scarfy_update_time) {
                scarfy_rec.x = scarfy_frame * (scarfy.width/scarfy_frame_nbr);
                scarfy_frame++;
                // reset animation
                if(scarfy_frame == scarfy_frame_nbr) {
                    scarfy_frame = 0;
                }
                scarfy_running_time = 0;
            }
        }

        DrawTextureRec(nebula, nebula_rec, nebula_pos, WHITE);
        DrawTextureRec(scarfy, scarfy_rec, scarfy_pos, WHITE);

        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    CloseWindow();
} 