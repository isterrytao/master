#include <string.h>
#include "GBRtMsg.h"
#include "Hall.h"
#include "CellDataM.h"
#include "AppInfo.h"
#include "HardwareSn.h"
#include "RuntimeM.h"
#include "Soc.h"
#include "Soh.h"
#include "Insu.h"
#include "BalanceM.h"
#include "Statistic.h"
#include "PwmCapture_Lcfg.h"
#include "ChargeConnectM.h"
#include "ChargerComm.h"
#include "ChgSckTmpM.h"
#include "EL.h"
#include "DtuComm_M35.h"
#include "RelayM_Lcfg.h"
#include "SystemConnection_Lcfg.h"
#include "Diagnosis.h"
#include "Dio.h"

#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "logger.h"



typedef struct {
    GB32960_RtMessageHeader GBRtHeader;
    union {
        struct {
            uint16 customMsgVersion;
            uint8 fwAddrSpace;
            uint32 runTime;
            uint8 runMode;
            uint16 chargeableCycles;
        } deviceInfo;
        struct {
            uint8 vinCode[17];
            uint8 deviceCnt;
        } deviceList;
        struct {
            uint16 soc;
            uint16 soh;
            uint16 insuResSys;
            uint16 insuResPos;
            uint16 insuResNeg;
            uint8 runStatus;
            uint8 wakeSignal;
            uint8 relayNum;
        } systemStatus;
        struct {
            uint8 totalSysNum;
            uint8 sysNumOfThisFrame;
            uint16 totalVoltage;
            sint16 current;
            uint16 totalCellNum;
            uint16 startCellOfThisFrame;
            uint8 cellNumOfThisFrame;
        } cellVoltages;
        struct {
            uint8 totalSysNum;
            uint8 sysNumOfThisFrame;
            uint16 totalCellNum;
        } cellTemperatures;
        struct {
            uint8 highVoltageSysNum;
            uint8 highVoltageCellNum;
            uint16 highVoltageValue;
            uint8 lowVoltageSysNum;
            uint8 lowVoltageCellNum;
            uint16 lowVoltageValue;
            uint8 highTemperatureSysNum;
            uint8 highTemperatureCellNum;
            uint8 highTemperatureValue;
            uint8 lowTemperatureSysNum;
            uint8 lowTemperatureCellNum;
            uint8 lowTemperatureValue;
        } cellPeakData;
        struct {
            uint16 supplyVoltage;
            uint8 ops;
            uint8 signalStrength;
            uint8 errorBitrate;
            uint16 lac;
            uint16 ci;
            sint32 longtitude;
            sint32 latitude;
        } dtuStatus;

        struct {
            uint8 connection;
            uint16 chargedTime;
            uint16 outputVoltage;
            uint16 outputCurrent;
            uint32 errorStatus;
            uint8 s2Status;
            uint16 ccResistor;
            uint16 cc2Resistor;
            uint16 cpFreq;
            uint16 cpDuty;
            uint8 elStatus;
            uint8 connectorTemp[3];
        } chargerStatus;

        struct {
            uint16 totalCellNum;
        } balanceStatus;

        struct {
            uint16 alarmNum;
        } alarmStatus;

        struct {
            uint8 chgType;
            uint16 chgResoltion;
            uint32 chgCumu;
            uint32 totalChgCumu;
            uint8 dchgType;
            uint16 dchgResoltion;
            uint32 dchgCumu;
            uint32 totalDchgCumu;
        } powerInfo;

    } dataHeader;
} GB32960_PACKED GBRt_MsgBuffer;

#define MSG_VERSION         0U

#define MEMBER_SIZEOF_MSG_HEADER(_member_) sizeof(((GBRt_MsgBuffer *)0)->dataHeader._member_)

#define MSG_LENGTH_DEVICE_INFO         (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(deviceInfo) + 16U/*FWID*/ + 16U/*BurnID*/)
#define MSG_LENGTH_DEVICE_LIST         (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(deviceList) + (uint16)(1U + SYSTEM_BMU_NUM) * sizeof(DeviceInfo_DeviceInfoType) /*HWIDs*/)
#define MSG_LENGTH_SYSTEM_STATUS       (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(systemStatus) + RELAYM_FN_NUM)
#define MSG_LENGTH_CELL_VOLTAGES       (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(cellVoltages) + (uint16)(2UL * SYSTEM_BATTERY_CELL_NUM))
#define MSG_LENGTH_CELL_TEMPERATURES   (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(cellTemperatures) + (uint16)(1UL * SYSTEM_TEMP_CELL_NUM))
#define MSG_LENGTH_CELL_PEAK_DATA      (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(cellPeakData))
#define MSG_LENGTH_DTU_STATUS          (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(dtuStatus))
#define MSG_LENGTH_BALANCE_STATUS      (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(balanceStatus) + (uint16)(7UL + SYSTEM_BATTERY_CELL_NUM) / 8U)
#define MSG_LENGTH_ALARM_STATUS        (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(alarmStatus))
#define MSG_LENGTH_CHARGER_STATUS      (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(chargerStatus))
#define MSG_LENGTH_CUMU_INFO           (1U/*信息类型标识*/ + MEMBER_SIZEOF_MSG_HEADER(powerInfo))


