//
// Created by 31910 on 2025/6/21.
//


/**
 * @file toast.c
 * @brief List组件实现文件
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "OLED.h"
#include "Ticker.h"

#include "ui_core.h"
#include "list.h"
#include "anime.h"
#include "config.h"

#define UP (-1)
#define DOWN 1

void List_EmptyFunc(UIComponent *list) {
}

void List_Draw(UIComponent *component);

uint8_t List_LoadAppearAnime(UIComponent *);

uint8_t List_LoadDisappearAnime(UIComponent *);

// List组件绘制函数
void List_Draw(UIComponent *component) {
    const UI_ListArgs *args = (const UI_ListArgs *) component->args;

    if (!args) return;
    OLED_ClearArea(args->X, args->Y,
                   LIST_WIDTH, LIST_HEIGHT);

    OLED_DrawLine(args->line_distance,0,args->line_distance,OLED_HEIGHT);
    OLED_DrawLine(OLED_WIDTH-args->line_distance,0,OLED_WIDTH-args->line_distance,OLED_HEIGHT);

    // 绘制List文本（居中显示）
    //    OLED_ShowString(args->X + TOAST_HOR_MARGIN, args->Y + TOAST_VER_MARGIN, args->Text, OLED_8X16);
    for (int i = 0; i < args->itemNum; i++) {
        if (i >= args->itemNum) {  //显示完了
            break;
        }
        int8_t thisY = args->Y + (args->each_height - 16) / 2 + args->each_height * i;  // (args->each_height - 16) 是上边缘宽
        if(thisY < -args->each_height || thisY > OLED_HEIGHT){   //超过屏幕的不渲染
            continue;
        }
        OLED_ShowString(args->X + LIST_LEFT_PADDING, thisY,
                        args->items[i], OLED_8X16);
        OLED_DrawLine(args->X, args->Y + args->each_height * i,
                      args->X + LIST_WIDTH - 1, args->Y + args->each_height * i);
    }
    set_color(2);
    OLED_DrawRectangle(args->sel_box_X + LIST_LEFT_PADDING, args->sel_box_Y + LIST_SEL_BOX_V_MARGIN,
                       args->sel_box_width, args->sel_box_height - 2 * LIST_SEL_BOX_V_MARGIN, 1);
    set_color(1);
}

// List组件销毁函数
void List_Destroy(UIComponent *list) {
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    for (int i = 0; i < args->itemNum; i++) {
        free(args->items[i]);
    }

    free(args);
    cAnime_Destroy(list);
    Anime_Destroy(args->List_row_anime);
    Anime_Destroy(args->List_box_mov_anime);
    Anime_Destroy(args->List_shake_anime);
    free(list);
}

/**
 * @warning 须确保 items 是一个常驻变量。
 * */
UIComponent *List_Create(uint8_t itemNum,void (*confirm_Func)(UIComponent *list), String *items) {
    // 分配组件内存
    UIComponent *list = (UIComponent *) malloc(sizeof(UIComponent));  // auto free
    UI_ListArgs *args = (UI_ListArgs *) malloc(sizeof(UI_ListArgs));  // remember free
    uint8_t *itemWidth = (uint8_t *) malloc(sizeof(uint8_t) * itemNum);    // need free 1 by 1

    if (!list || !args || !itemWidth) {
        free(args);
        free(list);
        free(itemWidth);
        return NULL;
    }

    memset(list, 0, sizeof(UIComponent));
    memset(args, 0, sizeof(UI_ListArgs));
    memset(itemWidth, 0, sizeof(uint8_t) * itemNum);

    for (int i = 0; i < itemNum; i++) {
        itemWidth[i] = OLED_getStringWidth(items[i]);
    }

    // 设置回调函数
    list->state = UI_STATE_NORMAL;
    list->draw_func = List_Draw;
    list->destroy_func = List_Destroy;

    args->X = (OLED_WIDTH - LIST_WIDTH) / 2;
    args->Y = OLED_HEIGHT - LIST_HEIGHT;
    args->each_height = LIST_HEIGHT / LIST_SHOW_NUM;
    args->itemNum = itemNum;
    args->items = items;
    args->itemWidth = itemWidth;
    args->firstIndex = 0;
    args->confirm_Func = 0;
    args->line_distance = args->X-5;
    args->confirm_Func = confirm_Func;

    args->sel_box_X = args->X;
    args->sel_box_Y = args->Y;
    args->sel_box_width = args->itemWidth[0];
    args->sel_box_height = args->each_height;

    args->List_shake_anime = NULL;
    args->List_box_mov_anime = NULL;
    args->List_row_anime = NULL;

    list->args = args;

    return list;
}

