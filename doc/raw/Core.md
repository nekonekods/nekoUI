# 核心介绍
> 此文档介绍NyaUI的大体框架。

## 快速启动
> [!NOTE]
> 如果要直接烧录这个代码，请按照以下接线：笔者使用的是stm32F103C8T6，OLED屏幕采用的是I2C协议的屏幕，SCL->A12，SDA->A11。\
> （当你打开keil时，你会发现缺失一大堆文件，没事，你把那些东西从keil文件列表里删掉就好，我这还要用就懒得删了。。）

在引入该项目中所有所需的库之后，在main函数中写下如下结构，即可启动整个系统。
```c
#include "ui_core.h"

int main(void) {
    UI_Init();
    while (1) {
        UI_LoopFunc();
        
            //你的代码...
            
        OLED_Update();
    }
}
```
我们能看到，主函数调用了**初始化函数**`UI_Init`和**主循环函数**`UI_LoopFunc`，这两个函数是整套系统的核心函数。\
为了显示帧率，我一般会将main函数写成：
```c
int main(void) {
    UI_Init();
    uint32_t time = 0;

    while (1) {
        UI_LoopFunc();
        
        // 你的代码...
        
        time = 1000 / (GetTick() - time);
        OLED_ShowNum(0, 0, time, 3, OLED_8X16);
        time = GetTick();
        OLED_Update();
    }
}
```
User中给出了供使用的main.c文件作为模板使用，keil也包含的是这个文件。至于那个main.cpp，那是我用来做乱七八糟测试的，感兴趣也可以看看，不做维护。
> [!WARNING]
> 当你打开keil时，你会发现缺失有一大堆文件，没事，你把那些东西从keil文件列表里删掉就好，我这还要用就懒得删了。。）

## UI组件
### UI组件构成
UI组件的定义如下：
```c
typedef struct UIComponent {
    UIKnot base;
    UIComponentState state;                       //（动画）状态
    
    void (*draw_func)(struct UIComponent*);       // 绘图函数
    void (*destroy_func)(struct UIComponent*);    // 销毁函数
    void* args;                                   // 组件控制参数
    UIAnimation* animation;                       // 动画对象
} UIComponent;
```
- 其中前两个参数不由用户控制，当用户调用组件操作函数时由内部维护。
- 绘图函数`draw_func`主要用于渲染组件，但还有一些用处，下文说。
- 销毁函数`destroy_func`用于销毁组件，在这个函数中需要销毁包含组件本身和内部**所有**动态申请的内存。
- 参数组`args`是组件最核心的部分，所有关于这个组件本身的数据将全部在这里保存。这里通常需要用户按照组件需求，设计一个参数结构体，并将这个结构体对象放在这个指针下。
- 动画`animation`是放置当前组件正在播放的动画的对象，关于`UIAnimation`结构与功能，下文详说。

### UI组件操作函数
 - `UI_RegisterComponent`组件注册函数，用于将组件放置在UI组件链表里，这意味着组件开始工作。
 - `UI_RemoveComponent`组件移除函数，用于将组件移除UI链表。这将同时调用`destroy_func`

## 动画
动画组件与UI组件是包含关系，一个组件在一个时刻只会有一个正在播放的动画，虽然这会带来一些限制，但仍具有十足的拓展性。

该系统参考了关键帧的思想，并向其中加入了非线性函数这一特性，使得开发者能轻易地控制动画中各项参数，
同时动画也会变得更加灵动。

### 动画组件构成
动画组件定义如下：
```c
typedef float (*UIAnimationFunc)(float);
typedef  int16_t st_ed_g[2];

typedef struct UIAnimation{
    uint8_t UpdateArgsNum;          //动画更新参数个数
    int16_t **UpdateArgs;           //动画更新参数列表
    int16_t (*UpdateArgSt_Ed)[2];   //动画更新参数起始和终止位置
    UIAnimationFunc* UpdateFuncs;   //动画更新曲线函数列表
    uint32_t duration;              // 动画总时长（ms）
    
    void (*StartFunc)(UIComponent*);        //开始钩子函数
    void (*FinishedFunc)(UIComponent*);     //结束钩子函数
    
    uint32_t start_tick;             // 动画开始时间（滴答数）
} UIAnimation;
```
- `UIAnimationFunc`是动画曲线函数，以一个范围为0\~1的参数来描述动画的时间进度，返回此时按照曲线，动画的行进进度。其他情况后续详说。
- `st_ed_g`为“start-end group”，描述某个参数在动画中的起始位置。是一个包含两个16位整数的数组。

- `UpdateArgsNum`是动画中需要更新的参数个数。请务必保证这个参数和下面所有提供的数组的长度都对应上
- `UpdateArgs`是需更新的参数对应的地址，是一个16位整数指针列表。
- `UpdateArgSt_Ed`是需更新的参数的始末值，是一个`st_ed_g`列表。
- `UpdateFuncs`是更新参数时使用的曲线函数，是一个`UIAnimationFunc`列表。
- `duration`是动画持续时间。

