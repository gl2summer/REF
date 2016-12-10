#ifndef _GOS_FREERTOS_H
#define _GOS_FREERTOS_H

#if GFX_USE_OS_RAWRTOS


#include "raw_api.h"
#include <stdint.h>

#define TIME_IMMEDIATE		(RAW_NO_WAIT)
#define TIME_INFINITE		(RAW_WAIT_FOREVER)
typedef int8_t				bool_t;
typedef uint32_t			delaytime_t;
typedef RAW_TICK_TYPE		systemticks_t;
typedef int32_t				semcount_t;
typedef uint32_t				threadreturn_t;
typedef RAW_U8				threadpriority_t;
typedef uint32_t			size_t;

#define MAX_SEMAPHORE_COUNT	RAW_SEMAPHORE_COUNT
#define LOW_PRIORITY		CONFIG_RAW_PRIO_MAX-2
#define NORMAL_PRIORITY		CONFIG_RAW_PRIO_MAX/2
#define HIGH_PRIORITY		1

typedef RAW_SEMAPHORE gfxSem;
typedef RAW_MUTEX gfxMutex;
typedef RAW_TASK_OBJ* gfxThreadHandle;

#define DECLARE_THREAD_FUNCTION(fnName, param)	threadreturn_t fnName(void *param)
#define DECLARE_THREAD_STACK(name, sz)			PORT_STACK name[sz];

#define gfxHalt(msg)					{}
#define gfxExit()						{}
#define gfxAlloc(sz)					raw_malloc(sz)
#define gfxRealloc(p,osz,nsz)			raw_calloc(p, nsz)
#define gfxFree(ptr)					raw_free(ptr)
//#define gfxSleepMilliseconds(ms)		raw_sleep(ms*RAW_TICKS_PER_SECOND/1000)
//#define gfxSleepMicroseconds(ms)		(0)
#define gfxYield()						raw_sleep(0)
#define gfxSystemTicks()				raw_system_time_get()
#define gfxMillisecondsToTicks(ms)		((ms*RAW_TICKS_PER_SECOND/1000))
#define gfxSystemLock()					{}
#define gfxSystemUnlock()				{}
#define gfxMutexInit(pmutex)			raw_mutex_create(pmutex, (RAW_U8 *)"", RAW_MUTEX_INHERIT_POLICY, 3)
#define gfxMutexDestroy(pmutex)			raw_mutex_delete(pmutex)
#define gfxMutexEnter(pmutex) 				raw_mutex_get(pmutex, TIME_INFINITE)
#define gfxMutexExit(pmutex)				raw_mutex_put(pmutex)
#define gfxSemInit(psem, val, limit)	raw_semaphore_create(psem, "", val)//*(psem) = CreateSemaphore(0, val, limit, 0)
#define gfxSemDestroy(psem)				raw_semaphore_delete(psem)
//#define gfxSemWait(psem,wait_option)	(raw_semaphore_get((psem), wait_option)==RAW_SUCCESS)
//#define gfxSemWaitI(psem)				(raw_semaphore_get((psem), TIME_IMMEDIATE)==RAW_SUCCESS)
#define gfxSemSignal(psem)				raw_semaphore_put((psem))
#define gfxSemSignalI(psem)				raw_semaphore_put_all((psem))
#define gfxSemCounterI(psem)			((psem)->count)
#define gfxThreadMe()					{(unsigned int)raw_task_identify()}
#define gfxThreadClose(thread)			{}


extern RAW_VOID *raw_malloc(RAW_U32 size);
extern RAW_VOID raw_free(void *ptr);
extern RAW_VOID *raw_calloc(RAW_U32 nmemb, RAW_U32 size);
	
extern RAW_U16 raw_sleep(RAW_TICK_TYPE dly) ;
extern RAW_TICK_TYPE  raw_system_time_get(void);

extern RAW_U16 raw_mutex_create(RAW_MUTEX *mutex_ptr, RAW_U8 *name_ptr, RAW_U8 policy, RAW_U8 ceiling_prio);
extern RAW_U16 raw_mutex_delete(RAW_MUTEX *mutex_ptr);
extern RAW_U16 raw_mutex_get(RAW_MUTEX *mutex_ptr, RAW_TICK_TYPE wait_option);
extern RAW_U16 raw_mutex_put(RAW_MUTEX *mutex_ptr);
extern RAW_U16 raw_semaphore_create(RAW_SEMAPHORE *semaphore_ptr, RAW_U8 *name_ptr, RAW_U32 initial_count);
extern RAW_U16 raw_semaphore_delete(RAW_SEMAPHORE *semaphore_ptr);
extern RAW_U16 raw_semaphore_get(RAW_SEMAPHORE *semaphore_ptr, RAW_TICK_TYPE wait_option);
extern RAW_U16 raw_semaphore_put(RAW_SEMAPHORE *semaphore_ptr);

void gfxSleepMilliseconds(delaytime_t ms);
void gfxSleepMicroseconds(delaytime_t us);
bool_t gfxSemWait(gfxSem* psem, delaytime_t ms);
bool_t gfxSemWaitI(gfxSem* psem);
gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, threadpriority_t prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param);

#endif

#endif

