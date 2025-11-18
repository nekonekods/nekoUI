/**
 * @file MsgBox.c
 * @brief MsgBox组件实现文件
 */

#include <anime.h>
#include <MsgBox.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <Ticker.h>

#include "config.h"
#include "OLED.h"



UIAnimation *MsgBox_BounceAnime = 0;

void MsgBox_EmptyFunc(UIComponent *box) {
}

void MsgBox_Draw(UIComponent *box);

UIComponent *MsgBox_Create(const char *);

uint8_t MsgBox_LoadAppearAnime(UIComponent *);

uint8_t MsgBox_LoadDisappearAnime(UIComponent *);

void UI_StartAnime(UIComponent *);

// MsgBox组件绘制函数
void MsgBox_Draw(UIComponent *box) {
    const UI_MsgBoxArgs *args = (const UI_MsgBoxArgs *) box->args;

    if (!args) return;
    OLED_ClearArea(args->X, args->Y,
                   args->Width, args->Height);
    // 绘制Toast背景
    //    OLED_DrawRoundedRect(args->X, args->Y,args->Width, args->Height, 7,0);
    OLED_DrawRectangle(args->X, args->Y, args->Width, args->Height, 0);

    // 绘制Toast文本（居中显示）
    //    OLED_ShowString(args->X + TOAST_HOR_MARGIN, args->Y + TOAST_VER_MARGIN, args->Text, OLED_8X16);
    for (int i = 0; i < args->LinePtrNum; i++) {
        OLED_ShowString((OLED_WIDTH - args->LineWidth[i]) / 2, args->Y + TOAST_VER_MARGIN + 16 * i,
                        (const char *) (args->Text + args->LinePtrs[i]), OLED_8X16);
    }
}

// MsgBox组件销毁函数
void MsgBox_Destroy(UIComponent *box) {
    free(((UI_MsgBoxArgs *) box->args)->LinePtrs);
    free(((UI_MsgBoxArgs *) box->args)->Text);
    free(box->args);
    //    free(box->cur_args);
    cAnime_Destroy(box);
    free(box);
}

/**
 * @brief 更新消息框文本（raw）
 * @param box 消息框组件
 * @param text 要显示的文本
 * @return 0 失败 1 成功
 */
uint8_t rMsgBox_UpdateText(UIComponent *box, const char *text) {
    char *text_copy = malloc(strlen(text) + 1); //要分配字符串大小的空间
    uint8_t *strPtr = malloc(sizeof(uint8_t) * TOAST_MAX_LINES);
    uint8_t *strWthPtr = malloc(sizeof(uint8_t) * TOAST_MAX_LINES);
    if (!text_copy || !strPtr || !strWthPtr) {
        free(text_copy);
        free(strPtr);
        free(strWthPtr);
        return 0;
    }
    // 复制文本
    strcpy(text_copy, text);
    // 设置Toast属性
    int8_t num = 1;
    strPtr[0] = 0;
    int8_t len = strlen(text_copy);
    for (int i = 0; i < len && num < TOAST_MAX_LINES; i++) {
        if (text_copy[i] == '\n') {
            //将回车转义为多个字符串
            text_copy[i] = '\0';
            strPtr[num] = i + 1;
            num++;
        }
    }

    uint8_t *prev = strPtr;
    strPtr = (uint8_t *) malloc(sizeof(uint8_t) * num);
    memcpy(strPtr, prev, sizeof(uint8_t) * num);
    if (strPtr == NULL) {
        free(text_copy);
        free(strPtr);
        free(strWthPtr);
        free(prev);
        return 0;
    }
    free(prev);

    uint8_t max = 0;
    for (int i = 0; i < num; i++) {
        if ((strWthPtr[i] = (OLED_getStringWidth((const char *) (text_copy + strPtr[i])))) > max) {
            max = strWthPtr[i];
        }
    }

    prev = strWthPtr;
    strWthPtr = (uint8_t *) malloc(sizeof(uint8_t) * num);
    memcpy(strWthPtr, prev, sizeof(uint8_t) * num);
    if (strWthPtr == NULL) {
        free(box);
        free(text_copy);
        free(strPtr);
        free(strWthPtr);
        free(prev);
        return 0;
    }
    free(prev);

    UI_MsgBoxArgs *args = box->args;
    free(args->Text);
    args->Text = text_copy;
    free(args->LinePtrs);
    args->LinePtrs = strPtr;
    args->LinePtrNum = num;
    args->TextMaxWidth = max;
    free(args->LineWidth);
    args->LineWidth = strWthPtr;
    args->Width = args->TextMaxWidth + 2 * TOAST_HOR_MARGIN;
    args->Height = 16 * args->LinePtrNum + 2 * TOAST_VER_MARGIN;
    args->X = (OLED_WIDTH - args->TextMaxWidth) / 2 - TOAST_HOR_MARGIN;
    args->Y = (OLED_HEIGHT - args->Height) / 2;

    return 1;
}

UIComponent *MsgBox_Create(const char *text) {
    // 分配组件内存
    UIComponent *box = malloc(sizeof(UIComponent));
    UI_MsgBoxArgs *args = malloc(sizeof(UI_MsgBoxArgs));

    if (!box || !args) {
        free(args);
        free(box);
        return NULL;
    }

    memset(box, 0, sizeof(UIComponent));
    memset(args, 0, sizeof(UI_MsgBoxArgs));

    // 设置回调函数
    box->state = UI_STATE_NORMAL;
    box->draw_func = MsgBox_Draw;
    box->destroy_func = MsgBox_Destroy;

    box->args = args;

    if (!rMsgBox_UpdateText(box, text)) {
        free(args);
        free(box);
        return NULL;
    }
    return box;
}

