#ifndef UI_CORE_H
#define UI_CORE_H
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * @defgroup fin_action_group
 */
#define DESTROY_WHEN_FIN 1
#define KEEP_WHEN_FIN 0

#define async_func_def(name,type,arg) \
void _##name(void *arg); \
uint8_t __##name##_lock = 0;\
void name(type arg){ \
if(__##name##_lock == 1) return;\
_##name((void*)arg);\
}\
void _##name(void *___) {\
type arg = (type)___;

#define func_start(name) \
static int16_t state = 0;\
static UITask* t = 0;\
switch(state){\
case 0:\
t = UI_AddTask(0,_##name,___)

#define async_func_def_end(name,fin)\
t->finish = fin;\
__##name##_lock = 0;\
}}

#define func_wait_if(var)\
state = __LINE__;\
return;\
case __LINE__:\
if(var) return;

#define func_delay(time)\
state = __LINE__;\
t->process_time = GetTick() + time;\
return;\
case __LINE__:

#define func_gap()\
state = __LINE__;\
return;\
case __LINE__:


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
