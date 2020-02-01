/* ----------------------- System includes ----------------------------------*/
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <stdlib.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mt_mb.h"
#include "mt_mbport.h"

/* ----------------------- Modbus includes ----------------------------------*/
//#include <intrinsics.h>

/* ----------------------- Variables ----------------------------------------*/
static BOOL bIsWithinException = FALSE;
static portMUX_TYPE mb_mux = portMUX_INITIALIZER_UNLOCKED;

/* ----------------------- Start implementation -----------------------------*/

void vMBPortSetWithinException(BOOL bInException) {
  bIsWithinException = bInException;
}

BOOL bMBPortIsWithinException(void) { return bIsWithinException; }

void vMBPortEnterCritical(void) { portENTER_CRITICAL(&mb_mux); }

void vMBPortExitCritical(void) { portEXIT_CRITICAL(&mb_mux); }

void vMBPortClose(void) {
  extern void vMBPortSerialClose(void);
  extern void vMBPortTimerClose(void);
  extern void vMBPortEventClose(void);
  vMBPortSerialClose();
  vMBPortTimerClose();
  vMBPortEventClose();
}
