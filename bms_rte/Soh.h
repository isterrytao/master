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

#endif

/**
 * \}
 */