- `StartFunc`和`FinishedFunc`分别是动画开始播放和结束时调用的函数，不使用时请赋予一个**空函数**指针，而**不要赋0**

- `start_tick`是动画起始时间，无需用户手动管理。
### 动画操作函数
- `Anime_Create`用于创建动画示例，用户需按照参数列表提供各项参数，此函数调用时，用户直接传入字面量或局部变量即可，函数内部会为这些内容重新分配空间并复制内容。
- `Anime_Destroy`（或`cAnime_Destroy`）用于销毁动画对象。
- `UI_StartAnime`(ui_core.c)用于启动动画
### 动画处理周期
具体实现请参照`process_anime`(ui_core.c)函数，此处仅讲述大致流程。

1. 当用户调用`UI_StartAnime`时，组件进入等待状态(`UI_STATE_WAIT_FOR_ANIMATE`)
2. 当组件被轮询时，调用开始函数，并将状态改为正在动画(`UI_STATE_ANIMATING`)
3. 处理动画时，先通过时间计算出当前进度，再按照曲线计算出参数进度，再更新动画。
4. 当计算当前进度时，发现进度>1，即动画已经结束了时会将进度**置于1**，更新组件组件状态，并执行结束函数。
5. 若组件处于一般状态(`UI_STATE_NORMAL`)，那么也会调用渲染函数。

> 开始函数在渲染函数前调用，结束函数在渲染函数后调用。

### 动画曲线函数说明
动画曲线函数描绘了参数的变化路径，以此来实现各种灵动的效果。

函数接受一个0\~1的浮点数，描述当前动画进度，按照函数本身返回一个0\~1进度值。

这个函数事实上是一个数学函数`f(x)`，可以是连续的，也可以是离散的。但一定要保证f(x)在x等于0\~1之间都有定义。

例如，提供一个函数实现
```c
float line(float x){
    return x;
}
```
这个函数描述了一个线性运动，故对象将匀速运动。

当动画执行完毕后，该参数会停留在`f(1)`的位置上，如果担心有累计误差，请为`x == 1`单独添加一条分支。

特别的，如果`f(1) == 0`，则表明该动画是一个往返动画，在执行完毕动画后，参数将保持初始的值。
如果想实现往返动画，那么请留意`st_ed_g`参数，该动画的结束值则成为了一个动画幅度绑定的参数。即：
设`f(x)`的最大值为`k`，那么运动幅度就是k乘始末值之差。

`f(x)`的返回值可以超出0\~1的区间，但是建议不要超的太多，往返动画保持在±1.5的区间中为宜。单向动画建议0\~1.5。

### 动画的衔接
如果两个动画中间需要进行进行衔接，例如列表中不同选项间的切换，通常可以进行动画对象的复用。即，将使用过的动画对象进行保存，然后只修改部分参数，并在使用时将他挂载至对象上。\
在编写这类代码时，可以考虑这几个方面：
1. 在做动画衔接时，为了保持流畅性，我们可以将起始位置设置为组件当前位置，只着重考虑目标位置。
2. 目标位置的来源有很多，最简单直接的方式就是将目标位置设置在某个静态变量或对象属性中。
3. 目标位置有时还可以是上一个动画目标位置的相对位置，这是我们只需要在原参数的基础上进行部分修改就好了。
4. 设置完后，还需要调用一次`UI_StartAnime`函数，来重置动画系统里的各项参数。

## 事件系统
学过js的同学可能能熟悉一点，我是按照`promise`函数的用法来写的这个功能。你可以通过这个功能，在未来时间上部署任务。\
结构如下：
```c
typedef struct UITask{
    UIKnot base;
    void (*task_func)(void* args);
    uint32_t process_time;
    void* args;
    uint8_t finish;
} UITask;
```
结构很简单，包含需要执行的函数，执行时间(ms，绝对事件)，可能需要的参数，以及一个**finish参数**来控制在运行结束后是否删除该事件(后面细讲)。\
当当前时间大于等于设定时间时，对应任务就会被执行。
操作用的函数有两个，添加和删除：
```c
UITask* UI_AddTask(uint32_t time,void (*task)(void*),void*);
void UI_RemoveTask(UITask* t);
```
例如：
```c
UI_AddTask(GetTick()+3000,task_func,component)->finish = DESTROY_WHEN_FIN;
```
这句话向三秒后部署了运行`task_func`这一函数的任务，当该函数被调用时，将会把`component`作为参数传入，具体行为由函数本身决定。
`->finish = DESTROY_WHEN_FIN`在这里表示在任务运行之后将销毁这个task对象。\
其中这里可以填的值为`DESTROY_WHEN_FIN`或`KEEP_WHEN_FIN`，表示任务运行结束后要不要销毁task对象。\
这种写法可以实现两个操作之间的延时，用处类似`delay`函数，不过这种操作是异步的，他不会打断主循环。\
但是实际操作时时会发现，这种写法并不能随便的在函数中像`delay`函数那样随调随停，这种情况我们将会在后面讨论。