static GBRt_MsgBuffer headerBuffer;
static void fillDeviceInfo(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.deviceInfo.customMsgVersion = MSG_VERSION;
    msgHeader->dataHeader.deviceInfo.fwAddrSpace = AppInfoTag.FWAddrSpace;
    msgHeader->dataHeader.deviceInfo.runTime = OSTimeGet();
    msgHeader->dataHeader.deviceInfo.runMode = (uint8)RuntimeM_GetMode();
    msgHeader->dataHeader.deviceInfo.chargeableCycles = 0U;
}

static const GB32960_CopySegmentType copySegmentsDeviceInfo[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(deviceInfo), PTR_TYPE_DATA, {&headerBuffer}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(deviceInfo), 1U + MEMBER_SIZEOF_MSG_HEADER(deviceInfo) + 16U, PTR_TYPE_DATA, {AppInfoTag.FWID}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(deviceInfo) + 16U, 1U + MEMBER_SIZEOF_MSG_HEADER(deviceInfo) + 32U, PTR_TYPE_DATA, {AppInfoTag.BurnID}},
};

static void fillDeviceList(GBRt_MsgBuffer *msgHeader) {
    (void)strcpy((sint8 *)msgHeader->dataHeader.deviceList.vinCode, "1234567890ABCDEF");
    msgHeader->dataHeader.deviceList.deviceCnt = SYSTEM_BMU_NUM + 1U;
}

const GB32960_CopySegmentType copySegmentsDeviceList[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(deviceList), PTR_TYPE_DATA, {&headerBuffer}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(deviceList), 1U + MEMBER_SIZEOF_MSG_HEADER(deviceList) + 16U, PTR_TYPE_GET_DATA, {HardwareSn_GetPtr}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(deviceList) + 16U, 1U + MEMBER_SIZEOF_MSG_HEADER(deviceList) + 26U, PTR_TYPE_DATA, {AppInfoTag.FWVersion}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(deviceList) + 26U, 1U + MEMBER_SIZEOF_MSG_HEADER(deviceList) + 26U + SYSTEM_BMU_NUM * sizeof(DeviceInfo_DeviceInfoType), PTR_TYPE_GET_DATA, {DeviceInfo_GetPtr}},
};

static void fillSystemStatus(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.systemStatus.soc = Soc_Get();
    msgHeader->dataHeader.systemStatus.soh = Soh_Get();
    msgHeader->dataHeader.systemStatus.insuResSys = Insu_GetSystem();
    msgHeader->dataHeader.systemStatus.insuResPos = Insu_GetPositive();
    msgHeader->dataHeader.systemStatus.insuResNeg = Insu_GetNegative();
    msgHeader->dataHeader.systemStatus.runStatus = (ChargeConnectM_GetConnectType() != CHARGE_TYPE_NONE) ? 3U : 2U;
    msgHeader->dataHeader.systemStatus.wakeSignal = RuntimeM_GetWakeSignal();
    msgHeader->dataHeader.systemStatus.relayNum = RELAYM_FN_NUM;
    //for (i = 0; i < RELAYM_FN_NUM; ++i) {
    //    msgHeader->dataHeader.systemStatus.relayStatus[i] = RelayM_GetActualStatus(i) == RELAYM_ACTUAL_ON ? 1: 0;
    //}
}

static uint16 copyRelayData(uint16 offset, uint8 *buf, uint16 len) {
    uint16 i;
    for (i = 0U; i < len; i++) {
        *buf++ =  RelayM_GetActualStatus((uint8)(offset + i)) == RELAYM_ACTUAL_ON ? 1U : 0U;
    }

    return len;
}


static const GB32960_CopySegmentType copySegmentsSystemStatus[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(systemStatus), PTR_TYPE_DATA, {&headerBuffer}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(systemStatus), 1U + MEMBER_SIZEOF_MSG_HEADER(systemStatus) + RELAYM_FN_NUM, PTR_TYPE_COPY_DATA, {copyRelayData}}
};


static void fillCellVoltage(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.cellVoltages.totalSysNum = 1U;
    msgHeader->dataHeader.cellVoltages.sysNumOfThisFrame = 1U;
    msgHeader->dataHeader.cellVoltages.totalVoltage = Statistic_GetBcu100mvTotalVoltage();
    msgHeader->dataHeader.cellVoltages.current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    msgHeader->dataHeader.cellVoltages.totalCellNum = SYSTEM_BATTERY_CELL_NUM;
    msgHeader->dataHeader.cellVoltages.startCellOfThisFrame = 1U;
    msgHeader->dataHeader.cellVoltages.cellNumOfThisFrame = (uint8)(SYSTEM_BATTERY_CELL_NUM & 0xFFU);
}

static const GB32960_CopySegmentType copySegmentsCellVoltage[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(cellVoltages), PTR_TYPE_DATA, {&headerBuffer}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(cellVoltages), 1U + MEMBER_SIZEOF_MSG_HEADER(cellVoltages) + (uint16)(2UL * SYSTEM_BATTERY_CELL_NUM), PTR_TYPE_GET_DATA, {CellDataM_GetVoltagePtr}}
};

static void fillCellTemperatures(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.cellTemperatures.totalSysNum = 1U;
    msgHeader->dataHeader.cellTemperatures.sysNumOfThisFrame = 1U;
    msgHeader->dataHeader.cellTemperatures.totalCellNum = SYSTEM_TEMP_CELL_NUM;
}

