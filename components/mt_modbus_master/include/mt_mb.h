/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mb.h,v 1.17 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef MT_MB_H
#define MT_MB_H

#include "mt_mbconfig.h"
#include "mt_port.h"

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#include "mt_mbport.h"
#include "mt_mbproto.h"

/*! \defgroup modbus Modbus
 * \code #include "mb.h" \endcode
 *
 * This module defines the interface for the application. It contains
 * the basic functions and types required to use the Modbus protocol stack.
 * A typical application will want to call eMBInit() first. If the device
 * is ready to answer network requests it must then call eMBEnable() to activate
 * the protocol stack. In the main loop the function eMBPoll() must be called
 * periodically. The time interval between pooling depends on the configured
 * Modbus timeout. If an RTOS is available a separate task should be created
 * and the task should always call the function eMBPoll().
 *
 * \code
 * // Initialize protocol stack in RTU mode for a slave with address 10 = 0x0A
 * eMBInit( MB_RTU, 0x0A, 38400, MB_PAR_EVEN );
 * // Enable the Modbus Protocol Stack.
 * eMBEnable(  );
 * for( ;; )
 * {
 *     // Call the main polling loop of the Modbus protocol stack.
 *     eMBPoll(  );
 *     ...
 * }
 * \endcode
 */

/* ----------------------- Defines ------------------------------------------*/

/*! \ingroup modbus
 * \brief Use the default Modbus TCP port (502)
 */
#define MB_TCP_PORT_USE_DEFAULT 0

/* ----------------------- Type definitions ---------------------------------*/

/*! \ingroup modbus
 * \brief Modbus serial transmission modes (RTU/ASCII).
 *
 * Modbus serial supports two transmission modes. Either ASCII or RTU. RTU
 * is faster but has more hardware requirements and requires a network with
 * a low jitter. ASCII is slower and more reliable on slower links (E.g. modems)
 */
typedef enum
{
    MB_RTU,   /*!< RTU transmission mode. */
    MB_ASCII, /*!< ASCII transmission mode. */
    MB_TCP    /*!< TCP mode. */
} eMBMode;

/*! \ingroup modbus
 * \brief If register should be written or read.
 *
 * This value is passed to the callback functions which support either
 * reading or writing register values. Writing means that the application
 * registers should be updated and reading means that the modbus protocol
 * stack needs to know the current register values.
 *
 * \see eMBRegHoldingCB( ), eMBRegCoilsCB( ), eMBRegDiscreteCB( ) and
 *   eMBRegInputCB( ).
 */
typedef enum
{
    MB_REG_READ, /*!< Read register values and pass to protocol stack. */
    MB_REG_WRITE /*!< Update register values. */
} eMBRegisterMode;

/*! \ingroup modbus
 * \brief Errorcodes used by all function in the protocol stack.
 */
typedef enum
{
    MB_ENOERR,    /*!< no error. */
    MB_ENOREG,    /*!< illegal register address. */
    MB_EINVAL,    /*!< illegal argument. */
    MB_EPORTERR,  /*!< porting layer error. */
    MB_ENORES,    /*!< insufficient resources. */
    MB_EIO,       /*!< I/O error. */
    MB_EILLSTATE, /*!< protocol stack in illegal state. */
    MB_ETIMEDOUT  /*!< timeout error occurred. */
} eMBErrorCode;

/* if  MB_DEVICE_USE_TYPE == MB_DEVICE_MASTER  */

/* ----------------------- Type definitions ---------------------------------*/
/*! \ingroup modbus
 * \brief Errorcodes used by all function in the Master request.
 */
typedef enum
{
    MB_MRE_NO_ERR,      /*!< no error. */
    MB_MRE_NO_REG,      /*!< illegal register address. */
    MB_MRE_ILL_ARG,     /*!< illegal argument. */
    MB_MRE_REV_DATA,    /*!< receive data error. */
    MB_MRE_TIMEDOUT,    /*!< timeout error occurred. */
    MB_MRE_MASTER_BUSY, /*!< master is busy now. */
    MB_MRE_EXE_FUN      /*!< execute function error. */
} eMBMasterReqErrCode;
/*! \ingroup modbus
 *  \brief TimerMode is Master 3 kind of Timer modes.
 */
typedef enum
{
    MB_TMODE_T35,             /*!< Master receive frame T3.5 timeout. */
    MB_TMODE_RESPOND_TIMEOUT, /*!< Master wait respond for slave. */
    MB_TMODE_CONVERT_DELAY    /*!< Master sent broadcast ,then delay sometime.*/
} eMBMasterTimerMode;

