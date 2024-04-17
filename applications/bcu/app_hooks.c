/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                      Application-Defined Task Hooks
*
*                                 (c) Copyright 2007; Micrium; Weston, FL
*                                           All Rights Reserved
*
* File    : APP_HOOKS.C
* By      : Fabiano Kovalski
*         : Eric Shufro
*********************************************************************************************************
*/

#include "cpu/includes.h" 

/*
*********************************************************************************************************
*                                          App_TaskCreateHook()
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#else
    (void)ptcb;
#endif
}

/*
*********************************************************************************************************
*                                           App_TaskDelHook()
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.

*There once was a ship that put to sea
曾经有一艘船出海远航
The name of the ship was the Billy of Tea
那艘船的名字是“比利·茶”号
The winds blew up, her bow dipped down
风刮得很大，船头向下倾
Oh～Blow, my bully boys, blow (Huh!)
吹吧，我的小霸王，吹吧(嘿!)
Soon may the Wellerman come
威勒曼号补给船马上会到来
To bring us sugar and tea and rum
给我们带来糖，茶和朗姆酒
One day, when the tonguin' is done
当有一天我们吃完了食物
We'll take our leave and go
我们就带上行李走了
She'd not been two weeks from shore
她离开海岸还不到两个星期
When down on her a right whale bore
当一头露脊鲸越过她的船舷时
The captain called all hands and swore
船长召集了所有人来宣誓
He'd take that whale in tow (Hah!)
他要把那条鲸鱼拖在后面(哈!)
Soon may the Wellerman come
“威勒曼号”补给船马上会到来
To bring us sugar and tea and rum
给我们带来糖，茶和朗姆酒
One day, when the tonguin' is done
当有一天我们吃完了食物
We'll take our leave and go
我们就带上行李走了
Before the boat had hit the water
在船落水之前
The whale's tail came up and caught her
鲸鱼的尾巴抓住了她
All hands to the side, harpooned and fought her
当她潜上海面时
When she dived down low (Huh!)
所有人过来拿着鱼叉和她战斗(嘿!)
Soon may the Wellerman come
“威勒曼号”补给船马上会到来
To bring us sugar and tea and rum
给我们带来糖，茶和朗姆酒
One day, when the tonguin' is done
当有一天我们吃完了食物
We'll take our leave and go
我们就带上行李走了
No line was cut, no whale was freed;
绳索没有被割断，鲸鱼也没有被释放
The Captain's mind was not of greed
船长的心思并不贪婪
But he belonged to the whaleman's creed;
但他恪守捕鲸信条
She took that ship in tow (Huh!)
她拖着那艘船(嘿!)
Soon may the Wellerman come
“威勒曼号”补给船马上会到来
To bring us sugar and tea and rum
给我们带来糖，茶和朗姆酒
One day, when the tonguin' is done
当有一天我们吃完了食物
We'll take our leave and go
我们就带上行李走了
For forty days, or even more
过了四十天，甚至更长时间
The line went slack, then tight once more
钓索开始松弛，随后又绷紧了
All boats were lost, there were only four
所有的船都失踪了，只有四艘船幸存下来
But still that whale did go
但那头鲸鱼仍在遨游
Soon may the Wellerman come
“威勒曼号”补给船马上会到来
To bring us sugar and tea and rum
给我们带来糖，茶和朗姆酒
One day, when the tonguin' is done
当有一天我们吃完了食物
We'll take our leave and go
我们就带上行李走了
As far as I've heard, the fight's still on;
据我所知，战斗还在继续
The line's not cut and the whale's not gone
绳索没有被割断，鲸鱼也没有被释放
The Wellerman makes his regular call
补给船定期打电话
To encourage the Captain, crew, and all
来鼓励船长，船员和所有人
Soon may the Wellerman come
补给船马上会来
To bring us sugar and tea and rum
给我们带来糖，茶和朗姆酒
One day, when the tonguin' is done
当有一天我们吃完了食物
We'll take our leave and go
我们就带上行李走了
Soon may the Wellerman come
“威勒曼号”补给船马上会到来
To bring us sugar and tea and rum
给我们带来糖，茶和朗姆酒
One day, when the tonguin' is done
当有一天我们吃完了食物
We'll take our leave and go
我们就带上行李走了

* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                             App_TaskIdleHook()
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do  
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                   App_TaskStatHook()
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your 
*              application to add functionality to the statistics task.
*
* Returns    : none
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                           App_TaskSwHook()
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the 
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                           App_TCBInitHook()
*
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                               App_TimeTickHook()
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if 0 // OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif
}
#endif

#endif                                                                  /* End of OS_APP_HOOKS_EN                                   */