static uint16 copyTempData(uint16 offset, uint8 *buf, uint16 len) {
    uint16 i;
    const uint8 *tmpPtr = CellDataM_GetTempraturePtr() + offset;

    for (i = 0U; i < len; i++) {
        if (*tmpPtr > 10U && *tmpPtr < 250U) {
            *buf++  = *tmpPtr++ - 10U;
        } else {
            *buf++ = *tmpPtr++;
        }
    }
    return i;
}

static const GB32960_CopySegmentType copySegmentsCellTemperatures[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(cellTemperatures), PTR_TYPE_DATA, {&headerBuffer}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(cellTemperatures), 1U + MEMBER_SIZEOF_MSG_HEADER(cellTemperatures) + (uint16)(1UL * SYSTEM_TEMP_CELL_NUM), PTR_TYPE_COPY_DATA, {copyTempData}}
};


static void fillCellPeakData(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.cellPeakData.highVoltageSysNum = 1U;
    msgHeader->dataHeader.cellPeakData.highVoltageCellNum = (uint8)(Statistic_GetBcuHvLogicIndex(0U) + 1U);
    msgHeader->dataHeader.cellPeakData.highVoltageValue = Statistic_GetBcuHv(0U);
    msgHeader->dataHeader.cellPeakData.lowVoltageSysNum = 1U;
    msgHeader->dataHeader.cellPeakData.lowVoltageCellNum = (uint8)(Statistic_GetBcuLvLogicIndex(0U) + 1U);
    msgHeader->dataHeader.cellPeakData.lowVoltageValue = Statistic_GetBcuLv(0U);
    msgHeader->dataHeader.cellPeakData.highTemperatureSysNum = 1U;
    msgHeader->dataHeader.cellPeakData.highTemperatureCellNum = (uint8)(Statistic_GetBcuHtLogicIndex(0U) + 1U);
    msgHeader->dataHeader.cellPeakData.highTemperatureValue = Statistic_GetBcuHt(0U) - 10U;
    msgHeader->dataHeader.cellPeakData.lowTemperatureSysNum = 1U;
    msgHeader->dataHeader.cellPeakData.lowTemperatureCellNum = (uint8)(Statistic_GetBcuLtLogicIndex(0U) + 1U);
    msgHeader->dataHeader.cellPeakData.lowTemperatureValue = Statistic_GetBcuLt(0U) - 10U;
}

static const GB32960_CopySegmentType copySegmentsCellPeakData[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(cellPeakData), PTR_TYPE_DATA, {&headerBuffer}},
};

static void fillDtuStatus(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.dtuStatus.supplyVoltage = DtuM35_GetSupplyVoltage();
    msgHeader->dataHeader.dtuStatus.ops = DtuM35_GetOPS();
    DtuM35_GetSingalQuality(&msgHeader->dataHeader.dtuStatus.signalStrength,
                            &msgHeader->dataHeader.dtuStatus.errorBitrate);
    DtuM35_GetLacCi(&msgHeader->dataHeader.dtuStatus.lac);
    DtuM35_GetLoc(&msgHeader->dataHeader.dtuStatus.longtitude);
}
static const GB32960_CopySegmentType copySegmentsDtuStatus[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(dtuStatus), PTR_TYPE_DATA, {&headerBuffer}},
};

static void fillChargerStatus(GBRt_MsgBuffer *msgHeader) {
    uint8 temp;
    uint32 temp32;

    msgHeader->dataHeader.chargerStatus.connection = ChargeConnectM_GetConnectType();
    temp32 = Statistic_GetEclipseChargeTime();
    if (temp32 > 0xFFFFU) {
        temp32 = 0xFFFFU;
    }
    msgHeader->dataHeader.chargerStatus.chargedTime = (uint16)temp32;
    msgHeader->dataHeader.chargerStatus.outputVoltage = ChargerComm_GetChargerOutputHV();
    msgHeader->dataHeader.chargerStatus.outputCurrent = (uint16)ChargerComm_GetChargerOutputCurrent();
    msgHeader->dataHeader.chargerStatus.errorStatus = ChargerComm_GetChargerFaultBitmap();
    msgHeader->dataHeader.chargerStatus.s2Status =  ChargeConnectM_GetS2();
    msgHeader->dataHeader.chargerStatus.ccResistor = ChargerConnectM_GetCCxResistanceStatus(CHARGECONNECTM_ADC_CC_CHANNEL);
    msgHeader->dataHeader.chargerStatus.cc2Resistor = ChargerConnectM_GetCCxResistanceStatus(CHARGECONNECTM_ADC_CC2_CHANNEL);
    msgHeader->dataHeader.chargerStatus.cpFreq = CP_GetFrequence();
    msgHeader->dataHeader.chargerStatus.cpDuty = CP_GetDuty();
    msgHeader->dataHeader.chargerStatus.elStatus = (uint8)EL_GetDiagnosisStatus(0U);
    if (msgHeader->dataHeader.chargerStatus.connection == CHARGE_TYPE_AC) {
        temp = ChgSckTmpM_GetTemperature(CHGSCKTMPM_TEMPERATURE_AC_AL);
        if (temp >= 10U && temp <= 250U) {
            temp -= 10U;
        }
        msgHeader->dataHeader.chargerStatus.connectorTemp[0U] = temp;
        temp = ChgSckTmpM_GetTemperature(CHGSCKTMPM_TEMPERATURE_AC_BN);
        if (temp >= 10U && temp <= 250U) {
            temp -= 10U;
        }
        msgHeader->dataHeader.chargerStatus.connectorTemp[1U] = temp;
        temp = ChgSckTmpM_GetTemperature(CHGSCKTMPM_TEMPERATURE_AC_C);
        if (temp >= 10U && temp <= 250U) {
            temp -= 10U;
        }
        msgHeader->dataHeader.chargerStatus.connectorTemp[2U] = temp;
    } else if (msgHeader->dataHeader.chargerStatus.connection == CHARGE_TYPE_DC) {
        temp = ChgSckTmpM_GetTemperature(CHGSCKTMPM_TEMPERATURE_DC_POSITIVE);
        if (temp >= 10U && temp <= 250U) {
            temp -= 10U;
        }
        msgHeader->dataHeader.chargerStatus.connectorTemp[0U] = temp;
        temp = ChgSckTmpM_GetTemperature(CHGSCKTMPM_TEMPERATURE_DC_NEGATIVE);
        if (temp >= 10U && temp <= 250U) {
            temp -= 10U;
        }
        msgHeader->dataHeader.chargerStatus.connectorTemp[1U] = temp;
        msgHeader->dataHeader.chargerStatus.connectorTemp[2U] = 0xFFU;
    } else {
        msgHeader->dataHeader.chargerStatus.connectorTemp[0U] = 0xFFU;
        msgHeader->dataHeader.chargerStatus.connectorTemp[1U] = 0xFFU;
        msgHeader->dataHeader.chargerStatus.connectorTemp[2U] = 0xFFU;
    }
}