/* ----------------------- Function prototypes ------------------------------*/
/*! \ingroup modbus
 * \brief Initialize the Modbus Master protocol stack.
 *
 * This functions initializes the ASCII or RTU module and calls the
 * init functions of the porting layer to prepare the hardware. Please
 * note that the receiver is still disabled and no Modbus frames are
 * processed until eMBMasterEnable( ) has been called.
 *
 * \param eMode If ASCII or RTU mode should be used.
 * \param ucPort The port to use. E.g. 1 for COM1 on windows. This value
 *   is platform dependent and some ports simply choose to ignore it.
 * \param ulBaudRate The baudrate. E.g. 19200. Supported baudrates depend
 *   on the porting layer.
 * \param eParity Parity used for serial transmission.
 *
 * \return If no error occurs the function returns eMBErrorCode::MB_ENOERR.
 *   The protocol is then in the disabled state and ready for activation
 *   by calling eMBMasterEnable( ). Otherwise one of the following error codes
 *   is returned:
 *    - eMBErrorCode::MB_EPORTERR IF the porting layer returned an error.
 */
eMBErrorCode eMBMasterInit(eMBMode eMode, UCHAR ucPort, ULONG ulBaudRate,
                           eMBParity eParity, int tx_pin, int rx_pin,
                           int en_pin);

/*! \ingroup modbus
 * \brief Release resources used by the protocol stack.
 *
 * This function disables the Modbus Master protocol stack and release all
 * hardware resources. It must only be called when the protocol stack
 * is disabled.
 *
 * \note Note all ports implement this function. A port which wants to
 *   get an callback must define the macro MB_PORT_HAS_CLOSE to 1.
 *
 * \return If the resources where released it return eMBErrorCode::MB_ENOERR.
 *   If the protocol stack is not in the disabled state it returns
 *   eMBErrorCode::MB_EILLSTATE.
 */
eMBErrorCode eMBMasterClose(void);

/*! \ingroup modbus
 * \brief Enable the Modbus Master protocol stack.
 *
 * This function enables processing of Modbus Master frames. Enabling the
 * protocol stack is only possible if it is in the disabled state.
 *
 * \return If the protocol stack is now in the state enabled it returns
 *   eMBErrorCode::MB_ENOERR. If it was not in the disabled state it
 *   return eMBErrorCode::MB_EILLSTATE.
 */
eMBErrorCode eMBMasterEnable(void);

/*! \ingroup modbus
 * \brief Disable the Modbus Master protocol stack.
 *
 * This function disables processing of Modbus frames.
 *
 * \return If the protocol stack has been disabled it returns
 *  eMBErrorCode::MB_ENOERR. If it was not in the enabled state it returns
 *  eMBErrorCode::MB_EILLSTATE.
 */
eMBErrorCode eMBMasterDisable(void);

/*! \ingroup modbus
 * \brief The main pooling loop of the Modbus Master protocol stack.
 *
 * This function must be called periodically. The timer interval required
 * is given by the application dependent Modbus slave timeout. Internally the
 * function calls xMBMasterPortEventGet() and waits for an event from the
 * receiver or transmitter state machines.
 *
 * \return If the protocol stack is not in the enabled state the function
 *   returns eMBErrorCode::MB_EILLSTATE. Otherwise it returns
 *   eMBErrorCode::MB_ENOERR.
 */
eMBErrorCode eMBMasterPoll(void);

// cmd callback
eMBErrorCode eMBMasterCB01(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen);
eMBErrorCode eMBMasterCB02(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen);
eMBErrorCode eMBMasterCB03(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen);
eMBErrorCode eMBMasterCB04(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen);
eMBErrorCode eMBMasterCB05(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen);
eMBErrorCode eMBMasterCB06(UCHAR *recvBuf, UCHAR recvCmd, USHORT recvLen);

/*! \ingroup modbus
 *\brief These Modbus functions are called for user when Modbus run in Master
 *Mode.
 */

// cmd func
eMBException eMBMasterFunc01(UCHAR *recvFrame, USHORT *recvBit);
eMBException eMBMasterFunc02(UCHAR *recvFrame, USHORT *recvBit);
eMBException eMBMasterFunc03(UCHAR *recvFrame, USHORT *recvBit);
eMBException eMBMasterFunc04(UCHAR *recvFrame, USHORT *recvBit);
eMBException eMBMasterFunc05(UCHAR *recvFrame, USHORT *recvBit);
eMBException eMBMasterFunc06(UCHAR *recvFrame, USHORT *recvBit);

/*�� \ingroup modbus
 *\brief These functions are interface for Modbus Master
 */
void vMBMasterGetPDUSndBuf(UCHAR **pucFrame);
UCHAR ucMBMasterGetDestAddress(void);
void vMBMasterSetDestAddress(UCHAR Address);
USHORT usMBMasterGetPDUSndLength(void);
void vMBMasterSetPDUSndLength(USHORT SendPDULength);
void vMBMasterSetCurTimerMode(eMBMasterTimerMode eMBTimerMode);
BOOL xMBMasterRequestIsBroadcast(void);
eMBMasterResEventType eMBMasterGetErrorType(void);
void vMBMasterSetErrorType(eMBMasterResEventType errorType);
eMBMasterReqErrCode eMBMasterWaitRequestFinish(void);

/* ----------------------- Callback -----------------------------------------*/

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
