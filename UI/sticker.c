//
// Created by 31910 on 2025/9/6.
//

#include "sticker.h"

#include <stdlib.h>

#include "OLED.h"

void Sticker_Draw(Sticker* sticker,int16_t X,int16_t Y) {
    set_color(0);
    OLED_PrintImage(X, Y, sticker->Width, sticker->Height, sticker->Mask);
    set_color(1);
    OLED_PrintImage(X, Y, sticker->Width, sticker->Height, sticker->Picture);
}

void Sticker_Destroy(Sticker * sticker) {
    free(sticker);
}

Sticker* Sticker_Generate(const uint8_t* Picture, const uint8_t* Mask, uint16_t Width, uint16_t Height) {
    Sticker* sticker = malloc(sizeof(Sticker));
    if(!sticker) {
        free(sticker);
        return NULL;
    }

    sticker->Picture = Picture;
    sticker->Mask = Mask;
    sticker->Width = Width;
    sticker->Height = Height;

    return sticker;
}