static const GB32960_CopySegmentType copySegmentsChargerStatus[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(chargerStatus), PTR_TYPE_DATA, {&headerBuffer}},
};

static void fillBalanceStatus(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.balanceStatus.totalCellNum = SYSTEM_BATTERY_CELL_NUM;
}

static const GB32960_CopySegmentType copySegmentsBalanceStatus[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(balanceStatus), PTR_TYPE_DATA, {&headerBuffer}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(balanceStatus), 1U + MEMBER_SIZEOF_MSG_HEADER(balanceStatus) + (uint16)(7UL + SYSTEM_BATTERY_CELL_NUM) / 8U, PTR_TYPE_GET_DATA, {BalanceM_GetBalanceStatusPtr}},
};


static Diagnosis_LevelType alarms[DIAGNOSIS_ITEM_DEFAULT_NUM];
static uint8 alaramLengthCaculated = 0U;
static uint8 alaramNum = 0U;
static uint8 alarmCopyOffset = 0U;
static void fillAlarmStatus(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.alarmStatus.alarmNum = alaramNum;
}

static uint16 copyAlarmData(uint16 offset, uint8 *buf, uint16 len) {
    uint16 copyed = 0U;

    (void)offset;

    if (alarmCopyOffset >= (uint8)DIAGNOSIS_ITEM_MAX_NUM) {
        (void)memset(buf, (int)DIAGNOSIS_LEVEL_NONE, len);
        copyed = len;
    }
    else {
        while (len >= 2U && alarmCopyOffset < ARRAY_SIZE(alarms)) {
            if (alarms[alarmCopyOffset] != DIAGNOSIS_LEVEL_NONE) {
                len -= 2U;
                copyed += 2U;
                *buf++ = (uint8)Diagnosis_IndexToItem(alarmCopyOffset);
                *buf++ = alarms[alarmCopyOffset];
            }
            ++alarmCopyOffset;
        }
    }
    return copyed;
}

static uint16 getAlarmLength(void) {
    if (alaramLengthCaculated == 0U) {
        uint8 i;
        alarmCopyOffset = 0U;
        alaramNum = 0U;
        Diagnosis_GetAllLevelByIndex(alarms, ARRAY_SIZE(alarms));
        for (i = 0U; i < ARRAY_SIZE(alarms); ++i) {
            if (alarms[i] != DIAGNOSIS_LEVEL_NONE) {
                alaramNum++;
            }
        }
        alaramLengthCaculated = 1U;
    }
    return 1U + MEMBER_SIZEOF_MSG_HEADER(alarmStatus) + 2U * (uint16)alaramNum;
}

static void clearAlarmLength(void) {
    alaramLengthCaculated = 0U;
}

static const GB32960_CopySegmentType copySegmentsAlarmStatus[] = {
    {0U, 1U + MEMBER_SIZEOF_MSG_HEADER(alarmStatus), PTR_TYPE_DATA, {&headerBuffer}},
    {1U + MEMBER_SIZEOF_MSG_HEADER(alarmStatus), 0xFFFEU, PTR_TYPE_COPY_DATA, {copyAlarmData}},
};

