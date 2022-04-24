/**
 * \file Soh.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief SOH头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20161213 |
 */

/**
 * \addtogroup Soh SOH
 * \{
 */


#ifndef SOH_H_
#define SOH_H_

#include "Std_Types.h"
#include "App_Types.h"
#include "Soh_Cfg.h"

/**
 * \brief SOH百分比值值类型定义 精度0.1%
 */
typedef uint16 App_SohType;

/**
 * \brief SOH与充放电次数关系表
 */
typedef struct {
    uint16 dischgCnt; /**< 放电次数 */
    App_SohType soh; /**< soh值 */
} Soh_DischgCntType;

typedef struct {
    Soh_DischgCntType *dischgCntSoh;
    uint8 dischgCntSohNum;
    boolean isLerp; /**< 是否线性插值 */
} Soh_DischgCntInfoType;

#define SOH_SELF_CALIB_DURA_RES (5U * 60U) /**< 统计时长阈值分辨率, 单位：秒 */

typedef struct {
    uint8 loopThr; /**< 充放电循环次数阈值，分辨率 1次, 循环次数大于此阈值不校准 */
    uint8 tempDiffThr; /**< 温度差阈值，分辨率 1摄氏度, 温度差大于此阈值不校准 */
    uint8 sohCalibThr; /**< SOH校准阈值，分辨率 1%，校准量小于此阈值不校准 */
    uint8 sohCalibStepMax; /**< 最大SOH校准步长，分辨率 1%，校准量大于此阈值时限制到阈值 */
    uint16 durationThr; /**< 统计时长阈值, 分辨率 5分钟, 时长大于此阈值不校准 */
    uint8 powerCapRatio; /**< 能量转换容量时，额定电压调整比率，分辨率 1% */
    uint8 capRatioThr; /**< 观察容量比额定容量的最小比率，分辨率 1%，观察容量小于此阈值时不校准 */
    boolean enable; /**< 使能开关 */
} Soh_SelfCalibConfigType;

/**
 * \brief SOH配置参数类型定义
 */
typedef struct{
    boolean isMonitorNominalCapChange; /**< soh是否监控程序额定容量变化 */
}Soh_ConfigInfoType;

/**
 * \brief SOH内部参数类型定义
 */
typedef struct{
    App_SohType soh; /**< soh值 */
    boolean selfCalibEn; /**< soh自校准启用开关 */
}Soh_InnerDataType;

extern const Soh_ConfigInfoType Soh_ConfigInfo;

/**
 * \brief Soh模块初始化
 * \note 依赖于系统的总容量、额定容量等信息
 */
void Soh_Init(void);

/**
 * \brief 获取当前实时SOH

 * \return 当前SOH值
 */
App_SohType Soh_Get(void);

/**
 * \brief 配置系统SOH值
 * \note 主要应用对系统SOH值的更新
 *
 * \param Soh_Value 新SOH值
 * \return E_OK:配置成功 E_NOT_OK:配置失败
 */
Std_ReturnType Soh_Set(App_SohType Soh_Value);

/**
 * \brief 计算soh
 *
 * \param totalCap 总容量
 * \param nominalCap 额定容量
 *
 * \return soh值
 */
App_SohType Soh_FormCap(App_CapType totalCap, App_CapType nominalCap);

/**
 * \brief 根据充放电次数配置soh
 *
 * \param dischgCnt 充放电次数
 *
 * \return E_OK:配置成功 E_NOT_OK:配置失败
 */
Std_ReturnType Soh_ConfigByDischgCnt(uint16 dischgCnt);

/**
 * \brief 根据给定的soh值配置soh
 *
 * \param soh 给定的soh值
 *
 * \return E_OK:配置成功 E_NOT_OK:配置失败
 */
Std_ReturnType Soh_ConfigBySoh(App_SohType soh);

/**
 * \brief 根据总容量配置soh
 *
 * \param totalCap 总容量
 *
 * \return E_OK:配置成功 E_NOT_OK:配置失败
 */
Std_ReturnType Soh_ConfigByTotalCap(App_CapType totalCap);

/**
 * \brief 满充事件回调，发生满充时，触发SOH自校准策略
 */
void SohSelfCalib_FullChgEventCbk(void);

/**
 * \brief 放空事件回调，发生放空时，触发SOH自校准策略
 */
void SohSelfCalib_EmptyDchgEventCbk(void);

/**
 * \brief 注册相关的诊断异常事件回调，发生异常时，清理SOH自校准信息
 */
void SohSelfCalib_RegisterDiagExcptCbk(void);

#endif

/**
 * \}
 */