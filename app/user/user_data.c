/*
 * user_data.c
 *
 *  Created on: Dec 25, 2016
 *      Author: zulolo
 */

#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "smart_socket_global.h"

xSemaphoreHandle xSmartSocketEventListSemaphore;
SmartSocketEventList_t tSmartSocketEventList;

int32_t DAT_nAddEventHistory(uint64 unTime, EventType_t tEventType, uint64_t unData)	//void* pData, uint8_t unDataLen)
{
    if(xSemaphoreTake(xSmartSocketEventListSemaphore, (portTickType)10) == pdTRUE ){
    	tSmartSocketEventList.tEvent[tSmartSocketEventList.unEventNum % EVENT_HISTORY_MAX_RECORD_NUM].unTime = unTime;
    	tSmartSocketEventList.tEvent[tSmartSocketEventList.unEventNum % EVENT_HISTORY_MAX_RECORD_NUM].tEventType = tEventType;
    	tSmartSocketEventList.tEvent[tSmartSocketEventList.unEventNum % EVENT_HISTORY_MAX_RECORD_NUM].data = unData;
    	//memcpy(&(tSmartSocketEventList.tEvent[tSmartSocketEventList.unEventNum % EVENT_HISTORY_MAX_RECORD_NUM].data), pData, unDataLen);
    	tSmartSocketEventList.unEventNum++;

		system_param_save_with_protect(GET_USER_DATA_SECTORE(USER_DATA_EVENT_HISTORY),
				&tSmartSocketEventList, sizeof(tSmartSocketEventList));

        xSemaphoreGive(xSmartSocketEventListSemaphore);
    }else{
		printf("Take event list semaphore failed.\n");
		return (-1);
    }
	return 0;
}

SmartSocketEvent_t DAT_nGetEventHistory(uint8_t unEventSelecter)	// unEventSelecter =0 means latest, =1 means second last
{
	SmartSocketEvent_t tEvent;
	uint8_t unEventIndex;

	tEvent.tEventType = SMART_SOCKET_EVENT_INVALID;

	if ((unEventIndex >= EVENT_HISTORY_MAX_RECORD_NUM) ||
			(tSmartSocketEventList.unEventNum < (unEventIndex + 1))){
		return tEvent;
	}
	unEventIndex = (tSmartSocketEventList.unEventNum - 1) % EVENT_HISTORY_MAX_RECORD_NUM;
    if(xSemaphoreTake(xSmartSocketEventListSemaphore, (portTickType)10) == pdTRUE ){
    	tEvent = tSmartSocketEventList.tEvent[(unEventIndex >= unEventSelecter)?
    			(unEventIndex - unEventSelecter) : (EVENT_HISTORY_MAX_RECORD_NUM - (unEventSelecter - unEventIndex))];
        xSemaphoreGive(xSmartSocketEventListSemaphore);
        return tEvent;
    }else{
		printf("Take event list semaphore failed.\n");
		return tEvent;
    }
	return tEvent;
}
