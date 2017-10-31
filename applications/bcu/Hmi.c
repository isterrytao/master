#include "Uart.h"
#include "Async_Event.h"
#include "Hmi.h"


#if defined(UART_CHANNEL_HMI)

static uint8 icount = 0U;
static uint8 copyTxData(uint8 *buf, uint8 size) {
    const uint8 count = 20U;
    uint8 i;

    for (i = 0U; i < size && icount < count; i++) {
        *buf++ = icount++;
    }

    return i;
}
static Async_EvnetCbkReturnType cbk(Async_EventType *event, uint8 byWhat) {
    (void)event;
    (void)byWhat;

    icount = 0U;
    (void)Uart_SendAsync(UART_CHANNEL_HMI, copyTxData);
    return ASYNC_EVENT_CBK_RETURN_OK;
}

Std_ReturnType Hmi_Init(Async_LooperType *looper, uint32 baud) {
    static Async_EventType event;
    Std_ReturnType rc;

    rc =  Uart_Init(UART_CHANNEL_HMI, baud, NULL, NULL);
    if (rc == E_OK) {
        rc = Async_EventInit(&event, looper, cbk, 1000UL);
    }

    if (rc == E_OK) {
        rc = Async_EventRegisterToLooper(&event);
    }
    return rc;
}

#else

Std_ReturnType Hmi_Init(Async_LooperType *looper, uint32 baud) {
    (void)looper;
    (void)baud;
    return E_OK;
}

#endif
