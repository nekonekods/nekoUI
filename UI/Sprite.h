//
// Created by 31910 on 2025/9/9.
//

#ifndef SPRITE_H
#define SPRITE_H

#include "sticker.h"
#include "ui_core.h"

typedef struct UI_SpriteArg {
    float X;
    float Y;

    Sticker* sticker;
    float v_y;
    float v_x;

    float a_y;
    float a_x;
}UI_SpriteArg;

UIComponent* Sprite_Generate(Sticker* sticker);
void Sprite_place(UIComponent* sprite,float X,float Y);
void Sprite_setXVelocity(UIComponent* sprite,float);
void Sprite_setYVelocity(UIComponent* sprite,float);

void Sprite_setXAcceleration(UIComponent* sprite,float);
void Sprite_setYAcceleration(UIComponent* sprite,float);


#endif //SPRITE_H
