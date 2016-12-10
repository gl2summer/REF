#ifndef RAW_CPU_H
#define RAW_CPU_H


#define  NEED_STATUS   1

typedef  unsigned int        CPU_SR;                   

CPU_SR 	OS_CPU_SR_Save(void);
void	OS_CPU_SR_Restore(CPU_SR sr);
                                                              
#ifdef   NEED_STATUS
#define  RAW_SR_ALLOC()             CPU_SR  cpu_sr = (CPU_SR)0
#else
#define  RAW_SR_ALLOC() 
#endif

														
#define  USER_CPU_INT_DISABLE()        {cpu_sr = OS_CPU_SR_Save();}
#define  USER_CPU_INT_ENABLE()         {OS_CPU_SR_Restore(cpu_sr);}


#endif




