//
// Created by 31910 on 2025/9/9.
//

#include "sticker.h"
#include "Sprite.h"

#include <OLED.h>
#include <stdlib.h>
#include <Ticker.h>

#include "config.h"

void Sprite_Update(UIComponent* sprite) {
#define ARGS ((UI_SpriteArg*)sprite->args)
    static uint32_t last_time = 0;
    if(!sprite) return;
    if(!last_time) {
        last_time = GetTick();
        return;
    }

    uint16_t delta_time = GetTick() - last_time;
    last_time = GetTick();


    // 先更新速度（包括加速度影响）
    ARGS->v_y += ARGS->a_y * delta_time / 1000;
    ARGS->v_x += ARGS->a_x * delta_time / 1000;

    // 再更新位置
    ARGS->Y += ARGS->v_y * delta_time / 1000;
    ARGS->X += ARGS->v_x * delta_time / 1000;

// #define reduce 1.02  // 1.02为无衰减
//     if (ARGS->Y > OLED_HEIGHT) {
//         ARGS->Y = OLED_HEIGHT;  // 校正位置，避免深入屏幕下方
//         ARGS->v_y = -ARGS->v_y * reduce;  // 反弹并损失部分能量
//     }
// #undef reduce

}

void Sprite_Draw(UIComponent* sprite) {
#define ARGS ((UI_SpriteArg*)sprite->args)
    if(!sprite || !ARGS->sticker) return;
    Sprite_Update(sprite);
    Sticker_Draw(ARGS->sticker,ARGS->X,ARGS->Y);
}


void Sprite_Desdroy(UIComponent* sprite) {
    free(sprite->args);
    free(sprite);
}

UIComponent* Sprite_Generate(Sticker* sticker) {
    UIComponent* Sprite = malloc(sizeof(UIComponent));
    UI_SpriteArg* args = malloc(sizeof(UI_SpriteArg));
    args->a_y = 0;
    args->a_x = 0;
    args->v_y = 0;
    args->v_x = 0;
    if(!Sprite || !args) {
        free(Sprite);
        free(args);
        return NULL;
    }
    Sprite->draw_func = Sprite_Draw;
    Sprite->destroy_func = Sprite_Desdroy;
    Sprite->args = args;

    args->sticker = sticker;
    args->X = 0;
    args->Y = 0;

    UI_RegisterComponent(Sprite);

    return Sprite;
}

void Sprite_setXVelocity(UIComponent* sprite,float v_x) {
    ((UI_SpriteArg*)sprite->args)->v_x = v_x;
}

void Sprite_setYVelocity(UIComponent* sprite,float v_y) {
    ((UI_SpriteArg*)sprite->args)->v_y = v_y;
}

void Sprite_setXAcceleration(UIComponent* sprite,float a_x) {
    ((UI_SpriteArg*)sprite->args)->a_x = a_x;
}

void Sprite_setYAcceleration(UIComponent* sprite,float a_y) {
    ((UI_SpriteArg*)sprite->args)->a_y = a_y;
}

void Sprite_place(UIComponent* sprite,float X,float Y) {
    ((UI_SpriteArg*)sprite->args)->X = X;
    ((UI_SpriteArg*)sprite->args)->Y = Y;
}
