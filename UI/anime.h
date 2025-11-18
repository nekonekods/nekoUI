//
// Created by 31910 on 2025/6/15.
//


#ifndef ANIME_H
#define ANIME_H
#include "ui_core.h"

typedef float (*UIAnimationFunc)(float);
typedef  int16_t st_ed_g[2];
typedef struct UIComponent UIComponent;


/**
 * @brief 动画结构体
 * @param UpdateArgsNum 动画更新参数个数
 * @param UpdateArgs 动画更新参数列表
 * @param UpdateArgSt_Ed 动画更新参数起始和终止位置
 * @param UpdateFuncs 动画更新曲线函数列表
 * @param UpdateOtherArgs 主参数更新后，子参数更新函数
 * @param start_tick 动画开始时间（滴答数）
 * @param duration 动画总时长（滴答数）
 *
 * */
typedef struct UIAnimation{
    uint8_t UpdateArgsNum; //动画更新参数个数
    int16_t **UpdateArgs; //动画更新参数列表
    int16_t (*UpdateArgSt_Ed)[2]; //动画更新参数起始和终止位置
    UIAnimationFunc* UpdateFuncs; //动画更新曲线函数列表
    uint32_t start_tick;    // 动画开始时间（滴答数）
    uint32_t duration;    // 动画总时长（滴答数）
    void (*StartFunc)(UIComponent*);
    void (*FinishedFunc)(UIComponent*);

} UIAnimation;


UIAnimation *Anime_Create(UIComponent *component, uint8_t argNum, int16_t *updateArgsPtr[], int16_t updateArgSt_Ed[][2],
                          UIAnimationFunc updateFuncs[], uint32_t duration, void (*startFunc)(UIComponent *),
                          void (*finishedFunc)(UIComponent *));

void cAnime_Destroy(UIComponent* conponent);
void Anime_Destroy(UIAnimation* anime);

float fly_in(float);
float jump_in(float);
float jump_out(float);
float shake(float);
float fly_in_sp(float);
float bounce(float x);
#endif //ANIME_H