const GB32960_RTMessageItemType loginOnceData[] = {
    {0x81U, TRUE, LEN_TYPE_LENGTH, MSG_LENGTH_DEVICE_INFO, NULL, (GB32960_FillMessageFuncType)fillDeviceInfo, copySegmentsDeviceInfo, (uint8)ARRAY_SIZE(copySegmentsDeviceInfo)},
    {0x82U, TRUE, LEN_TYPE_LENGTH, MSG_LENGTH_DEVICE_LIST, NULL, (GB32960_FillMessageFuncType)fillDeviceList, copySegmentsDeviceList, (uint8)ARRAY_SIZE(copySegmentsDeviceList)},
    {0xFFU, TRUE, LEN_TYPE_LENGTH, 0x00U, NULL, NULL, NULL, 0U}
};

const GB32960_RTMessageItemType intervalData[] = {
    {0x83U, TRUE, LEN_TYPE_LENGTH, MSG_LENGTH_SYSTEM_STATUS, NULL, (GB32960_FillMessageFuncType)fillSystemStatus, copySegmentsSystemStatus, (uint8)ARRAY_SIZE(copySegmentsSystemStatus)},
    {0x08U, TRUE, LEN_TYPE_LENGTH, MSG_LENGTH_CELL_VOLTAGES, NULL, (GB32960_FillMessageFuncType)fillCellVoltage, copySegmentsCellVoltage, (uint8)ARRAY_SIZE(copySegmentsCellVoltage)},
    {0x09U, TRUE, LEN_TYPE_LENGTH, MSG_LENGTH_CELL_TEMPERATURES, NULL, (GB32960_FillMessageFuncType)fillCellTemperatures, copySegmentsCellTemperatures, (uint8)ARRAY_SIZE(copySegmentsCellTemperatures)},
    {0x06U, TRUE, LEN_TYPE_LENGTH, MSG_LENGTH_CELL_PEAK_DATA, NULL, (GB32960_FillMessageFuncType)fillCellPeakData, copySegmentsCellPeakData, (uint8)ARRAY_SIZE(copySegmentsCellPeakData)},
    {0x84U, TRUE, LEN_TYPE_LENGTH, MSG_LENGTH_DTU_STATUS, NULL, (GB32960_FillMessageFuncType)fillDtuStatus, copySegmentsDtuStatus, (uint8)ARRAY_SIZE(copySegmentsDtuStatus)},
    {0x86U, FALSE, LEN_TYPE_LENGTH, MSG_LENGTH_BALANCE_STATUS, NULL, (GB32960_FillMessageFuncType)fillBalanceStatus, copySegmentsBalanceStatus, (uint8)ARRAY_SIZE(copySegmentsBalanceStatus)},
    {0x85U, FALSE, LEN_TYPE_LENGTH, MSG_LENGTH_CHARGER_STATUS, NULL, (GB32960_FillMessageFuncType)fillChargerStatus, copySegmentsChargerStatus, (uint8)ARRAY_SIZE(copySegmentsChargerStatus)},
    {0x87U, TRUE, LEN_TYPE_GET_LENGTH, 0U, getAlarmLength, (GB32960_FillMessageFuncType)fillAlarmStatus, copySegmentsAlarmStatus, (uint8)ARRAY_SIZE(copySegmentsAlarmStatus)},
    {0xFFU, TRUE, LEN_TYPE_LENGTH, 0x00U, NULL, NULL, NULL, 0U}
};


const GB32960_RtMessageConfigDataType GB32960RtMessageConfigData = {
    &headerBuffer.GBRtHeader,
    loginOnceData,
    intervalData,
    clearAlarmLength,
};


#include "SaveM.h"

static GBRt_MsgBuffer record_header;

/**
 * \brief      20170315: 经讨论决定,使用国标协议版本号低字节作为数据存储记录类型的版本标识
 */
#if MSG_VERSION > 0U
#define GB_TYPE_TO_RECORD_TYPE(type)    ((((uint16)MSG_VERSION << 8) & 0xFF00U) | type)
#else
#define GB_TYPE_TO_RECORD_TYPE(type)    (type)
#endif

static const GB32960_CopySegmentType copyRecordSegmentsDeviceInfo[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(deviceInfo), PTR_TYPE_DATA, {&record_header.dataHeader}},
    {MEMBER_SIZEOF_MSG_HEADER(deviceInfo), MEMBER_SIZEOF_MSG_HEADER(deviceInfo) + 16U, PTR_TYPE_DATA, {AppInfoTag.FWID}},
    {MEMBER_SIZEOF_MSG_HEADER(deviceInfo) + 16U, MEMBER_SIZEOF_MSG_HEADER(deviceInfo) + 32U, PTR_TYPE_DATA, {AppInfoTag.BurnID}},
};

const GB32960_CopySegmentType copyRecordSegmentsDeviceList[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(deviceList), PTR_TYPE_DATA, {&record_header.dataHeader}},
    {MEMBER_SIZEOF_MSG_HEADER(deviceList), MEMBER_SIZEOF_MSG_HEADER(deviceList) + 16U, PTR_TYPE_GET_DATA, {HardwareSn_GetPtr}},
    {MEMBER_SIZEOF_MSG_HEADER(deviceList) + 16U, MEMBER_SIZEOF_MSG_HEADER(deviceList) + 26U, PTR_TYPE_DATA, {AppInfoTag.FWVersion}},
    {MEMBER_SIZEOF_MSG_HEADER(deviceList) + 26U, MEMBER_SIZEOF_MSG_HEADER(deviceList) + 26U + SYSTEM_BMU_NUM * sizeof(DeviceInfo_DeviceInfoType), PTR_TYPE_GET_DATA, {DeviceInfo_GetPtr}},
};

