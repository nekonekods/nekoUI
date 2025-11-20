
extern "C" {
#include "stm32f10x.h"
#include "OLED.h"
#include "Switch.h"

#include "Ticker.h"
#include "ui_core.h"
#include "toast.h"
#include "list.h"
#include "sticker.h"
#include "MsgBox.h"
#include "Sprite.h"

#include <stdlib.h>

void List_confirm_Func(UIComponent *list) {
    UI_ListArgs *a = (UI_ListArgs *) (list->args);
    String s = a->items[a->selected_index];
    Toast_Generate("select\n%s", s);
}

    extern const uint8_t bird[] = "p\210\10\14\12\222a\1\35\243AAZDx@\200\0\0\3\5\5\10\10\10\11\12\5\5\5\5\5\3\0";
    extern const uint8_t bird_mask[] = "\370\370\370\374\376\376\377\377\377\377\377\377\376\374\370\300\200\0\1\3\7\7\17\17\17\17\17\7\7\7\7\7\3\0";

    const uint8_t zhu[] ="\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\374\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\3\376\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1";

    const uint8_t ghost[] = "\370\6\3\1\361\1\1\361\1\3\6\374\1\3\6\374\0\0\300`0\20\34\17\0\0\0\0\1\3\3\6\0\0\0\0";
    const uint8_t ghost_mask[] = "\374\376\377\377\377\377\377\377\377\377\376\374\1\3\7\377\377\377\377\177?\37\37\17\0\0\0\1\1\3\3\6\0\0\0\0";

}


void process_anime(UIComponent *component);

int main(void) {
    SW_Init();
    UI_Init();
    uint32_t time = 0;
    int a = 0, b = 0, c = 0;

    Toast_Generate("启动成功！");
    while (1) {
        UI_LoopFunc();
        if(SW_Clicked(0)){
        }
        if(SW_Clicked(1)) {

        }
        time = 1000 / (GetTick() - time);
        OLED_ShowNum(0, 0, time, 3, OLED_8X16);
        time = GetTick();

        OLED_Update();
    }
}
