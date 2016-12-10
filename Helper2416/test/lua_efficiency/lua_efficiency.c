/**
 *******************************************************************************
 * @file       Lua_Efficiency.c 
 * @author     camel.shoko
 * @version    v1.1
 * @date       2013/9/27
 * @brief      Lua效率测试文件
 * @copyright  camel.shoko@gmail.com
 *******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "uart.h"
#include "timer4.h"

#include <math.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include <raw_api.h>
#include <rsh.h>

#include "lua_efficiency.h"

/* Local Variables -----------------------------------------------------------*/
const char LUA_SCRIPT[] = {
	"function loop_add(a, b,t)          "
	"   local sum = 0                   "
	"   for i = 1, t do          "
	"       sum = sum + a + b           "
	"   end                             "
	"   return sum                      "
	"end                                "
	"                                   "
};


/* Global Functions ----------------------------------------------------------*/

/**
 * @brief  c_loop_add
 * @note   C语言循环加
 * @param  none
 * @retval none
 */
uint32_t c_loop_add(int a,int b,int t)
{
	uint32_t result=0,i=0;
	
	for (i=0;i<t;i++) {
		result = result + a + b;
	}
	return result;
}

/**
 * @brief  lua_loop_add
 * @note   Lua循环加
 * @param  none
 * @retval none
 */
int lua_loop_add(lua_State *L, const char *func_name, int x, int y, int t) 
{
	int sum;
	
	/*装载脚本*/
	lua_getglobal(L, func_name);
	/* 第一个参数 */
	lua_pushnumber(L, x);
	/* 第二个参数 */
	lua_pushnumber(L, y);
	/* 第三个参数 */
	lua_pushnumber(L, t);
	
	/* 调用函数,告知有三个参数,一个返回值 */
	lua_call(L, 3, 1);
	/* 得到结果 */
	sum = (int) lua_tointeger(L, -1);
	
	lua_pop(L, 1);
	return sum;
}


/**
 * @brief  Lua_Efficiency_Test
 * @note   Lua效率测试
 * @param  none
 * @retval none
 */
void Lua_Efficiency_Test(void)
{
	#define US_PERCENT				15
	
	int sum, i;
	uint32_t duration = 0;
	
	DEBUG0("Start Lua..");
	lua_State *L = luaL_newstate();
	
	if (L == NULL) {
		DEBUG0("Cannot Create State: Not Enough Memory\r\n");
	}
	DEBUG0(">123");
	//64kb内存紧张,用luaL_openlibs加载所有模块,会自动退出
	luaopen_base(L); 
	DEBUG0(">>124");
	
	luaL_dostring(L, LUA_SCRIPT);
	DEBUG0("LUA Module Init Success\r\n");
	TIMER4_Init();
	
	for (i = 2000; i < 20000; i = i + 2000) { 
		
		//计算lua循环加运算的耗时
		TIMER4_Start();
		sum = lua_loop_add(L, "loop_add", 1, 1, i);
		duration = TCNTB4_CFG - TIMER4_Stop();
		DEBUG2("--Lua_Cnt %06d   Lua_Sum %06d", i, sum);
		DEBUG1(">>Frequency Duration(us): %06d\r\n", (duration*US_PERCENT));
		
		//计算c循环加运算的耗时
		TIMER4_Start();
		sum = c_loop_add(1, 1, i);
		duration = TCNTB4_CFG - TIMER4_Stop();
		DEBUG2("--C_Cnt   %06d   C_Sum %06d", i, sum);
		DEBUG1(">>Frequency Duration(us): %04d\r\n", (duration*US_PERCENT));
		
	}
	//关闭虚拟机
	lua_close(L);
}


/*----------------------------------------------------------------------------*/
RAW_S32 cmd_lua_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

xCommandLineInputListItem lua_shell_item ;

xCommandLineInput lua_shell_cmd = 
{
	"lua",
	"lua -- < no arg >\n",
	cmd_lua_test,
	0
};


RAW_S32 cmd_lua_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
	RAW_SR_ALLOC();
	
	RAW_CRITICAL_ENTER();
	Lua_Efficiency_Test();
	RAW_CRITICAL_EXIT();
	
	return 1;
}



/****************** (C) COPYRIGHT 2013 Camle.shoko ***********END OF FILE******/