static const GB32960_CopySegmentType copyRecordSegmentsSystemStatus[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(systemStatus), PTR_TYPE_DATA, {&record_header.dataHeader}},
    {MEMBER_SIZEOF_MSG_HEADER(systemStatus), MEMBER_SIZEOF_MSG_HEADER(systemStatus) + RELAYM_FN_NUM, PTR_TYPE_COPY_DATA, {copyRelayData}}
};

static const GB32960_CopySegmentType copyRecordSegmentsCellVoltage[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(cellVoltages), PTR_TYPE_DATA, {&record_header.dataHeader}},
    {MEMBER_SIZEOF_MSG_HEADER(cellVoltages), MEMBER_SIZEOF_MSG_HEADER(cellVoltages) + (uint16)(2UL * SYSTEM_BATTERY_CELL_NUM), PTR_TYPE_GET_DATA, {CellDataM_GetVoltagePtr}}
};

static const GB32960_CopySegmentType copyRecordSegmentsCellTemperatures[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(cellTemperatures), PTR_TYPE_DATA, {&record_header.dataHeader}},
    {MEMBER_SIZEOF_MSG_HEADER(cellTemperatures), MEMBER_SIZEOF_MSG_HEADER(cellTemperatures) + (uint16)(1UL * SYSTEM_TEMP_CELL_NUM), PTR_TYPE_COPY_DATA, {copyTempData}}
};

static const GB32960_CopySegmentType copyRecordSegmentsCellPeakData[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(cellPeakData), PTR_TYPE_DATA, {&record_header.dataHeader}},
};

static const GB32960_CopySegmentType copyRecordSegmentsDtuStatus[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(dtuStatus), PTR_TYPE_DATA, {&record_header.dataHeader}},
};

static const GB32960_CopySegmentType copyRecordSegmentsChargerStatus[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(chargerStatus), PTR_TYPE_DATA, {&record_header.dataHeader}},
};

static const GB32960_CopySegmentType copyRecordSegmentsBalanceStatus[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(balanceStatus), PTR_TYPE_DATA, {&record_header.dataHeader}},
    {MEMBER_SIZEOF_MSG_HEADER(balanceStatus), MEMBER_SIZEOF_MSG_HEADER(balanceStatus) + (uint16)(7UL + SYSTEM_BATTERY_CELL_NUM) / 8U, PTR_TYPE_GET_DATA, {BalanceM_GetBalanceStatusPtr}},
};

static void fillPowerInfoRecord(GBRt_MsgBuffer *msgHeader) {
    msgHeader->dataHeader.powerInfo.chgType = (uint8)Soc_ConfigInfo.ChgCumuInfo.type;
    msgHeader->dataHeader.powerInfo.chgResoltion = Soc_ConfigInfo.ChgCumuInfo.resoltion;
    msgHeader->dataHeader.powerInfo.chgCumu = Soc_GetChgPower();
    msgHeader->dataHeader.powerInfo.totalChgCumu = Soc_GetCumuChgPower();
    msgHeader->dataHeader.powerInfo.dchgType = (uint8)Soc_ConfigInfo.DchgCumuInfo.type;
    msgHeader->dataHeader.powerInfo.dchgResoltion = Soc_ConfigInfo.DchgCumuInfo.resoltion;
    msgHeader->dataHeader.powerInfo.dchgCumu = Soc_GetDchgPower();
    msgHeader->dataHeader.powerInfo.totalDchgCumu = Soc_GetCumuDchgPower();
}

static const GB32960_CopySegmentType copyRecordSegmentsPowerInfo[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(powerInfo), PTR_TYPE_DATA, {&record_header.dataHeader}},
};

static Diagnosis_LevelType alarmsRecord[DIAGNOSIS_ITEM_MAX_NUM];
static uint8 alaramNumRecord = 0U;
static uint8 alarmCopyOffsetRecord = 0U;
static void fillAlarmStatusRecord(GBRt_MsgBuffer *msgHeader) {
    uint8 i;
    alarmCopyOffsetRecord = 0U;
    alaramNumRecord = 0U;
    Diagnosis_GetAllLevelByIndex(alarmsRecord, ARRAY_SIZE(alarmsRecord));
    for (i = 0U; i < ARRAY_SIZE(alarmsRecord); ++i) {
        if (alarmsRecord[i] != DIAGNOSIS_LEVEL_NONE) {
            alaramNumRecord++;
        }
    }
    msgHeader->dataHeader.alarmStatus.alarmNum = alaramNumRecord;
}

static uint16 copyAlarmDataRecord(uint16 offset, uint8 *buf, uint16 len) {
    uint16 copyed = 0U;

    (void)offset;

    if (alarmCopyOffsetRecord >= (uint8)DIAGNOSIS_ITEM_MAX_NUM) {
        (void)memset(buf, (int)DIAGNOSIS_LEVEL_NONE, len);
        copyed = len;
    }
    else {
        while (len >= 2U && alarmCopyOffsetRecord < ARRAY_SIZE(alarmsRecord)) {
            if (alarmsRecord[alarmCopyOffsetRecord] != DIAGNOSIS_LEVEL_NONE) {
                len -= 2U;
                copyed += 2U;
                *buf++ = (uint8)Diagnosis_IndexToItem(alarmCopyOffsetRecord);
                *buf++ = alarmsRecord[alarmCopyOffsetRecord];
            }
            ++alarmCopyOffsetRecord;
        }
    }
    return copyed;
}