void MsgBox_Finish(UIComponent *box) {
    MsgBox_LoadDisappearAnime(box);
    UI_StartAnime(box);
}

void MsgBox_AppearFinFunc(UIComponent *box) {
    UI_AddTask(
                GetTick() + TOAST_DURATION,
                MsgBox_Finish,
                box)
            ->finish = 1;
}

uint8_t MsgBox_LoadAppearAnime(UIComponent *toast) {
    // 定义动画属性
    int16_t *updateArgs[] = {&(((UI_MsgBoxArgs *) toast->args)->Y)};
    st_ed_g updateArgSt_Ed[] = {
        {OLED_HEIGHT, ((UI_MsgBoxArgs *) toast->args)->Y} //这里有点绕，因为初始化时就把y的最终
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
                                    MsgBox_EmptyFunc,
                                    /*MsgBox_AppearFinFunc*/MsgBox_EmptyFunc);
    if (toast->animation == NULL) {
        return 0;
    }
    return 1;
}

uint8_t MsgBox_LoadDisappearAnime(UIComponent *box) {
    // 定义动画属性
    int16_t *updateArgs[] = {&(((UI_MsgBoxArgs *) box->args)->Y)};

    st_ed_g updateArgSt_Ed[] = {
        {((UI_MsgBoxArgs *) box->args)->Y, OLED_HEIGHT}
    };

    UIAnimationFunc updateFuncs[] = {fly_in};

    if (box->animation) {
        cAnime_Destroy(box);
    }

    box->animation = Anime_Create(box,
                                  1,
                                  updateArgs,
                                  updateArgSt_Ed,
                                  updateFuncs,
                                  TOAST_ANIME_DURATION,
                                  MsgBox_EmptyFunc,
                                  UI_RemoveComponent);
    if (box->animation == NULL) {
        return 0;
    }
    return 1;
}

// uint8_t MsgBox_LoadBounceAnime(UIComponent *box) {
//     if (!MsgBox_BounceAnime) {
//         // 定义动画属性
//         int16_t *updateArgs[] = {&((UI_MsgBoxArgs *) box->args)->Y};
//
//         st_ed_g updateArgSt_Ed[] = {
//             {((UI_MsgBoxArgs *) box->args)->Y, ((UI_MsgBoxArgs *) box->args)->Y - MSGBOX_BOUNCE_HEIGHT}
//         };
//
//         UIAnimationFunc updateFuncs[] = {bounce};
//
//         MsgBox_BounceAnime = Anime_Create(box,
//                                           1,
//                                           updateArgs,
//                                           updateArgSt_Ed,
//                                           updateFuncs,
//                                           MSGBOX_BOUNCE_ANIME_DURATION,
//                                           MsgBox_EmptyFunc,
//                                           MsgBox_EmptyFunc);
//         if (MsgBox_BounceAnime == NULL) {
//             return 0;
//         }
//     }
//     box->animation = MsgBox_BounceAnime;
//     return 1;
//
// }

uint8_t MsgBox_LoadBounceAnime(UIComponent *box) {
    if (!MsgBox_BounceAnime) {
        // 定义动画属性
        int16_t *updateArgs[] = {
            &((UI_MsgBoxArgs *) box->args)->X,
            &((UI_MsgBoxArgs *) box->args)->Y,
            &((UI_MsgBoxArgs *) box->args)->Width,
            &((UI_MsgBoxArgs *) box->args)->Height
        };

        st_ed_g updateArgSt_Ed[] = {
            {((UI_MsgBoxArgs *) box->args)->X, ((UI_MsgBoxArgs *) box->args)->X + MSGBOX_BOUNCE_HEIGHT},
            {((UI_MsgBoxArgs *) box->args)->Y, ((UI_MsgBoxArgs *) box->args)->Y + MSGBOX_BOUNCE_HEIGHT},
            {((UI_MsgBoxArgs *) box->args)->Width, ((UI_MsgBoxArgs *) box->args)->Width - MSGBOX_BOUNCE_HEIGHT},
            {((UI_MsgBoxArgs *) box->args)->Height, ((UI_MsgBoxArgs *) box->args)->Height - MSGBOX_BOUNCE_HEIGHT}
        };

        UIAnimationFunc updateFuncs[] = {bounce, bounce, bounce, bounce};

        MsgBox_BounceAnime = Anime_Create(box,
                                          4,
                                          updateArgs,
                                          updateArgSt_Ed,
                                          updateFuncs,
                                          MSGBOX_BOUNCE_ANIME_DURATION,
                                          MsgBox_EmptyFunc,
                                          MsgBox_EmptyFunc);
        if (MsgBox_BounceAnime == NULL) {
            return 0;
        }
    }
    box->animation = MsgBox_BounceAnime;
    return 1;

}



UIComponent *MsgBox_Generate(const char *format, ...) {
    char text[128];
    va_list arg; //定义可变参数列表数据类型的变量arg
    va_start(arg, format); //从format开始，接收参数列表到arg变量
    vsprintf(text, format, arg); //使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);

    UIComponent *box = MsgBox_Create(text);
    if (!box) return 0;
    if (!MsgBox_LoadAppearAnime(box)) {
        MsgBox_Destroy(box);
        return 0;
    }

    UI_StartAnime(box);
    UI_RegisterComponent(box);
    return box;
}

uint8_t MsgBox_UpdateText(UIComponent *box, const char *format, ...) {
    char text[128];
    va_list arg; //定义可变参数列表数据类型的变量arg
    va_start(arg, format); //从format开始，接收参数列表到arg变量
    vsprintf(text, format, arg); //使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);

    if (!box) return 0;
    if (!rMsgBox_UpdateText(box, text)) return 0;
    if (!MsgBox_LoadBounceAnime(box)) return 0;
    UI_StartAnime(box);
}