uint8_t List_LoadAppearAnime(UIComponent *list) {
    // 定义动画属性
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    int16_t *updateArgs[] = {
            &(args->Y),
            &(args->sel_box_Y),
            &(args->sel_box_width),                                                                                                                             (int16_t *)&(args->line_distance)
    };
    st_ed_g updateArgSt_Ed[] = {
            {OLED_HEIGHT, args->Y},
            {OLED_HEIGHT, args->sel_box_Y},
            {0,           args->itemWidth[args->selected_index]},
            {0,           args->line_distance}
    };
    UIAnimationFunc updateFuncs[] = {
            fly_in,
            fly_in,
            fly_in,
            fly_in,
    };

    if (list->animation) {
        cAnime_Destroy(list);
    }

    list->animation = Anime_Create(list,
                                   4,
                                   updateArgs,
                                   updateArgSt_Ed,
                                   updateFuncs,
                                   LIST_ANIME_DURATION,
                                   List_EmptyFunc,
                                   cAnime_Destroy);
    if (list->animation == NULL) {
        return 0;
    }
    return 1;
}

uint8_t List_LoadDisappearAnime(UIComponent *list) {
    // 定义动画属性
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    int16_t *updateArgs[] = {
            &(args->Y),
            &(args->sel_box_Y),
            &(args->sel_box_width),
            (int16_t *)&(args->line_distance)
    };
    st_ed_g updateArgSt_Ed[] = {
            {args->Y,         OLED_HEIGHT},
            {args->sel_box_Y, OLED_HEIGHT},
            {args->sel_box_width, 0},
            {args->line_distance,0}
    };

    UIAnimationFunc updateFuncs[] = {
            fly_in,
            fly_in,
            fly_in,
            fly_in
    };

    list->animation = Anime_Create(list,
                                   4,
                                   updateArgs,
                                   updateArgSt_Ed,
                                   updateFuncs,
                                   LIST_ANIME_DURATION,
                                   List_EmptyFunc,
                                   UI_RemoveComponent
                                   );
    if (list->animation == NULL) {
        return 0;
    }
    return 1;
}

void List_ShakeBox(UIComponent *list) {
    if (!list) return;
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    if (args->List_shake_anime == NULL) {
        int16_t *updateArgs[] = {&(args->sel_box_X)};
        st_ed_g updateArgSt_Ed[] = {
                {args->X, args->X + LIST_SHAKE_AMPLITUDE}
        };
        UIAnimationFunc updateFuncs[] = {shake};

        args->List_shake_anime = Anime_Create(list,
                                        1,
                                        updateArgs,
                                        updateArgSt_Ed,
                                        updateFuncs,
                                        LIST_CHANGE_DURATION,
                                        List_EmptyFunc,
                                        List_EmptyFunc);
        list->animation = args->List_shake_anime;
        if (list->animation == NULL) {
            return;
        }
    } else {
        list->animation = args->List_shake_anime;
    }
    UI_StartAnime(list);
}

void List_row(UIComponent *list, int8_t dir, int8_t width) {
    // 上滚：执行（下移）
    // 下滚：{扩展}，执行（上移），{移标，回复拓展}

    if (!list) return;
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    if (args->List_row_anime == NULL) {
        int16_t *updateArgs[] = {
                &(args->Y),
                &(args->sel_box_width)
        };

        st_ed_g updateArgSt_Ed[] = {
                {args->Y,args->Y + dir * args->each_height},
                {args->sel_box_width,width}
        };

        UIAnimationFunc updateFuncs[] = {
                fly_in,
                fly_in};

        args->List_row_anime = Anime_Create(list,
                                      2,
                                      updateArgs,
                                      updateArgSt_Ed,
                                      updateFuncs,
                                      LIST_CHANGE_DURATION,
                                      List_EmptyFunc,
                                      List_EmptyFunc);
        list->animation = args->List_row_anime;

        if (list->animation == NULL) {
            return;
        }
    } else {  //仅做参数的调整
        args->List_row_anime->UpdateArgSt_Ed[0][0] = args->Y;
        args->List_row_anime->UpdateArgSt_Ed[0][1] += dir * args->each_height;//只是把起点向上抬了，终点还是那个位置。
        args->List_row_anime->UpdateArgSt_Ed[1][0] = args->sel_box_width;
        args->List_row_anime->UpdateArgSt_Ed[1][1] = width;
        list->animation = args->List_row_anime;
    }
    UI_StartAnime(list);
}

