#ifndef __MXSTATUS_H__
#define __MXSTATUS_H__

#include <string>
#include "mxDef.h"

using namespace std;

/**
* @file mxStatus.h
* @brief ϵͳ����״̬��
* @details Ӧ�ó���ĺ�������ֵ
* @mainpage ���̸���
* @author steven
* @email niyoutian_0510@163.com
* @version V1.0
* @date 
* @license ��Ȩ
*/

typedef enum 
{
	STATUS_SUCCESS,           /**< Call succeeded */
	STATUS_FAILED,		      /**< Call failed */
	STATUS_CONTINUE, 	      /**< Call continue */
	STATUS_ERR_PARAMETER,     /**< parameter failed */
	STATUS_ERR_MALLOC,        /**< malloc failed */
	STATUS_WAKE_UP,           /**< wake up timer */
	STATUS_END                /**< not use */
}Status_e;


string getStatusCodeString(u32_t status);

#endif

