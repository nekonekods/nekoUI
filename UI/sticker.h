//
// Created by 31910 on 2025/9/6.
//

#ifndef STICKER_H
#define STICKER_H
#include <stdint.h>
#include <ui_core.h>

typedef struct {

    uint16_t Width;  //静态，被动参数
    uint16_t Height; //静态，被动参数
    const uint8_t* Picture;
    const uint8_t* Mask;

} Sticker;

Sticker* Sticker_Generate(const uint8_t* Picture, const uint8_t* Mask, uint16_t Width, uint16_t Height);

void Sticker_Draw(Sticker* sticker,int16_t X,int16_t Y);

void Sticker_Destroy(Sticker * sticker);
#endif //STICKER_H
