#ifndef UI_CORE_H
#define UI_CORE_H
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct UIAnimation UIAnimation;
typedef float (*UIAnimationFunc)(float);


// 组件状态
typedef enum {
    UI_STATE_NORMAL,
    UI_STATE_WAIT_FOR_ANIMATE,
    UI_STATE_ANIMATING,
} UIComponentState;


typedef struct UIKnot{
    struct UIKnot* prev;
    struct UIKnot* next;
} UIKnot;


typedef struct UIComponent {
    UIKnot base;
    UIComponentState state;
    void (*draw_func)(struct UIComponent*);       // 绘图函数
    void (*destroy_func)(struct UIComponent*);    // 销毁函数
    void* args;
    UIAnimation* animation;
} UIComponent;

typedef struct UITask{
    UIKnot base;
    void (*task_func)(void* args);
    uint32_t process_time;
    void* args;
    uint8_t finish;
} UITask;



// UI管理器API
extern void UI_Init();
extern void UI_RegisterComponent(UIComponent* component);
extern void UI_RemoveComponent(UIComponent* component);
extern void UI_StartComponentAnimation(UIComponent* component,
                                       UIAnimationFunc anim_func,
                                      uint16_t duration, 
                                      bool is_in_animation);
extern void UI_LoopFunc(void);

void process_anime(UIComponent *component);
void process_task(UITask* task);
void process_key(UIComponent *pComponent, uint8_t *KeyList, int8_t KeyCount);

extern bool UI_HandleKey(uint8_t key_code);

void UI_RegisterComponent(UIComponent* component);
void UI_RemoveComponent(UIComponent* component);

UIKnot* Knot_Create(void);

UITask* UI_AddTask(uint32_t time,void (*task)(void*),void*);
void UI_RemoveTask(UITask* t);

void UI_StartAnime(UIComponent *uic);

void UIComponent_Destroy(UIComponent* component);


#endif // UI_CORE_H
