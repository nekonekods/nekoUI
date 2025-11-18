//
// Created by 31910 on 2025/5/30.
//

#ifndef UNTITLED4_CONFIG_H
#define UNTITLED4_CONFIG_H

#include <stdint.h>

// 定义屏幕宽度 (px)
#define OLED_WIDTH (int16_t)128
// 定义屏幕高度 (px)
#define OLED_HEIGHT 64
// 定义滴答率 (Hz)
#define TICK_RATE 100

// 定义提示框显示时长 (ms)
#define TOAST_DURATION 2000
// 定义提示框左右边距 (px)
#define TOAST_HOR_MARGIN 10
// 定义提示框上下边距 (px)
#define TOAST_VER_MARGIN 7
// 定义出现动画持续时间 (ms)
#define TOAST_ANIME_DURATION 500
// 定义提示框最大行数
#define TOAST_MAX_LINES 4

// 定义提示框弹跳高度 (px)
#define MSGBOX_BOUNCE_HEIGHT 4
// 定义提示框弹跳动画时长 (ms)
#define MSGBOX_BOUNCE_ANIME_DURATION 300

//列表宽度
#define LIST_WIDTH 64
//列表高度
#define LIST_HEIGHT 64
//一页里面显示几项
#define LIST_SHOW_NUM 4
//动画（出入）时长
#define LIST_ANIME_DURATION 500
//切换动画时长
#define LIST_CHANGE_DURATION 200
//抖动大小（px）
#define LIST_SHAKE_AMPLITUDE 5
//列表文字和左边的边界的距离（左内边距）
#define LIST_LEFT_PADDING 3
//选择框上下外边距（这个数据是单边的）
#define LIST_SEL_BOX_V_MARGIN 1






#endif //UNTITLED4_CONFIG_H
