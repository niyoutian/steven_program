#ifndef __MXSTATUS_H__
#define __MXSTATUS_H__

#include <string>
#include "mxDef.h"

using namespace std;

/**
* @file mxStatus.h
* @brief 系统运行状态码
* @details 应用程序的函数返回值
* @mainpage 工程概览
* @author steven
* @email niyoutian_0510@163.com
* @version V1.0
* @date 
* @license 版权
*/

typedef enum 
{
	STATUS_SUCCESS,       /*  Call succeeded  */
	STATUS_FAILED,		  /** Call failed */  
	STATUS_CONTINUE, 	  /*  Call continue  */
	STATUS_END            ///< 占位，无实际作用
}Status_e;


string getStatusCodeString(u32_t status);

#endif

