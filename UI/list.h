//
// Created by 31910 on 2025/6/21.
//
#include <stdint.h>

#include "ui_core.h"

#ifndef UNTITLED4_LIST_H
#define UNTITLED4_LIST_H

typedef char* String;

typedef struct UI_ListArgs{
    int16_t X;
    int16_t Y;
    int8_t line_distance;
    uint16_t each_height;
    uint8_t itemNum;
    String* items;
    uint8_t* itemWidth;
    uint8_t firstIndex;

    void (*confirm_Func)(UIComponent *list);

    UIAnimation *List_shake_anime;
    UIAnimation *List_box_mov_anime;
    UIAnimation *List_row_anime;

    int8_t selected_index;
    int16_t sel_box_X;
    int16_t sel_box_Y;
    int16_t sel_box_width;
    int16_t sel_box_height;

} UI_ListArgs;

UIComponent* List_Generate(uint8_t itemNum,void (*confirm_Func)(UIComponent *list),...);
UIComponent* List_Generate_l(uint8_t itemNum,void (*confirm_Func)(UIComponent *list), String *items);

uint8_t List_prv(UIComponent *list);
uint8_t List_nxt(UIComponent *list);

uint8_t List_select(UIComponent *list);
#endif //UNTITLED4_LIST_H