static uint16 getAlarmLengthRecord(void) {
    return 1U + MEMBER_SIZEOF_MSG_HEADER(alarmStatus) + 2U * (uint16)alaramNumRecord;
}

static const GB32960_CopySegmentType copyRecordSegmentsAlarmStatus[] = {
    {0U, MEMBER_SIZEOF_MSG_HEADER(alarmStatus), PTR_TYPE_DATA, {&record_header.dataHeader}},
    {MEMBER_SIZEOF_MSG_HEADER(alarmStatus), 0xFFFEU, PTR_TYPE_COPY_DATA,  {copyAlarmDataRecord}},
};

static boolean has_dtu_module(void) {
    return DIO_CHANNEL_DTU_IS_PRESENT() ? TRUE : FALSE;
}

static boolean is_valid_in_charging_mode(void) {
    return CHARGECONNECTM_IS_CONNECT() ? TRUE : FALSE;
}

static const GB32960_RecordItemType onceDataList[] = {
    {GB_TYPE_TO_RECORD_TYPE(0x81U), MSG_LENGTH_DEVICE_INFO, NULL, (GB32960_FillMessageFuncType)fillDeviceInfo, copyRecordSegmentsDeviceInfo, (uint8)ARRAY_SIZE(copyRecordSegmentsDeviceInfo), NULL},
    {GB_TYPE_TO_RECORD_TYPE(0x82U), MSG_LENGTH_DEVICE_LIST, NULL, (GB32960_FillMessageFuncType)fillDeviceList, copyRecordSegmentsDeviceList, (uint8)ARRAY_SIZE(copyRecordSegmentsDeviceList), NULL},
    {0xFFFFU, 0x00U, NULL, NULL, NULL, 0U, NULL}
};

static const GB32960_RecordItemType fixedCycleDataList[] = {
    {GB_TYPE_TO_RECORD_TYPE(0x84U), MSG_LENGTH_DTU_STATUS, NULL, (GB32960_FillMessageFuncType)fillDtuStatus, copyRecordSegmentsDtuStatus, (uint8)ARRAY_SIZE(copyRecordSegmentsDtuStatus), has_dtu_module},
    {0xFFFFU, 0x00U, NULL, NULL, NULL, 0U, NULL}
};



static const GB32960_RecordItemType unfixedCycleDataList[] = {
    {GB_TYPE_TO_RECORD_TYPE(0x83U), MSG_LENGTH_SYSTEM_STATUS, NULL, (GB32960_FillMessageFuncType)fillSystemStatus, copyRecordSegmentsSystemStatus, (uint8)ARRAY_SIZE(copyRecordSegmentsSystemStatus), NULL},
    {GB_TYPE_TO_RECORD_TYPE(0x08U), MSG_LENGTH_CELL_VOLTAGES, NULL, (GB32960_FillMessageFuncType)fillCellVoltage, copyRecordSegmentsCellVoltage, (uint8)ARRAY_SIZE(copyRecordSegmentsCellVoltage), NULL},
    {GB_TYPE_TO_RECORD_TYPE(0x09U), MSG_LENGTH_CELL_TEMPERATURES, NULL, (GB32960_FillMessageFuncType)fillCellTemperatures, copyRecordSegmentsCellTemperatures, (uint8)ARRAY_SIZE(copyRecordSegmentsCellTemperatures), NULL},
    {GB_TYPE_TO_RECORD_TYPE(0x06U), MSG_LENGTH_CELL_PEAK_DATA, NULL, (GB32960_FillMessageFuncType)fillCellPeakData, copyRecordSegmentsCellPeakData, (uint8)ARRAY_SIZE(copyRecordSegmentsCellPeakData), NULL},
    {GB_TYPE_TO_RECORD_TYPE(0x86U), MSG_LENGTH_BALANCE_STATUS, NULL, (GB32960_FillMessageFuncType)fillBalanceStatus, copyRecordSegmentsBalanceStatus, (uint8)ARRAY_SIZE(copyRecordSegmentsBalanceStatus), BalanceM_IsBalance},
    {GB_TYPE_TO_RECORD_TYPE(0x85U), MSG_LENGTH_CHARGER_STATUS, NULL, (GB32960_FillMessageFuncType)fillChargerStatus, copyRecordSegmentsChargerStatus, (uint8)ARRAY_SIZE(copyRecordSegmentsChargerStatus), is_valid_in_charging_mode},
    {GB_TYPE_TO_RECORD_TYPE(0x87U), 0U, getAlarmLengthRecord, (GB32960_FillMessageFuncType)fillAlarmStatusRecord, copyRecordSegmentsAlarmStatus, (uint8)ARRAY_SIZE(copyRecordSegmentsAlarmStatus), NULL},
    {GB_TYPE_TO_RECORD_TYPE(0x89U), MSG_LENGTH_CUMU_INFO, NULL, (GB32960_FillMessageFuncType)fillPowerInfoRecord, copyRecordSegmentsPowerInfo, (uint8)ARRAY_SIZE(copyRecordSegmentsPowerInfo), NULL},
    {0xFFFFU, 0x00U, NULL, NULL, NULL, 0U, NULL}
};

