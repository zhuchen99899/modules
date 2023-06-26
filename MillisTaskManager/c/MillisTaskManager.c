/**
  ******************************************************************************
  * @file    MillisTaskManager.h
  * @author  zc
  * @version v1.0
  * @date    2023.6.26
  * @brief   超轻量级分时合作式任务调度器，可以替代旧的millis()轮询方案，不依赖ArduinoAPI
  * @Upgrade 2023.6.26 移植于MillisTaskManager ,可跨TI 28335 GD32 STM32 平台
  ******************************************************************************
  * @attention
  * 需要提供一个精确到毫秒级的系统时钟，然后周期调用Running函数
  ******************************************************************************
  */
#include "MillisTaskManager.h"

#ifndef NULL
#   define NULL 0
#endif


/**
  * @brief  初始化任务调度器
  * @param  priorityEnable:设定是否开启优先级
  * @retval TaskManager_t:任务管理器
  */

TaskManager_t ManagerCreate(TaskManager_t TaskManager,bool priorityEnable)
{
    TaskManager.PriorityEnable = priorityEnable;
    TaskManager.Head = NULL;
    TaskManager.Tail = NULL;

    return TaskManager;
}

/**
  * @brief  调度器析构，释放任务链表内存
  * @param  TaskManager:任务管理器
  * @retval void
  */
void ManagerDelete(TaskManager_t TaskManager)
{
    Task_t* now = TaskManager.Head;
    while(1)
    {
        /*当前节点是否为空*/
        if(now == NULL)
            break;

        /*将当前节点缓存，等待删除*/
        Task_t* now_del = now;

        /*移动到下一个节点*/
        now = now->Next;

        /*删除当前节点内存*/
        free(now_del);
    }
}
/**
  * @brief  往任务链表添加一个任务，设定间隔执行时间
  * @param  TaskManager:任务管理器
  * @param  func:任务函数指针
  * @param  timeMs:周期时间设定(毫秒)
  * @param  state:任务开关
  * @retval 任务节点地址
  */
Task_t* TaskRegister(TaskManager_t TaskManager,TaskFunction_t func, uint32_t timeMs, bool state)
{
 /*寻找当前函数*/
    Task_t* task = TaskFind(TaskManager,func);
    
    /*如果被注册*/
    if(task != NULL)
    {
        /*更新信息*/
        task->Time = timeMs;
        task->State = state;
        return task;
    }

    /*为新任务申请内存*/
    Task_t* task=(Task_t*)malloc(sizeof(Task_t));
 

    /*是否申请成功*/
    if(task == NULL)
    {
        return NULL;
    }

    task->Function = func;        //任务回调函数
    task->Time = timeMs;          //任务执行周期
    task->State = state;          //任务状态
    task->TimePrev = 0;           //上一次时间
    task->TimeCost = 0;           //时间开销
    task->TimeError = 0;          //误差时间
    task->Next = NULL;            //下一个节点
    
    /*如果任务链表为空*/
    if(TaskManager.Head == NULL)
    {
        /*将当前任务作为链表的头*/
        TaskManager.Head = task;
    }
    else
    {
        /*从任务链表尾部添加任务*/
        TaskManager.Tail->Next = task;
    }
    
    /*将当前任务作为链表的尾*/
    TaskManager.Tail = task;
    return task;

}

/**
  * @brief  寻找任务,返回任务节点
  * @param  TaskManager:任务管理器
  * @param  func:任务函数指针
  * @retval 任务节点地址
  */
Task_t* TaskFind(TaskManager_t TaskManager,TaskFunction_t func)
{
    Task_t* now = TaskManager.Head;
    Task_t* task = NULL;
    while(1)
    {
        if(now == NULL)//当前节点是否为空
            break;

        if(now->Function == func)//判断函数地址是否相等
        {
            task = now;
            break;
        }

        now = now->Next;//移动到下一个节点
    }
    return task;//返回任务节点地址
}


/**
  * @brief  获取当前节点的前一个节点
  * @param  TaskManager:任务管理器
  * @param  task:当前任务节点地址
  * @retval 前一个任务节点地址
  */
Task_t* GetPrev(TaskManager_t TaskManager,Task_t* task)
{
    Task_t* now = TaskManager.Head;    //当前节点
    Task_t* prev = NULL;   //前一节点
    Task_t* retval = NULL; //被返回的节点

    /*开始遍历链表*/
    while(1)
    {
        /*如果当前节点为空*/
        if(now == NULL)
        {
            /*遍历结束*/
            break;
        }
        
        /*如果当前节点与被搜索的节点匹配*/
        if(now == task)
        {
            /*保存前一个节点，准备返回*/
            retval = prev;
            break;
        }
        
        /*当前节点保存为前一节点*/
        prev = now;
        
        /*节点后移*/
        now = now->Next;
    }
    return retval;
}


/**
* @brief  注销任务（谨慎使用，线程不安全）
  * @param  func:任务函数指针
  * @retval true:成功 ; false:失败
  */
bool TaskLogout(TaskFunction_t func)
{

}

