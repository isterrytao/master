/**
 * \file Soh_Lcfg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief SOH配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20190517 |
 */
#include "Soh.h"

const Soh_ConfigInfoType Soh_ConfigInfo = {
    TRUE, /**< isMonitorNominalCapChange: soh是否监控程序额定容量变化 */
};

static Soh_DischgCntType Soh_DischgCnt[] = {
    {50U, 1000U},
    {500U, 950U},
    {1000U, 900U},
    {1500U, 850U},
    {2000U, 800U},
    {3000U, 700U},
};

const Soh_DischgCntInfoType Soh_DischgCntInfo = {
    Soh_DischgCnt, /* Soh_DischgCntType *dischgCntSoh */
    (uint8)(sizeof(Soh_DischgCnt) / sizeof(Soh_DischgCnt[0])), /* uint8 dischgCntSohNum */
    TRUE, /* boolean isLerp */
};

const Soh_SelfCalibConfigType Soh_SelfCalibConfig = {
    3U, /* uint8 loopThr; < 充放电循环次数阈值，分辨率 1次, 循环次数大于此阈值不校准 */
    30U, /* uint8 tempDiffThr; < 温度差阈值，分辨率 1摄氏度, 温度差大于此阈值不校准 */
    10U, /* uint8 sohCalibThr; < SOH校准阈值，分辨率 1%，校准量小于此阈值不校准 */
    10U, /* uint8 sohCalibStepMax; < 最大SOH校准步长，分辨率 1%，校准量大于此阈值时限制到阈值 */
    (7U * 24U * 12U), /* uint16 durationThr; < 统计时长阈值, 分辨率 5分钟, 时长大于此阈值不校准 */
    100U, /* uint8 powerCapRatio; 能量转换容量时，额定电压调整比率，分辨率 1% */
    60U, /* uint8 capRatioThr; < 观察容量比额定容量的最小比率，分辨率 1%，观察容量小于此阈值时不校准 */
    TRUE, /* boolean enable; < 使能开关 */
};
