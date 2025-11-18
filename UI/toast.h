/**
 * @file toast.h
 * @brief Toast组件头文件
 */

#ifndef TOAST_H
#define TOAST_H

#include "ui_core.h"

/**
 * @brief Toast参数结构体
 * @param X 组件左上角X坐标
 * @param Y 组件左上角Y坐标
 * @param Width 组件宽度
 * @param Height 组件高度
 * @param TextWidth 文本宽度
 */

typedef struct {
    int16_t X;    //静态，被动参数
    int16_t Y;    //动画，被动参数
    uint16_t Width;  //静态，被动参数
    uint16_t Height; //静态，被动参数
    uint16_t TextMaxWidth;  //静态，被动
    char* Text;   //静态，主动
    uint8_t* LinePtrs;   //静态，被动
    uint8_t* LineWidth;  //静态，被动
    uint8_t LinePtrNum;  //静态，被动
} UI_ToastArgs;


int8_t Toast_Generate(const char* format, ...);

#endif // TOAST_H
