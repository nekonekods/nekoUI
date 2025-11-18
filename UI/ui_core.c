#include <stdbool.h>
#include <stdlib.h>
#include "config.h"
#include "OLED.h"
#include "Ticker.h"
#include "anime.h"
#include "ui_core.h"


UIKnot* UI_ComponentKnot;
UIKnot* UI_TaskKnot;

// 检查是否有可见组件
static bool UI_HasVisibleComponents(void) {
//    UIComponent* current = g_ui_manager.head;
//    while (current != NULL) {
//        current = current->next;
//    }
    return false;
}

// 初始化UI管理器
void UI_Init() {
    OLED_Init();
    Ticker_Init();
    UI_ComponentKnot = Knot_Create();
    UI_TaskKnot = Knot_Create();
}

// 注册组件
void UI_RegisterComponent(UIComponent* component) {
#define CP ((UIKnot*) component)
    if (CP == NULL) return;
    UI_ComponentKnot->prev->next = CP; //O->O  K
    CP->prev = UI_ComponentKnot->prev; //O<-O  K
    CP->next = UI_ComponentKnot;       //O  O->K
    UI_ComponentKnot->prev = CP;       //O  O<-K
#undef CP
}
// 移除组件
void UI_RemoveComponent(UIComponent* component) {
#define CP ((UIKnot*) component)
    if (component == NULL) return;
    CP->prev->next = CP->next;
    CP->next->prev = CP->prev;
    component->destroy_func(component);
#undef CP
}


UITask* Task_Create(uint32_t time, void (*task)(void*),UIComponent* args){
    UITask* t = (UITask*) malloc(sizeof(UITask));
    if(!t){
//        free(t);
        return NULL;
    }else{
        t->process_time = time;
        t->task_func = task;
        t->args = args;
        t->finish = 0;
        return t;
    }
}
void Task_Destroy(UITask* t){
    free(t);
}

UITask* UI_AddTask(uint32_t time,void (*task)(void*),void* args){
    UITask* task_Obj = Task_Create(time, task, args);
    if (task_Obj == NULL) return NULL;
#define TSK ((UIKnot*) task_Obj)
    UI_TaskKnot->prev->next = TSK; //O->O  K
    TSK->prev = UI_TaskKnot->prev; //O<-O  K
    TSK->next = UI_TaskKnot;       //O  O->K
    UI_TaskKnot->prev = TSK;       //O  O<-K
#undef TSK
    return task_Obj;
}

void UI_RemoveTask(UITask* task){
#define TSK ((UIKnot*) task)
    if (TSK == NULL) return;
    TSK->prev->next = TSK->next;
    TSK->next->prev = TSK->prev;
    Task_Destroy(task);
}

void process_anime(UIComponent *component) {
    if (component->state == UI_STATE_WAIT_FOR_ANIMATE){
        if (component->animation == NULL) {  //意外爆炸
            component->state = UI_STATE_NORMAL;
            return;
        }
        component->state = UI_STATE_ANIMATING;
        component->animation->StartFunc(component);
    }
    if (component->state == UI_STATE_ANIMATING) {
        if (component->animation == NULL) {  //意外爆炸
            component->state = UI_STATE_NORMAL;
            return;
        }
        UIAnimation* anim = component->animation;
        float linear_progress = (float)(GetTick()-anim->start_tick)/anim->duration;
        for (int i = 0; i < anim->UpdateArgsNum; i++) {  // 遍历所有要更新的参数

            if (linear_progress >= 1) {  //更新主参数，如果要结束了，就归到1，并更新状态
                linear_progress = 1.0f;
                component->state = UI_STATE_NORMAL;
            }

            // 获取起始值和终止值
            int16_t start = anim->UpdateArgSt_Ed[i][0];
            int16_t end = anim->UpdateArgSt_Ed[i][1];
            // 使用更新曲线函数计算进度
            // 更新参数
            if(end-start == 0) continue;
            *(anim->UpdateArgs[i]) = start + (end - start) * anim->UpdateFuncs[i](linear_progress);
        }
        component->draw_func(component);

        if (component->state == UI_STATE_NORMAL){ //如果状态被更新，则说明动画运行结束了，因此执行最终函数
            anim->FinishedFunc(component);
        }
    }else{
        component->draw_func(component);
    }
}

void process_task(UITask* task){
    uint32_t time = GetTick();
    if(time >= task->process_time){
        task->task_func(task->args);
        if(task->finish){  //执行完了之后再说要不要删的事
            UI_RemoveTask(task);
        }
    }
}

UIKnot* Knot_Create(void) {
    UIKnot* knot = malloc(sizeof(UIKnot));
    if (!knot) {
        return NULL;
    }else{
        knot->prev = knot;
        knot->next = knot;
        return knot;
    }
}




void UI_LoopFunc(void) {
    OLED_Clear();
    UIKnot* current = UI_ComponentKnot;
    UIKnot* next;
    UIComponent* component;
    while ((next = current->next) != UI_ComponentKnot) {
        component = (UIComponent*)next;
        current = next;
        process_anime(component);
    }

    current = UI_TaskKnot;
    UITask* tasks;
    while ((next = current->next) != UI_TaskKnot) {
        tasks = (UITask*)next;
        current = next;
        process_task(tasks);
    }
}

void UI_StartAnime(UIComponent *uic) {
    if (!uic->animation) return;
    uic->state = UI_STATE_WAIT_FOR_ANIMATE;
    uic->animation->start_tick = GetTick();
}







