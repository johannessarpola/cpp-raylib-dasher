#include <cstdio>
#include "raylib.h"

struct Box2D
{
    int width;
    int height;
};

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame_set_size;
    int frame;
    float running_time;
    float update_time;
    Box2D sprite_sheet;

    void init_anim(int fps, int x_sprites_size, int y_sprites_size, Texture2D tex)
    {
        this->frame_set_size = x_sprites_size;
        this->frame = 0;
        this->running_time = 0.0;
        this->update_time = 1.0 / (fps / (x_sprites_size - 1));
        this->rec.width = tex.width / x_sprites_size;
        this->rec.height = tex.height / y_sprites_size;
        this->sprite_sheet.width = tex.width;
        this->sprite_sheet.height = tex.height;
        this->rec.x = 0.0;
        this->rec.y = 0.0;
    }

    void should_update_animation(float delta)
    {
        this->running_time += delta;
        if (this->running_time >= this->update_time)
        {
            this->next_frame();
            this->running_time = 0;
        }
    }

    void next_frame()
    {
        this->rec.x = this->frame * (this->sprite_sheet.width / this->frame_set_size);
        frame++;
        if (frame == frame_set_size)
        {
            frame = 0;
        }
    }
    /*
     * Sets the position in regards to the box size
     */
    void set_bounded_pos_x(int x)
    {
        this->pos.x = x - this->rec.width / 2;
    }

    /*
     * Sets the position in regards to the box size
     */
    void set_bounded_pos_y(int y)
    {
        this->pos.y = y - this->rec.height;
    }

    void set_pos_x(int x)
    {
        this->pos.x = x;
    }

    void set_pos_y(int y)
    {
        this->pos.y = y;
    }
};

int main()
{

    int window_dims[2];
    window_dims[0] = 512;
    window_dims[1] = 380;

    const int fps{60};

    InitWindow(window_dims[0], window_dims[1], "Dasher");
    SetTargetFPS(fps);

    int velocity{0};
    const int gravity{1200};       // pixels per second
    const int jump_velocity{-650}; // pixels per second
    bool airborne = false;

    // scarfy stuff
    Texture2D scarfy_sheet = LoadTexture("textures/scarfy.png");
    AnimData scarfy_data{};
    scarfy_data.init_anim(fps, 6, 1, scarfy_sheet);
    // exact center
    scarfy_data.set_bounded_pos_x(window_dims[0] / 2);
    scarfy_data.set_bounded_pos_y(window_dims[1]);

    // nebula stuff
    Texture2D nebula_sheet = LoadTexture("textures/12_nebula_spritesheet.png"); // 8x8 spritesheet
    AnimData nebula_data{};
    nebula_data.init_anim(fps, 8, 8, nebula_sheet);
    nebula_data.set_pos_x(window_dims[0]);
    nebula_data.set_bounded_pos_y(window_dims[1]);

    const int nebula_velocity{-400}; // pixels per second

    while (!WindowShouldClose())
    {
        const float delta_time{GetFrameTime()}; // time since last frame
        BeginDrawing();
        ClearBackground(WHITE);

        // logics

        // ground check
        if (scarfy_data.pos.y >= window_dims[1] - scarfy_data.rec.height)
        {
            velocity = 0;
            airborne = false;
            // rec in air
        }
        else
        {
            // gravity pulls down (positive)
            velocity += (gravity * delta_time);
            airborne = true;
        }

        // jumpers
        if (IsKeyDown(KEY_SPACE) && !airborne)
        {
            velocity += jump_velocity;
        }
        // move nebuls
        nebula_data.pos.x += (nebula_velocity * delta_time);
        if (nebula_data.pos.x < 0 - nebula_data.rec.width)
        {
            nebula_data.set_pos_x(window_dims[0]);
        }

        // update nebula animation
        nebula_data.should_update_animation(delta_time);

        scarfy_data.pos.y += (velocity * delta_time);

        // animate scarfy
        // no need to animate when in air
        if (!airborne)
        {
            scarfy_data.should_update_animation(delta_time);
        }

        DrawTextureRec(nebula_sheet, nebula_data.rec, nebula_data.pos, WHITE);
        DrawTextureRec(scarfy_sheet, scarfy_data.rec, scarfy_data.pos, WHITE);

        EndDrawing();
    }
    UnloadTexture(scarfy_sheet);
    UnloadTexture(nebula_sheet);
    CloseWindow();
}