#ifndef GB32960_3_H
#define GB32960_3_H

#include "Std_Types.h"
#include "Async_Looper.h"
#include "ComStack_Types.h"

#if defined(__MINGW32__)
#define GB32960_PACKED  __attribute__ ((packed))
#elif defined(__HC12__)
#define GB32960_PACKED
#endif

typedef struct {
    uint8 start[2];
    uint8 cmdFlag;
    uint8 ackFlag;
    uint8 vin[17];
    uint8 encrypt;
    uint16 dataLength;
} GB32960_PACKED GB32960_PacketHeaderType;

typedef struct {
    uint8 start[2];
    uint16 length;
} GB32960_PACKED GB32960_TpExtendHeaderType;

#define GB32960_CMDFLAG_LOGIN           0x01U
#define GB32960_CMDFLAG_RT_MSG          0x02U
#define GB32960_CMDFLAG_CACHED_MSG      0x03U
#define GB32960_CMDFLAG_LOGOUT          0x04U

#define GB32960_ACKFLAG_OK              0x01U
#define GB32960_ACKFLAG_ERR             0x02U
#define GB32960_ACKFLAG_VIN_DUPLICATED  0x03U
#define GB32960_ACKFLAG_CMD             0xFEU

#define NOT_FOTA_COMMAND            0U
#define NORMAL_FOTA_COMMAND         1U
#define FORCE_FOTA_COMMAND          2U

#define GB32960_REMOTE_DISABLE_VALUE    0x56ABU
#define GB32960_RESET_MODE_FORCE_CMD    0x55U
#define GB32960_RESET_MODE_CMD          0x1U
#define GB32960_RESET_MODE_CANCEL_CMD   0U

#define GB32960_LOCK_CAR_CMD_NONE        0U
#define GB32960_LOCK_CAR_CMD_LEVEL1      1U
#define GB32960_LOCK_CAR_CMD_LEVEL2      2U
#define GB32960_LOCK_CAR_CMD_LEVEL3      3U


typedef struct {
    uint16 serverPort;
    const char *serverAddr;
    PduIdType tpTxPduId;
    PduIdType tpRxPduId;
    void (*notifyOtaRequest)(void);
    void (*notifyDisconnected)(void);
    void (*notifyConnected)(void);
    void (*getCCID)(char *ccid, uint8 len);
    Std_ReturnType (*tcpConnect)(const char *host, uint16 port, void (*ack)(Std_ReturnType Result));
    Std_ReturnType (*tcpTransmit)(uint16 length, uint16 (*CopyData)(uint8 *buf, uint16 len), void (*TxConfirm)(NotifResultType Result));
    Std_ReturnType (*tcpDisconnect)(void (*ack)(Std_ReturnType Result));
    uint32 rtMessageInterval;
    uint32 rtMessageAlarmIntervals[4];
} GB32960_ConfigType;


typedef struct {
    uint8 year;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;
}GB32960_PACKED  GB32960_DateTimeType;


typedef struct {
    GB32960_DateTimeType datatime;
    uint16 loginSN;
    uint8 ccid[20];
    uint8 chargeableSystemNum;
    uint8 chargeableSystemSNLength;
    uint8 chargeableSystemSN[10];
} GB32960_PACKED GB32960_LoginDataType;

typedef struct {
    GB32960_DateTimeType datatime;
} GB32960_AckMessageType;

typedef struct {
    GB32960_DateTimeType datatime;
} GB32960_PACKED GB32960_RtMsgHeaderType;

typedef struct {
    GB32960_RtMsgHeaderType rtMsgHeader;
    uint8 runMode;
} GB32960_PACKED GB32960_RtRunModeMsgType;

typedef struct {
    GB32960_DateTimeType datatime;
    uint8 num;
    uint8 parameterBegin;
} GB32960_PACKED GB32960_ParamSetHeaderType;

typedef struct {
    GB32960_DateTimeType datatime;
    uint8 cmdID;
} GB32960_PACKED GB32960_CommandHeaderType;


#define GB32960_SUPPORT_COMMAND_NUMBER 9U
extern const uint8 GB32960_SupportCommandId[GB32960_SUPPORT_COMMAND_NUMBER];

typedef uint16 (*GB32960_CopyDataBody)(uint8 *buf, uint16 len);
typedef void (*GB32960_NotifyRtMessageReportConfirmation)(boolean succeed);

#define GB32960_CONNECTION_CONNECT    0
#define GB32960_CONNECTION_DISCONNECT 1

#if CPU_BYTE_ORDER == HIGH_BYTE_FIRST
#define GB32960_HostToNet16(a) (a)
#define GB32960_NetToHost16(a) (a)
#else
#define GB32960_HostToNet16(a) ((((a)&0xff00)>>8) | (((a) & 0x00ff)<<8))
#define GB32960_NetToHost16(a) (GB32960_HostToNet16(a))
#endif

void GB32960_Init(Async_LooperType *looper, boolean isRmoteDcm);
void GB32960_TcpConnectAck(Std_ReturnType result);
void GB32960_TcpDisconnectAck(Std_ReturnType result);
void GB32960_StopRtMessage(void);
void GB32960_TaskStop(void);
void GB32960_TaskRecover(void);

BufReq_ReturnType GB32960_StartOfReception(PduLengthType length);
BufReq_ReturnType GB32960_CopyRxData(const uint8 *dat, PduLengthType length);
void GB32960_RxIndication(NotifResultType Result);
void GB32960_ConnectionStateChange(uint8 state);
Std_ReturnType GB32960_ReportRtMsg(uint16 length, GB32960_CopyDataBody copyTxMsgBody, GB32960_NotifyRtMessageReportConfirmation confirmation);


Std_ReturnType DtuTp_Transmit(PduIdType CanTpTxSduId, const PduInfoType *TpTxInfoPtr);

extern const GB32960_ConfigType GB32960ConfigData;
boolean GB32960_BalanceRemoteIsConnect(void);
const uint8 *GB32960_GetBalanceRemote(void);
uint16 GB32960_GetBalnceRemoteLength(void);
boolean GB32960_IsDchgAllowed(void);
uint8 GB32960_FotaCommandGet(void);
void GB32960_FotaCommandSet(uint8 flag);
const char *GB32960_FotaPackageUrlGet(void);
uint8 GB32960_LockCarCommandGet(void);
boolean GB32960_IsRmoteDcm(void);
boolean GB32960_IsLoggedIn(void);

extern boolean GBRtMsg_SetBatCalibRes(const uint8 *buf);

#endif
