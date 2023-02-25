#include <cstdio>
#include <stdlib.h>
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

    void init(int fps, int x_sprites_size, int y_sprites_size, Texture2D tex)
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
        this->rec.x = this->frame * this->rec.width;
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

    void add_pos_x(int x)
    {
        this->pos.x += x;
    }

    void set_pos_y(int y)
    {
        this->pos.y = y;
    }
};

struct ScrollingBackgroundData
{
    Vector2 backgrounds[2]; // I guess this could be configured
    int tex_width;
    int repeat_nbr{2}; // I guess this could be configured
    float pos;
    float scale;
    float speed;

    void init(Texture2D tex, float speed, float scale)
    {
        this->tex_width = tex.width;
        this->scale = scale;
        this->speed = speed;
    }

    void should_reset_pos()
    {
        if (this->pos <= -tex_width * repeat_nbr)
        {
            this->pos = 0.0;
        }
    }
    void update_background_pos(float delta_time)
    {
        this->pos -= this->speed * delta_time;
        this->backgrounds[0].x = this->pos;
        this->backgrounds[1].x = this->pos + tex_width * repeat_nbr;
    }

    void draw(Texture2D tex)
    {
        DrawTextureEx(tex, backgrounds[0], 0.0, this->scale, WHITE);
        DrawTextureEx(tex, backgrounds[1], 0.0, this->scale, WHITE);
    }

    void tick(Texture2D tex, float delta_time)
    {
        this->update_background_pos(delta_time);
        this->should_reset_pos();
        this->draw(tex);
    }
};

bool is_on_ground(AnimData data, int window_height)
{
    return data.pos.y >= window_height - data.rec.height;
}

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

    // backgrounds
    Texture2D bg = LoadTexture("textures/far-buildings.png");
    Texture2D mg = LoadTexture("textures/back-buildings.png");
    Texture2D fg = LoadTexture("textures/foreground.png");

    // scarfy stuff
    Texture2D scarfy_sheet = LoadTexture("textures/scarfy.png");
    AnimData scarfy_data{};
    scarfy_data.init(fps, 6, 1, scarfy_sheet);
    // exact center
    scarfy_data.set_bounded_pos_x(window_dims[0] / 2);
    scarfy_data.set_bounded_pos_y(window_dims[1]);

    // nebula stuff
    Texture2D nebula_sheet = LoadTexture("textures/12_nebula_spritesheet.png"); // 8x8 spritesheet

    const int size_of_nebulae{3};
    AnimData nebulae[size_of_nebulae]{};

    for (int i = 0; i < size_of_nebulae; i++)
    {
        nebulae[i].init(fps, 8, 8, nebula_sheet);
        nebulae[i].set_bounded_pos_y(window_dims[1]);
        nebulae[i].set_pos_x(window_dims[0] + 500 * (i + 1));
    }

    ScrollingBackgroundData bgd{};
    bgd.init(bg, 20, 2.0);

    ScrollingBackgroundData mgd{};
    mgd.init(mg, 40, 2.0);

    ScrollingBackgroundData fgd{};
    fgd.init(fg, 80, 2.0);

    const int nebula_velocity{400}; // pixels per second

    float finish_line{7000.0};
    bool is_finished;
    is_finished = false;

    bool collision = false;

    while (!WindowShouldClose())
    {
        const float delta_time{GetFrameTime()}; // time since last frame
        BeginDrawing();
        ClearBackground(WHITE);
        if(!collision && !is_finished) {
            // draw background
            bgd.tick(bg, delta_time);
            mgd.tick(mg, delta_time);
            fgd.tick(fg, delta_time);

            // move goalposts
            finish_line -= (nebula_velocity * delta_time);
            // ground check
            if (is_on_ground(scarfy_data, window_dims[1]))
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

            scarfy_data.pos.y += (velocity * delta_time);
            if (!airborne)
            {
                scarfy_data.should_update_animation(delta_time);
            }


            for (int i = 0; i < size_of_nebulae; i++)
            {
                nebulae[i].pos.x -= (nebula_velocity * delta_time);
                nebulae[i].should_update_animation(delta_time);
                if(nebulae[1].pos.x + nebulae[i].rec.width <= 0) {
                    nebulae[i].pos.x = window_dims[0] + nebulae[i].rec.width + ((rand() % 5 + 1) * 75);
                }
            };

            for (AnimData nebula : nebulae)
            {
                // texture pad in sprite sheet
                float pad{50};
                Rectangle neb_rec
                {
                    nebula.pos.x + pad,
                    nebula.pos.y + pad,
                    nebula.rec.width - 2 * pad,
                    nebula.rec.height - 2 * pad
                };
                Rectangle scarfy_rec {
                    scarfy_data.pos.x,
                    scarfy_data.pos.y,
                    scarfy_data.rec.width - 2,
                    scarfy_data.rec.height - 2
                };
                collision = CheckCollisionRecs(neb_rec, scarfy_rec);
                if(collision) {
                    break;
                }
            };

            for (AnimData nebula : nebulae) {
                DrawTextureRec(nebula_sheet, nebula.rec, nebula.pos, WHITE);
            };

            // animate scarfy
            // no need to animate when in air
            DrawTextureRec(scarfy_sheet, scarfy_data.rec, scarfy_data.pos, WHITE);
        }

        if (is_finished)
        {
            DrawText("You win!", window_dims[0] / 2, window_dims[1] / 2, 20, BLUE);
        } else {
            char buf[32];
            sprintf(buf, "Distance left: %.0f", finish_line);
            DrawText(buf, 0, 0, 14, WHITE);
        }

        if (collision)
        {
            DrawText("You lose!", window_dims[0] / 2, window_dims[1] / 2, 20, RED);
        }

        if (scarfy_data.pos.x >= finish_line)
        {
            is_finished = true;
        }

        EndDrawing();
    }
    UnloadTexture(scarfy_sheet);
    UnloadTexture(nebula_sheet);
    UnloadTexture(bg);
    UnloadTexture(mg);
    UnloadTexture(fg);
    CloseWindow();
}