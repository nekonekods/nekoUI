#include "stm32f10x.h"
#include "OLED.h"
#include "Ticker.h"
#include "ui_core.h"
#include "toast.h"

int main(void) {
    UI_Init();
    uint32_t time = 0;

    Toast_Generate("启动成功！");
    while (1) {
        UI_LoopFunc();

        // Code here...

        time = 1000 / (GetTick() - time);
        OLED_ShowNum(0, 0, time, 3, OLED_8X16);
        time = GetTick();

        OLED_Update();
    }
}
