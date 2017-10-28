#include <m8c.h>
#include "PSoCAPI.h"

BYTE mouseData[2] = {0, 0};

void main (void)
{
	BYTE counter = 0;

	M8C_EnableGInt;
	
	SleepTimer_Start();
	SleepTimer_SetInterval(SleepTimer_64_HZ);
	SleepTimer_EnableInt();

	USBFS_Start(0, USB_5V_OPERATION);
	while(!USBFS_bGetConfiguration()) {
		// Wait for Device to enumerate
		// if enumeration does not happen within 10s, reset.	
		counter++;
		SleepTimer_SyncWait(8, SleepTimer_WAIT_RELOAD);
		if(counter == 80) M8C_Reset;
	}
	
	USBFS_LoadInEP(1, mouseData, 2, USB_NO_TOGGLE);
	
	while(1)
	{
		counter=0;	
		while(!USBFS_bGetEPAckState(1)) {
			// wait for ACK. if no ACK within 1s, reset.
			counter++;
			SleepTimer_SyncWait(1, SleepTimer_WAIT_RELOAD);
			if(counter == 64) M8C_Reset;
		}
		
		// 5s delay		
		SleepTimer_SyncWait(120, SleepTimer_WAIT_RELOAD);
		SleepTimer_SyncWait(200, SleepTimer_WAIT_RELOAD);
		
		if (mouseData[0] != 0xfd) mouseData[0] = 0xfd; // left
		else mouseData[0] = 0x03;	//right
		USBFS_LoadInEP(1, mouseData, 2, USB_TOGGLE);
	}
}		