void List_sel_box_move(UIComponent *list, int8_t dir, int8_t width) {
    if (!list) return;
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    if (args->List_box_mov_anime == NULL) {
        int16_t *updateArgs[] = {
                &(args->sel_box_Y),
                &(args->sel_box_width)
        };

        st_ed_g updateArgSt_Ed[] = {
                {args->sel_box_Y,     args->sel_box_Y + dir * args->each_height},
                {args->sel_box_width, width}
        };

        UIAnimationFunc updateFuncs[] = {
                fly_in,
                fly_in
        };

        args->List_box_mov_anime = Anime_Create(list,
                                          2,
                                          updateArgs,
                                          updateArgSt_Ed,
                                          updateFuncs,
                                          LIST_CHANGE_DURATION,
                                          List_EmptyFunc,
                                          List_EmptyFunc);
        list->animation = args->List_box_mov_anime;

        if (list->animation == NULL) {
            return;
        }
    } else {  //仅做参数的调整
        args->List_box_mov_anime->UpdateArgSt_Ed[0][0] = args->sel_box_Y;
        args->List_box_mov_anime->UpdateArgSt_Ed[0][1] += dir * args->each_height;
        args->List_box_mov_anime->UpdateArgSt_Ed[1][0] = args->sel_box_width;
        args->List_box_mov_anime->UpdateArgSt_Ed[1][1] = width;
        list->animation = args->List_box_mov_anime;
    }
    UI_StartAnime(list);
}


UIComponent *List_Generate_l(uint8_t itemNum,void (*confirm_Func)(UIComponent *list), String *items) {
    UIComponent *list = List_Create(itemNum,confirm_Func, items);

    if (!list) return 0;
    if (!List_LoadAppearAnime(list)) {
        List_Destroy(list);
        return 0;
    }

    UI_StartAnime(list);
    UI_RegisterComponent(list);
    return list;
}

UIComponent *List_Generate(uint8_t itemNum,void (*confirm_Func)(UIComponent *list), ...) {

    va_list arg;
    String *items = (String *) malloc(sizeof(String) * itemNum);      // need free 1 by 1
    String str;
    va_start(arg, confirm_Func);
    for (int i = 0; i < itemNum; i++) {
        str = va_arg(arg, String);   //原始数据
        items[i] = malloc(sizeof(char) * strlen(str) + 1);  //free first
        if (items[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(items[j]);
            }
            free(items);
            return 0;
        }
        strcpy(items[i], str);
    }
    va_end(arg);

    return List_Generate_l(itemNum, confirm_Func,items);
}

uint8_t List_prv(UIComponent *list) {
    if(!list) return 0;
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    if (args->selected_index == 0) { //最上面，不动
        List_ShakeBox(list);
        return 0;
    } else if (args->selected_index == args->firstIndex + 1 && args->firstIndex != 0) {
        //当前页面的第二个，且没顶头
        args->selected_index--;
        args->firstIndex--;
        //sel_box变换宽度，列表下滚
        List_row(list, DOWN, args->itemWidth[args->selected_index]);
    } else {
        args->selected_index--;
        //sel_box向上走
        List_sel_box_move(list, UP, args->itemWidth[args->selected_index]);
    }
}

uint8_t List_nxt(UIComponent *list) {
    if(!list) return 0;
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    if (args->selected_index == args->itemNum - 1) { //不动
        List_ShakeBox(list);
        return 0;
    } else if (args->selected_index == args->firstIndex + LIST_SHOW_NUM -2 && args->firstIndex+ LIST_SHOW_NUM  != args->itemNum  ) {   //同时减1，可以不用减
        //当前页面的第二个，且没顶头
        args->selected_index++;
        args->firstIndex++;
        //sel_box变换宽度，列表上滚
        List_row(list, UP, args->itemWidth[args->selected_index]);
    } else {
        args->selected_index++;
        //sel_box向下走
        List_sel_box_move(list, DOWN, args->itemWidth[args->selected_index]);
    }
}

uint8_t List_select(UIComponent *list){
    if(!list) return 0;
    UI_ListArgs *args = (UI_ListArgs *) list->args;
    args->confirm_Func(list);
    List_LoadDisappearAnime(list);
    UI_StartAnime(list);
}