比如说，我想添加一个每一帧都要执行的任务怎么办呢？你可以按如下方案实现：
```c
UI_AddTask(0,task_func,component)->finish = KEEP_WHEN_FIN;
```
这个语句将任务部署在时间线的最开始，而这个任务将不会被删除，意味着每一次检查这个task时都会被执行。

由于`finish`参数随时可变，所以各种任务是否运行，何时结束，都是可控的。

在与设备通信的任务中，我们可能有如下需求：\
我们在一个控制流中我们可能需要等待从机将信号发过来，或者我们单纯想进行一个几秒的延时，如果因为这个我需要将函数拆碎成多个函数，这太蠢了（\
于是我们设计了以下宏库(位于ui_core.h)来解决这一问题：\
```c
#define asyn_func_def(name,type,arg)
#define func_start(name)
#define asyn_func_def_end(name,fin)

#define func_wait_if(var)
#define func_delay(time)
#define func_gap()
```
下面将介绍这个宏库的使用方法：\
（在看之前，你的函数需要是一个无返回值，并且只传入0个或1个的参数和函数，这是目前来讲的限制..）\
假定我们有一个同步的工作流函数如下：
```c
void demo(Object* obj){
    int a = 0;              //局部变量
    static times = 0;       //局部静态变量，例如记录这个函数被调用的次数
    action1();
    while(isready() == 0);  //等待某一个标志位准备就绪
    a = 1;                  //局部变量被赋值
    action2(obj);           //参数被使用
    delay(1000);            //等待一秒
    action3(a);             //局部变量被使用
}
```
1. 将函数首尾替换成`asyn_func_def(name,type,arg)`和`asyn_func_def_end(name,fin)`宏,\
其中`name`为函数名，`type`和`arg`分别为传入参数的类型和变量名称,`fin`表示结束行为，即前面提到的那两个宏。\
一般都是填`DESTROY_WHEN_FIN`，毕竟一个函数调用一次执行一次嘛。
```c
asyn_func_def(demo,Object*,obj){
    ...
}asyn_func_def_end(demo,DESTROY_WHEN_FIN)
```
2. 在函数第一行插入`func_start(name)`，这里的name也是函数名。然后将所有的局部变量的声明放在两行中间，并且**全部**声明成**静态变量**。\
初始化变量时，你需要在声明这一行完成对静态变量（指的是原函数中声明为静态的那些变量，如例子中的times）的初始化。\
其他变量的初始化需放在`func_start`语句后面：
```c
asyn_func_def(demo,Object*,obj){
    static a,time = 0;  //声明+静态变量初始化
    func_start(name);
    a = 0               //其他变量的初始化
    ...
```
3. 将函数中的阻塞操作替换为对应标签宏，如之前的while等待将被替换为`func_wait_if(exp)`，之前的`delay`函数将被替换为`func_delay(time)`。\
你代码段中可能还有其他耗时操作，你可以用`func_gap()`宏来将一大段代码分割成两段，这样子这两段代码就会被分配到两个帧的时间被执行（甚至多个帧）。
```c
...
    action1();
    func_wait_if(isready() == 0);
    a = 1;
    func_gap();      //可以单纯是分割一下。
    action2(obj);
    func_delay(1000);
    action3(a);
...
```
到此，这个函数就可以在异步框架中被顺滑调用了！在等待时，仍然会进行动画更新屏幕刷新等操作，也会检查可能存在的其他task任务。

完整如下：
```c
asyn_func_def(demo,Object*,obj){   //函数定义
    static a,time = 0;  //声明+静态变量初始化
    func_start(name);
    a = 0               //其他变量的初始化
    action1();
    func_wait_if(isready() == 0);
    a = 1;
    func_gap();         //可以单纯是分割一下。
    action2(obj);
    func_delay(1000);
    action3(a);
}asyn_func_def_end(demo,DESTROY_WHEN_FIN)   //函数末尾
```
这个方案其实漏洞还是蛮多的，但是能较大程度上解决一9部分问题。\
实际上多参数方案也不是没有，但是我觉得还是有些臃肿，等我想想有没有什么比较我能接受的方案，后面再进行改进。

> [!NOTE]
> 经过改造后，该函数还会具有以下特性： 在宏展开后，会存在线程锁机制，因此该函数不会在同一段时间被调用两次。这是专门设计的，防止多个线程操作同一组静态变量造成混乱。

记录一下遗留的问题：
1. 如果想实现一个函数被多个任务调用，应该怎么改造呢？
2. 如果我们想改造多参数还有有返回值的函数该怎么办呢？