static uint32 UnfixedAlarmListCycle[] = {
    30000U,  /*!< 正常 */
    20000U,  /*!< 一级报警 */
    10000U,  /*!< 二级报警 */
    5000U,   /*!< 三级报警 */
    1000U,   /*!< 四级报警 */
};
static const Diagnosis_ItemType UnfixedAlarmList[] = {
    DIAGNOSIS_ITEM_CHG_HV, /**< 充电单体高压 */
    DIAGNOSIS_ITEM_DCHG_HV, /**< 放电单体高压 */
    DIAGNOSIS_ITEM_CHG_LV, /**< 充电单体低压 */
    DIAGNOSIS_ITEM_DCHG_LV, /**< 放电单体低压 */
    DIAGNOSIS_ITEM_CHG_HTV, /**< 充电总压高 */
    DIAGNOSIS_ITEM_DCHG_HTV, /**< 放电总压高 */
    DIAGNOSIS_ITEM_CHG_LTV, /**< 充电总压低 */
    DIAGNOSIS_ITEM_DCHG_LTV, /**< 放电总压低 */
    DIAGNOSIS_ITEM_CHG_DTV, /**< 充电总压差 */
    DIAGNOSIS_ITEM_DCHG_DTV, /**< 放电总压差 */
    DIAGNOSIS_ITEM_CHG_DV, /**< 充电压差 */
    DIAGNOSIS_ITEM_DCHG_DV, /**< 放电压差 */

    DIAGNOSIS_ITEM_CHG_HT, /**< 充电高温 */
    DIAGNOSIS_ITEM_DCHG_HT, /**< 放电高温 */
    DIAGNOSIS_ITEM_CHG_LT, /**< 充电低温 */
    DIAGNOSIS_ITEM_DCHG_LT, /**< 放电低温 */
    DIAGNOSIS_ITEM_CHG_DT, /**< 充电温差 */
    DIAGNOSIS_ITEM_DCHG_DT, /**< 放电温差 */

    DIAGNOSIS_ITEM_DC_CHG_OC, /**< 快充过流 */
    DIAGNOSIS_ITEM_AC_CHG_OC, /**< 慢充过流 */
    DIAGNOSIS_ITEM_FB_OC, /**< 回馈过流 */
    DIAGNOSIS_ITEM_DCHG_OC, /**< 持续放电过流 */
    DIAGNOSIS_ITEM_SP_OC, /**< 加速过流/瞬时放电过流 */

    DIAGNOSIS_ITEM_HSOC, /**< SOC高 */
    DIAGNOSIS_ITEM_LSOC, /**< SOC低 */

    DIAGNOSIS_ITEM_LEAK, /**< 漏电 */

    DIAGNOSIS_ITEM_VOLT_LINE, /**< 电压排线 */
    DIAGNOSIS_ITEM_TEMP_LINE, /**< 温感排线 */
};

static uint32 unfixed_get_cycle(void) {
    uint16 lv_flags[DIAGNOSIS_SUPPORT_LEVEL] = {0U};
    uint8 i;
    Diagnosis_LevelType lv;
    uint32 res = UnfixedAlarmListCycle[0];

    for(i = 0U; i < ARRAY_SIZE(UnfixedAlarmList); i++) {
        lv = Diagnosis_GetLevel(UnfixedAlarmList[i]);
        switch(lv){
#if DIAGNOSIS_SUPPORT_LEVEL > 3U
        case DIAGNOSIS_LEVEL_FOURTH:
            lv_flags[3] |= ((uint16)1U << i);
            break;
#endif
#if DIAGNOSIS_SUPPORT_LEVEL > 2U
        case DIAGNOSIS_LEVEL_THIRD:
            lv_flags[2] |= ((uint16)1U << i);
            break;
#endif
#if DIAGNOSIS_SUPPORT_LEVEL > 1U
        case DIAGNOSIS_LEVEL_SECOND:
            lv_flags[1] |= ((uint16)1U << i);
            break;
#endif
        case DIAGNOSIS_LEVEL_FIRST:
            lv_flags[0] |= ((uint16)1U << i);
            break;
        default:
            break;
        }
    }
    for(i = DIAGNOSIS_SUPPORT_LEVEL; i--;) {
        if(lv_flags[i]) {
            res = UnfixedAlarmListCycle[i + 1U];
            break;
        }
    }
    return res;
}


uint16 GBRtMsg_GetRecordLength(const GB32960_RecordItemType *item) {
    uint16 length = 0U;
    if (item->getLength) {
        length = item->getLength();
    } else {
        length = item->length;
    }
    /**
     * 数据长度不包含信息类型标识,所以此处需要减去该大小
     */
    return length - sizeof(((GBRt_MsgBuffer *)0)->GBRtHeader);
}

const SaveM_RecordConfigDataType SaveMRecordConfigData = {
    /* GBRt_MsgBuffer *headerData; */ &record_header.GBRtHeader,
    /* onPowerUpOnceData = */ {
        0U,
        NULL,
        onceDataList
    },
    /* fixedCycleData = */ {
        30000U,
        NULL,
        fixedCycleDataList
    },
    /* unfixedCycleData = */ {
        30000U,
        unfixed_get_cycle,
        unfixedCycleDataList
    }
};
