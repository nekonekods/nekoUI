//
// Created by 31910 on 2025/6/15.
//
#include <anime.h>
#include <stdlib.h>
#include <string.h>
#include "ui_core.h"


void cAnime_Destroy(UIComponent *conponent) {
    Anime_Destroy(conponent->animation);
}

void Anime_Destroy(UIAnimation *anime) {
    // 释放动画资源
    if (!anime) return;
    free(anime->UpdateArgs);
    free(anime->UpdateArgSt_Ed);
    free(anime->UpdateFuncs);
    free(anime);
}


UIAnimation *Anime_Create(UIComponent *component, uint8_t argNum, int16_t *updateArgsPtr[], int16_t updateArgSt_Ed[][2],
                          UIAnimationFunc updateFuncs[], uint32_t duration, void (*startFunc)(UIComponent *),
                          void (*finishedFunc)(UIComponent *)) {
    // 分配组件内存
    UIAnimation *anime = (UIAnimation *) malloc(sizeof(UIAnimation) * argNum);
    st_ed_g *st_ed = (st_ed_g *) malloc(sizeof(st_ed_g) * argNum);
    int16_t **updateArgs = (int16_t **) malloc(sizeof(int16_t *) * argNum);
    UIAnimationFunc *func_lst = (UIAnimationFunc *) malloc(sizeof(UIAnimationFunc) * argNum);

    if (!anime || !st_ed || !updateArgs || !func_lst) {
        free(anime);
        free(st_ed);
        free(updateArgs);
        free(func_lst);
        return NULL;
    }

    memcpy(st_ed, updateArgSt_Ed, sizeof(st_ed_g) * argNum);
    memcpy(updateArgs, updateArgsPtr, sizeof(int16_t *) * argNum);
    memcpy(func_lst, updateFuncs, sizeof(UIAnimationFunc) * argNum);

    // 这两个if!都是无法分配内存的情况。
    memset(anime, 0, sizeof(UIAnimation));

    anime->UpdateArgsNum = argNum;
    anime->UpdateArgs = updateArgs;
    anime->UpdateArgSt_Ed = st_ed;
    anime->UpdateFuncs = func_lst;
    anime->duration = duration;
    anime->FinishedFunc = finishedFunc;
    anime->StartFunc = startFunc;

    return anime;
}


float fly_in(float x) {
    if (x >= 1) return 1;

    x = x - 1;
    //    立方曲线 y = 1+(x-1)^3

    return x * x * x + 1;
}

float fly_in_sp(float x) {
    if (x >= 1) return 0;

    x = x - 1;
    //    立方曲线 y = 1+(x-1)^3

    return x * x * x + 1;
}

float jump_in(float x) {
    //平方曲线 y = 1-(x-1)^2
    if (x >= 1) return 1;
    return -1.875 * x * x + 2.875 * x;
}

float jump_out(float x) {
    //平方曲线 y = 1-(x-1)^2
    if (x >= 1) return 1;
    x = x - 1;
    return 1.875 * x * x + 2.875 * x + 1;
}

float shake(float x) {
    if (0 <= x && x < 0.2) {
        return -40 * x * x + 8 * x;
    } else if (0.2 <= x && x < 0.5) {
        return 35.56 * x * x - 24.89 * x + 3.556;
    } else if (0.5 <= x && x < 0.8) {
        return -35.56 * x * x + 46.22 * x - 14.22;
    } else if (0.8 <= x && x < 1) {
        return 40 * x * x - 72 * x + 32;
    } else if (x == 1) {
        return 0;
    }
}

float bounce(float x) {
    // return -4 * x * x + 4 * x;  //左右平衡的
    if (0 <= x && x <= 0.25) {
        return -16 * x * x + 8 * x;
    } else if (0.25 <= x && x < 1) {
        return -1.778 * x * x + 0.889 * x + 0.889;
    }else if(x == 1) {
        return 0;
    }
}
