/**
 * @file toast.c
 * @brief Toast组件实现文件 Toast组件继承于MsgBox组件
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "OLED.h"
#include "config.h"
#include "Ticker.h"
#include "toast.h"

#include <MsgBox.h>

#include "anime.h"
#include "ui_core.h"

void Toast_EmptyFunc(UIComponent *_){};
// Toast组件销毁函数
void Toast_Destroy(UIComponent *toast) {
    free(((UI_ToastArgs *) toast->args)->LinePtrs);
    free(((UI_ToastArgs *) toast->args)->Text);
    free(toast->args);
    cAnime_Destroy(toast);
    free(toast);
}

uint8_t Toast_LoadDisappearAnime(UIComponent *toast) {
    // 定义动画属性
    int16_t *updateArgs[] = {&(((UI_ToastArgs *) toast->args)->Y)};

    st_ed_g updateArgSt_Ed[] = {
        {((UI_ToastArgs *) toast->args)->Y, OLED_HEIGHT}
    };

    UIAnimationFunc updateFuncs[] = {fly_in};

    if (toast->animation) {
        cAnime_Destroy(toast);
    }

    toast->animation = Anime_Create(toast,
                                    1,
                                    updateArgs,
                                    updateArgSt_Ed,
                                    updateFuncs,
                                    TOAST_ANIME_DURATION,
                                    Toast_EmptyFunc,
                                    UI_RemoveComponent);
    if (toast->animation == NULL) {
        return 0;
    }
    return 1;
}

void Toast_AddDisFunc(UIComponent *toast) {
    Toast_LoadDisappearAnime(toast);
    UI_StartAnime(toast);
}

void Toast_AppearFinFunc(UIComponent *toast) {
    UI_AddTask(GetTick() + TOAST_DURATION, Toast_AddDisFunc, toast)
    -> finish = 1;
}

uint8_t Toast_LoadAppearAnime(UIComponent *toast) {
    // 定义动画属性
    int16_t *updateArgs[] = {&(((UI_ToastArgs *) toast->args)->Y)};
    st_ed_g updateArgSt_Ed[] = {
            {OLED_HEIGHT, ((UI_ToastArgs *) toast->args)->Y}   //这里有点绕，因为初始化时就把y的最终
            //值给了y，所以这里直接借用，在后续更
            //新中就将原来的值覆盖掉了。
    };
    UIAnimationFunc updateFuncs[] = {jump_in};

    if (toast->animation) {
        cAnime_Destroy(toast);
    }

    toast->animation = Anime_Create(toast,
                                    1,
                                    updateArgs,
                                    updateArgSt_Ed,
                                    updateFuncs,
                                    TOAST_ANIME_DURATION,
                                    Toast_EmptyFunc,
                                    Toast_AppearFinFunc);
    if (toast->animation == NULL) {
        return 0;
    }
    return 1;
}

int8_t Toast_Generate(const char *format, ...) {

    char text[128];
    va_list arg;                            //定义可变参数列表数据类型的变量arg
    va_start(arg, format);                    //从format开始，接收参数列表到arg变量
    vsprintf(text, format, arg);            //使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);

    UIComponent *toast = MsgBox_Create(text);
    if (!toast) return 0;
    if (!Toast_LoadAppearAnime(toast)) {
        Toast_Destroy(toast);
        return 0;
    }

    UI_StartAnime(toast);
    UI_RegisterComponent(toast);
    return 1;
}
