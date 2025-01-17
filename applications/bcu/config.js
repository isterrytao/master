module.exports = {
    notes:[
        {section:"BatteryInfo", chs:"电池基本信息", comments:"描述电池的相关信息."},
        {section:"SystemConnection", chs:"系统连接配置", comments:"描述主机型号,从机型号, 以及电池温感连接情况."},
        {section:"CanDrv", chs:"Can驱动层配置", comments:"描述HOH与硬件Can的对应关系"},
        {section:"CanIf", chs:"Can接口层配置", comments:"描述上层需要通过CanIf发送或接受的数据帧ID以及使用的HOH, 不需要配置主从协议的"},
        {section:"CanTp", chs:"Can传输层配置", comments:"描述需要使用CanTp接受或发送的通道及对应的CanId, 不需要配置主从协议的"},
        {section:"CanTp", chs:"Can传输层配置", comments:"描述需要使用CanTp接受或发送的通道及对应的CanId, 不需要配置主从协议的"},
    ],

    AppInfo: {
        releaseType: "T",
        releaseType_comments: '"T"或"R", 请参考<<软件版本规范>>',

        comments: "如果为模版, 请保持下列配置都为空",
        pid: "S144.000.868.01",
        pid_comments: "项目编码, 请参考<<优旦项目编码规范>>",
        requireVer: "1.1",
        requireVer_comments: "需求版本, 请参考<<软件版本规范>>",
        buildVer: "3",
        buildVer_comments: "编译版本, 请参考<<软件版本规范>>",
    },

    BatteryInfo: {
        notes: [
            {section:"type", chs: "电池类型", comments:'可选:"铅酸","镍氢","磷酸铁锂","锰酸锂","钴酸锂","三元材料","聚合物锂离子","钛酸锂","其他"'},
            {section:"nominal", chs: "额定参数", comments:'电池的一些额定参数'},
            {section:"serialnum", chs: "电池串数"},
            {section:"parallelnum", chs: "电池并数"},
            {section:"celltempnum", chs: "电池单体温感数量"},
            {section:"poletempnum", chs: "电池极柱温感数量"},
            {section:"heattempnum", chs: "加热片温感数量"},
            {section:"initsoc", chs: "初始SOC, 单位1%"},
            {section:"initsoh", chs: "初始SOH, 单位1%"},
        ],
        type: "磷酸铁锂",
        nominal: {
            notes:[
                {section:"cap", chs:"额定容量, 单位Ah"},
                {section:"displaycap", chs:"显示容量,单位Ah"},
                {section:"dischargecurrent", chs:"额定放电电流, 单位A"},
                {section:"dcchargecurrent", chs:"额定快充电流, 单位A"},
                {section:"acchargecurrent", chs:"额定慢充电流, 单位A"},
            ],
            cap: 440,
            displaycap: 450,
            dischargecurrent: 250,
            totalvoltage: 51.2,
            dcchargecurrent: 200,
            acchargecurrent: 200,
        },
        serialnum: 16,
        parallelnum: 3,
        celltempnum: 8,
        heattempnum: 0,
        poletempnum: 0,
        initsoc: 50,
        initsoh: 100,
    },
    SystemConnection:{
        notes:[
            {section:"bcu", chs:"BCU配置", comments:"主机描述."},
            {section:"bmus", chs:"BMU配置", comments:"从机描述"},
        ],

        bcu:{
            notes:[
                {section:"type", chs:"BCU类型", comments:'可选:"C600", "C601", "A600", "A601", "A602", "A603", "A650", "A651", "A652", "A653", "A660", "A661".'},
                {section:"battconn", chs:"电池连接", comments:"电池连接, 只有型号为A60X的时候有效, 长度与型号相关."},
                {section:"tempconn", chs:"温感连接", comments:"只有型号为A60X的时候有效, 长度与型号相关; C标识电池单体温感,P表示电池极柱温感,H表示加热片温感,0表示没有链接温感."},
            ],
            type:"A655",
            battconn:"111100001111_111100011011",
            tempconn:"CCCC_CCCC",
        },


        bmus:[
           /* {
                notes:[
                    {section:"type", chs:"BMU类型", comments:'可选:"M600", "M601", "M602", "M603".'},
                    {section:"battconn", chs:"电池连接", comments:"电池连接, 长度与型号相关."},
                    {section:"tempconn", chs:"温感连接", comments:"长度与型号相关; C标识电池单体温感,P表示电池极柱温感,H表示加热片温感,0表示没有链接温感."},
                ],
                type:"M600",
                battconn:"111000111000_111000111000",
                tempconn:"000000_CCCC_CCCC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            },{
                type:"M601",
                battconn:"110000110000_110000110000_110000110000",
                tempconn:"00C000_CC00_CC00_C0CC"
            }*/
        ]
    },

    RuntimeM: {
        powerDownConfirmCount: 10,
        maxTimeForWaitTaskPowerdown: 1,
        wakeupTimeForOTA: 90,
        wakeupTimeForData: "02:00",
        maxOnTimeForOTA: 60*60,
        maxOnTimeForData: 60*5,
        minBatteryVolForWakeup: 10,
        chargeEndOTA: true,
        userIsWakeup: {func:"UserStrategy_Wakeup", inc: "UserStrategy.h"},
        cpInvalidTimeFilter: 10*60,
        powerOffNotifiers: [
            {func:"Soc_PowerDownSaveCbk", inc: "Soc.h"},
            {func:"Statistic_PowerDownSaveCbk", inc: "Statistic.h"},
            {func:"SaveM_PowerDownSaveCbk", inc: "SaveM.h"},
            {func:"UserStrategy_SaftyOff", inc: "UserStrategy.h"},
            {func:"ParameterM_SyncFinished", inc: "ParameterM.h"},
            {func:"ChargeConnectM_SaftyOff", inc: "ChargeConnectM.h"},
        ],

        powerDownConfirmCount_comments: "确认下电信号计数 5ms/次",
        maxTimeForWaitTaskPowerdown_comments: "下电等待的最大时间, 单位秒",
        wakeupTimeForOTA_comments: '收到升级请求之后, 多长时间(什么时刻)唤醒开始升级, 单位秒或者指定时刻:"02:00"',
        wakeupTimeForData_comments: '关机之后, 多长时间唤醒开始向后台发送数据, 单位秒或者指定时刻:"02:00"',
        maxOnTimeForOTA_comments: "唤醒发送数据最长的工作时间, 单位秒",
        maxOnTimeForData_comments: "唤醒升级据最长的工作时间, 单位秒",
        minBatteryVolForWakeup_comments: "唤醒要求的最低供电电压, 单位v",
        chargeEndOTA_comments: "是否支持充电完成之后进行OTA, 针对没有常电的情况, 配置成true之后, 不会唤醒进行数据传输与OTA",
        cpInvalidTimeFilter_comments: "CP唤醒信号无效的过滤时间，单位秒",
        powerOffNotifiers_comments: "下电时需要调用的函数",
    },

    CurrentM:{
        // HALL传感器配置
        hall1: {
            // 通道1
            notes:[
                {section:"model", chs:"传感器型号", comments:'可选: "DHAB_S101_1"(DHAB_S101大量程), "DHAB_S101_2"(DHAB_S101小量程), "DHAB_S124_1"(DHAB_S124大量程), "DHAB_S124_2"(DHAB_S124小量程), DHAB_S118_1"(DHAB_S118大量程), "DHAB_S118_2"(DHAB_S118小量程), DHAB_S133_1"(DHAB_S133大量程 750A), "DHAB_S133_2"(DHAB_S133小量程 75A), DHAB_S137_1"(DHAB_S137大量程 1000A), "DHAB_S137_2"(DHAB_S137小量程 75A), "FS300E2T"(飞轩FS300E2T,300A量程), "FS500E2T"(飞轩FS500E2T,500A量程), "FS600E2T"(飞轩FS600E2T,600A量程), "FS700E2T"(飞轩FS700E2T,700A量程), "FS1200_EK2T"(飞轩FS1200_EK2T,1200A量程), "FS1500_EK4T"(飞轩FS1500_EK4T,1500A量程), "FS2000_EK2T"(飞轩FS2000_EK2T,2000A量程), "WHK30_350DHAB5S2L_1"(启东双赢30/350A大量程), "WHK30_350DHAB5S2L_2"(启东双赢30/350A小量程), "EHMWX911C500"(松下EHMWX911C500,500A量程), "HAH1BVW_S01"(LEM 100A单量程),  "HAH1BVW_S02"(LEM 200A单量程), "HAH1BVW_S03"(LEM 300A单量程), "HAH1BV_S02"(LEM 500A单量程),'
                                                                    +'"HAH1BVW_S08"(LEM 800A单量程), "HAH1BVW_S12"(LEM 1200A单量程), "WHK20BS5S2"(启东双赢 20A单量程), "WHK50BS5S2"(启东双赢 50A单量程), "FS300BT"(飞轩300A单量程), "FS600BT"(飞轩600A单量程), "WHK300EA5S2S"(启东双赢300A单量程), "WHK500EA5S2S"(启东双赢500A单量程), "CS1000BT5"(茶花港联1000A单量程)'},
                {section:"filterNum", chs:"滤波的采集次数"},
                {section:"dropNum", chs:"极值丢弃的个数"},
            ],
            model: "FS300E2T",
            filterNum: 20,
            dropNum: 2,
            zeroFilterValue: 1,
            autoCalibLimit:10,
            isReverse: false,
        },

        hall2: {
            // 通道2
            model: "FS300E2T",
            filterNum: 20,
            dropNum: 2,
            zeroFilterValue: 1,
            autoCalibLimit:10,
            isReverse: false,
        },

        // 分流器配置
        shunt: {
            notes:[
                {section:"model", chs:"传感器型号", comments:'可选:"WSBM8518L1000", "FL2200A25mV", "FL2300A25mV", "FL2300A50mV", "FL2400A50mV", "FL2500A50mV", "FL2600A50mV", "FL2650A50mV", "FL230A75mV", "FL250A75mV", "FL2100A75mV", "FL2150A75mV", "FL2200A75mV", "FL2250A75mV", "FL2300A75mV", "FL2400A75mV", "FL2500A75mV", "FL2600A75mV", "FL2650A75mV", "FL2750A75mV", "FL2800A75mV", "FL21000A75mV", "FL21200A75mV", "FL22000A75mV"'},
            ],
            model: "FL2500A75mV",
            zeroFilterValue: 0.2,
            autoCalibLimit:10,
            isReverse: false,
        },

        // 主回路电流采集
        main: {
            // 可选 "hall1", "hall2", "shunt", "userdef1", "userdef2"
            channel: "shunt", //注：若使用分流器，需要关闭电流的所有上电自检。
            // 可选 "hall1", "hall2", "shunt", "none", "userdef1", "userdef2" 但是不能与channel相同,
            redundant: "none"

        },
        // 加热回路电流采集
        heater: {
            // 可选 "hall1", "hall2", "shunt", "userdef1", "userdef2", "none"
            channel: "none",
            redundant: "none"
        },
    },

    RelayM: [
            {name: "主正", def: "POSITIVE_MAIN", model: "UNKNOWN" , hlss: "HLSS_BCU_HSS1"  , isSafeToOff: "NULL", hv: "HV1", isPositive:true, totalDecPercent: 90, continueDecPercent: 10, checkInterval: 20, checkTimeout: 1000 },
            // {name: "预充", def: "PRECHARGE", model: "UNKNOWN"   },
            // {name: "充电", def: "CHARGE", model: "UNKNOWN"   },
            // {name: "快充正", def: "POSITIVE_DC_CHARGE", model: "UNKNOWN"   },
            {name: "慢充正", def: "POSITIVE_AC_CHARGE", model: "UNKNOWN" , hlss: "HLSS_BCU_HSS2"  , isSafeToOff: "NULL", hv: "HV2", isPositive:true, totalDecPercent: 90, continueDecPercent: 10, checkInterval: 20, checkTimeout: 1000 },
            {name: "自锁", def: "SELF_LOCK", model: "UNKNOWN" , hlss: "HLSS_BCU_HSS4"   },
            {name: "蜂鸣器", def: "BUZZER", model: "UNKNOWN" , hlss: "HLSS_BCU_HSS3"   },
            // {name: "主负", def: "NEGATIVE_MAIN", model: "UNKNOWN"   },
            {name: "加热", def: "HEATER", model: "UNKNOWN" , hlss: "HLSS_BCU_LSS1"  , isSafeToOff: "NULL", hv: "HV3", isPositive:true, totalDecPercent: 90, continueDecPercent: 10, checkInterval: 20, checkTimeout: 1000 },
            // {name: "制冷", def: "COOLER", model: "UNKNOWN"   },
            // {name: "屏供电", def: "HMI", model: "UNKNOWN"   },
		    // {name: "DC", def: "DCDC", model: "UNKNOWN"   },
    ],

    BridgeInsu: {
        notes: [
            {section:"type", chs:"绝缘检测方式", comments:'"OFF", "BY_VOL", "ALWAYS_ON", "ON_WHEN_DETECT"'}
        ],
        type: "ALWAYS_ON",
    },

    CommProtocol: {
        notes: [
            {section:"chg", chs: "充电机通讯协议类型", comments: '"Default", "UDAN", "DongYaoV2.0", "ShiNeng", "UDAN_KaiYuan_DongYaoV1.0", "UDAN_ShiNeng","YiJiaTongV2.0", "QianGaiLi", "BanYiTong_Chg_V1.2", "ADY_V1.1", "CAN2.0C", "TieCheng_Soc", "LongGong_Chg", "ShiNeng_Heat", "XuGong_20190821", "HaoSheng"'},
            {section:"vcu", chs: "VCU通讯协议类型", comments: '"Default", "UDAN", "QianTuo_CAN20A_125K", "QianTuo_CAN20B_250K", "QianTuo_CAN20A_125K_V1.02", "CHOLIFT", "CAN_1232E_201809", "ZOWELL_V2.0", "XianFeng_V3.0", "BaoLi", "BanYiTong_Vcu_V1.2", "RuYi_Vcu", "CAN2.0F", "BaoLi_20200622", "Vcu_250k", "BMS_CAN_3.0", "LongGong_Vcu", "XuGong_20190821", "YuFeng_24V", "YuFeng_80V", "KaTe_20200812", "XuGong_20211120", "Megamette_V1", "HaoSheng", "XuGong_20220309", "XuGong_20220309_Delay"'},
        ],
        chg: "ShiNeng_Heat",
        vcu: "BaoLi_20200622",
    },

    HVProcess: {
        notes: [
            {section:"chg", chs: "高压流程类型", comments: '"DiffPort_Mutex", "DiffPort_Mutex_Heater", "SamePort", "SamePort_Heater", "DiffPort_Mutex_MultiStep_Heater", "DiffPort_Blind", "DiffPort_MultiStep_Heater_Dc", "DiffPort_Dc"'},
            {section:"dchg", chs: "高压流程类型", comments: '"DiffPort_Mutex", "DiffPort_Mutex_Precharge", "SamePort", "SamePort_Precharge", "SamePort_Heater", "SamePort_Blind", "SamePort_Blind_Precharge", "DiffPort_Blind", "DiffPort_Blind_Precharge", "DiffPort_Dc"'},
        ],
        chg: "DiffPort_Mutex_MultiStep_Heater",
        dchg: "DiffPort_Mutex",
    },

    PrechargeM: {
        notes: [
            {section:"enable", chs: "预充功能使能", comments: '"STD_ON", "STD_OFF"'},
            {section:"finishCond", chs: "预充完成条件{}"},
            {section:"retryNum", chs: "重试次数"},
            {section:"timeout", chs: "预充超时时间(ms)"},
            {section:"retryInternal", chs: "预充失败后重试间隔(ms)"},
            {section:"timeMin", chs: "最小预充时间(ms)"},
            {section:"validVoltMin", chs: "最低有效电压"},
            {section:"volChannel", chs: "获取MCU总压接口的HV通道"},
            {section:"continuousTime", chs: "预充完成持续时间(ms)"},
            {section:"relayOnTimeout", chs: "预充继电器闭合超时(ms)"},
            {section:"relayOffDelay", chs: "预充继电器断开延时，用于放电闭合后预充延时断开(ms)"},
            {section:"dchgRelayOnTimeout", chs: "放电继电器闭合超时(ms)"},
            {section:"prechargeRelayChannel", chs: "预充继电器通道号"},
            {section:"dchgRelayChannel", chs: "放电继电器通道号"},
            {section:"currentMax", chs: "预充最大安全电流(0.1A)"},
            {section:"overCurTime", chs: "允许超预充最大电流最大时间(ms)"},
        ],
        enable: "STD_ON",
        finishCond: {
            notes:[
                {section:"type", chs:"预充完成条件类型", comments:'"DELTA_VOLT", "PERCENT", "DELAY"'},
                {section:"value", chs:"预充完成值,若预充类型为延时，此值必须小于“预充超时时间"},
                {section:"current", chs:"完成电流值(0.1A)，输入CURRENT_INVALID_VALUE时表示不检查电流值"},
            ],
            type: "PERCENT",
            value: 0,
            current: "CURRENT_INVALID_VALUE",
        },
        retryNum: 0,
        timeout: 0,
        retryInternal: 1000,
        timeMin: 300,
        validVoltMin: 10,
        volChannel: "HV1",
        continuousTime: 200,
        relayOnTimeout: 5000,
        relayOffDelay: 500,
        dchgRelayOnTimeout: 5000,
        prechargeRelayChannel: "RELAYM_FN_NONE",
        dchgRelayChannel: "RELAYM_FN_POSITIVE_MAIN",
        currentMax: "CURRENT_INVALID_VALUE",
        overCurTime: 1000,
    },

    SocOcvCalib: {
        notes: [
            {section:"enable", chs: "OCV-SOC 使能", comments: '"STD_ON", "STD_OFF"'},
            {section:"temp", chs: "OCV-SOC温度表"},
            {section:"soc", chs: "OCV-SOC soc表"},
            {section:"vol", chs: "OCV-SOC 电压表"},
            {section:"calibCycle", chs: "OCV-SOC校准周期(单位min)"},
            {section:"longCycle", chs: "OCV-SOC校准周期(单位min)"},
            {section:"allowedCurrent", chs: "校准允许最大电流"},
            {section:"powerOnDelay", chs: "上电检查延时时间(单位s)"},
            {section:"checkCycle", chs: "OCV-SOC查询周期(单位s)"},
            {section:"diffSocMin", chs: "允许校准最小SOC差"},
            {section:"calibStep", chs: "SOC校准步长"},
            {section:"linear_calib", chs: "线性校准使能"},
            {section:"isAllowFuncPtr", chs: "是否允许校正函数指针"},
            {section:"temperature_linear", chs: "温度是否线性查表"},
        ],
        enable: "STD_OFF",
        temp: [-40, 125],
        soc: [0, 100],
        vol: [3200, 3200,3200, 3200],
        calibCycle: 720,
        longCycle: 7200,
        allowedCurrent: 3,
        powerOnDelay: 2,
        checkCycle: 1800,
        diffSocMin: 0,
        calibStep: 100,
        linear_calib: "TRUE",
        isAllowFuncPtr: "NULL",
        temperature_linear: "TRUE",
    },

   UserStrategy: {
        notes: [
            {section:"ota_tpye", chs: "OTA升级功能", comments: '"OTA_OFF", "USERSTRATEGY_REALTIME_OTA", "CHARGE_END_OTA", "ON_TIME_OTA"'},
            {section:"buzzer_alarm", chs: "蜂鸣器报警功能"},
            {section:"power_down_control", chs: "电源上下电控制"},
            {section:"manual_power_down", chs: "手动休眠功能"},
            {section:"oc_type", chs: "过流配置类型定义", comments: '"CURRENT", "CURRENT_OFFSET", "PERCENT "'},
            {section:"lv_protect", chs: "放电单体电压低有放电电流时才保护使能"},
            {section:"ltv_protect", chs: "放电总压低有放电电流时才保护使能"},
            {section:"chg_heat_parameters", chs: "充电加热策略相关参数"},
            {section:"chg_user_parameters", chs: "用户自定义充电策略相关参数"},
            {section:"cur_integration", chs: "电流积分时减去其他功耗"},
            {section:"lv_power_down", chs: "放电单体电压低休眠保护"},
        ],
        ota_tpye: "USERSTRATEGY_REALTIME_OTA",
        buzzer_alarm: {
            en: "STD_ON",
            hmi_buzzer_en: "STD_OFF",
            level1_on_time: 2000,
            level1_off_time: 10000,
            level2_on_time: 2000,
            level2_off_time: 2000,
            level3_on_time: 1000,
            level3_off_time: 1000,
        },
        power_down_control: {
            notes: [
                {section:"self_lock", chs: "自锁继电器功能使能,仅配置自锁继电器时有效"},
                {section:"auto_power_down", chs: "自动休眠功能"},
                {section:"manual_power_down", chs: "手动休眠功能"},
                {section:"onboard_key_type", chs:"选择板载自锁继电器类型，仅对A64x系列有效"},
            ],
            self_lock: {
                notes: [
                    {section:"ac_chg_en", chs: "慢充充电自锁继电器使能开关,使能时进入慢充,会自动闭合自锁继电器,禁能时进入慢充状态不会闭合自锁继电器,禁能时如果从自锁闭合状态(如放电自锁继电器使能,\
                    从放电进入充电)进入慢充状态时检测到充电唤醒信号会断开自锁继电器", comments: '"STD_OFF", "STD_ON"'},
                    {section:"dc_chg_en", chs: "快充充电自锁继电器使能开关,使能时进入快充,会自动闭合自锁继电器,禁能时进入快充状态不会闭合自锁继电器,禁能时如果从自锁闭合状态(如放电自锁继电器使能,\
                    从放电进入充电)进入快充状态时检测到充电唤醒信号会断开自锁继电器", comments: '"STD_OFF", "STD_ON"'},
                    {section:"dchg_en", chs: "放电自锁继电器使能开关,使能时进入放电,会自动闭合自锁继电器,禁能时进入放电状态不会闭合自锁继电器,禁能时如果从自锁闭合状态(如充电自锁继电器使能,\
                    从充电进入放电)进入放电状态时检测到放电唤醒信号会断开自锁继电器", comments: '"STD_OFF", "STD_ON"'},
                ],
                ac_chg_en: "STD_ON",
                dc_chg_en: "STD_OFF",
                dchg_en: "STD_ON",
            },
            auto_power_down: {
                en: "STD_ON",
		current: 5,
		time: 3600000,
            },
            manual_power_down: {
                notes: [
                    {section:"en", chs: "手动休眠功能使能开关", comments: '"STD_OFF", "STD_ON"'},
                    {section:"type", chs: "通道", comments: '"DIN1", "DIN2", "SW1", "SW2"'},
                    {section:"delay", chs: "延时时间(ms)"},
                ],
                en: "STD_ON",
		type: "DIN1",
                delay: 3000,
            },
            onboard_key_type: {
                notes: [
                    {section:"mode", chs: "A64x上电方式有两种，一种为自恢复模式，一种为自锁模式", comments: '"KEY_TYPE_IS_SELFLOCK", "KEY_TYPE_IS_SELFRESET"'},
                ],
                mode: "KEY_TYPE_IS_SELFRESET",
            },
        },
        oc_type: {
            ac_chg_oc_type: "CURRENT",
            dc_chg_oc_type: "CURRENT",
            dchg_oc_type: "CURRENT",
            dchg_sp_type: "CURRENT",
            dchg_fb_type: "CURRENT",
        },
        lv_protect: {
            en: "STD_OFF",
            level: "DIAGNOSIS_LEVEL_THIRD",
        },
        ltv_protect: {
            en: "STD_OFF",
            level: "DIAGNOSIS_LEVEL_THIRD",
        },
		chg_heat_parameters: {
            notes: [
                {section: "en", chs: "加热策略使能，配置加热继电器时改参数使能,若配置了加热继电器则默认为'STD_ON',否则为'STD_OFF'", comments: '"STD_ON", "STD_OFF"'},
                {section: "start_heat_temp", chs: "开始加热温度，当最低温度小于等于此温度时由充电转入加热"},
                {section: "stop_heat_temp", chs: "停止加热温度，当最低温度大于等于此温度时，结束加热流程进入纯充电流程"},
                {section: "revert_temp", chs: "回滚温度，用于充电加热不互斥流程中三种状态转换判定，若为充电加热互斥流程（DiffPort_Mutex_Heater）时，该参数默认为0"},
                {section: "start_heat_volt", chs: "充电转加热时请求电压(V)"},
                {section: "start_heat_current", chs: "充电转加热时请求电流(A)"},
                {section: "stop_heat_volt", chs: "加热结束转入充电时请求电压(V),若为请求系统总压该参数需配置为0,目前该参数默认为0"},
                {section: "stop_heat_current", chs: "加热结束转入充电是请求电流"},
                {section: "current_flag", chs: "加热电流是否经过分流器，若经过为TRUE，否则为FALSE"},
            ],
            en: "STD_ON",
            start_heat_temp: 0,
            stop_heat_temp: 14,
            revert_temp: 1,
            start_heat_volt: 51.2,
            start_heat_current: 18,
            stop_heat_volt: 0,
            stop_heat_current: 0,
            current_flag: "FALSE",
        },
        chg_user_parameters: {
            notes: [
                {section: "en", chs: "用户自定义充电策略使能，若配置为使能则启用用户自定义充电策略", comments: '"STD_ON", "STD_OFF"'},
                {section: "low_temp_start", chs: "低温区间起始温度(℃), low_temp_start < 低温区间 <= low_temp_stop"},
                {section: "low_temp_stop", chs: "低温区间结束温度"},
                {section: "low_temp_chg_current", chs: "低温区间短时充电电流(A)"},
                {section: "low_temp_chg_time", chs: "低温区间小电流充电时长(min)"},
                {section: "full_chg_volt", chs: "满充电压(mv)"},
                {section: "chg_end_current", chs: "满充后充电电流(A)"},
                {section: "chg_end_time", chs: "满充后小电流充电时长(min)，满充后达到小电流充电时长时停止充电"},
                {section: "chg_stop_current", chs: "满充后停止充电电流(A)，满充后当回路中电流小于满充后停止充电电流时停止充电"},
                {section: "chg_stop_volt", chs: "满充后停止充电最高电压(mv)，满充后进行小电流充电时若达到最高电压则停止充电"},
            ],
            en: "STD_OFF",
            low_temp_start: 0,
            low_temp_stop: 5,
            low_temp_chg_current: 20,
            low_temp_chg_time: 5,
            full_chg_volt: 3600,
            chg_end_current: 20,
            chg_end_time: 8,
            chg_stop_current: 15,
            chg_stop_volt: 3700,
        },
        cur_integration: {
            notes: [
                {section: "en", chs: "用户自定义电流积分策略使能，若配置为使能则启用用户自定义电流积分策略", comments: '"STD_ON", "STDOFF"'},
                {section: "current", chs: "电流大小"},
            ],
            en: "STD_OFF",
            current: 1.0,
        },
        lv_power_down: {
            notes: [
                {section: "en", chs: "用户自定义单体电压低休眠策略使能，若配置为使能则启用用户自定义电压低休眠策略", comments: '"STD_ON", "STD_OFF"'},
                {section: "lv", chs: "电压低阈值,单位mv"},
                {section: "time", chs: "低电压休眠持续时间,单位ms"},
            ],
            en: "STD_OFF",
            lv: 2800,
            time: 60000,
        }
    },

    FullCharge: {
        notes: [
            {section:"Flag", chs: "充电降流末端检查标志"},
            {section:"Delay", chs: "满充延时"},
            {section:"RelDelay", chs: "满充释放延时"},
            {section:"Volt", chs: "满充单体电压"},
            {section:"fullHook", chs: "满充钩子函数"},
            {section:"fullRelHook", chs: "满充释放钩子函数"},
        ],
        Flag: "TRUE",
        Delay: 5000,
        RelDelay: 5000,
        Volt: "3600",
        fullHook: "UserStrategy_FullChargeHook",
        fullRelHook: "UserStrategy_FullChargeReleaseHook",
    },

    PowerM: {
        notes: [
            {section:"dchgcontinues", chs: "持续放电电流表"},
            {section:"dchgpeak", chs: "脉冲放电电流表"},
            {section:"dchgfb", chs: "脉冲充电电流表"},
            {section:"chgcontinues", chs: "持续充电电流表"},
            {section:"dcchg", chs: "快充充电电流表"},
            {section:"acchg", chs: "慢充充电电流表"},
            {section:"heater", chs: "加热电流表"},
            {section:"extern", chs: "扩展电流最大允许功率表（无特殊说明，所有温度点必须有回滚）"},
        ],
        dchgcontinues: {
            notes:[
                {section:"soc", chs:"soc列表(x轴)"},
                {section:"temp", chs:"温度列表(y轴)"},
                {section:"tempAlt", chs:"温度回滚列表(y轴)"},
                {section:"current", chs:"电流表"},
                {section:"diag", chs:"诊断故障功率百分比表", comments:'type: "DCHG_LV", "DCHG_HV", "DCHG_HT", "DCHG_LT".....,'},
                {section:"decrease", chs:"末端降流表", comments:'type: "TO_PERCENT", "TO_LAST_PERCENT", "CONSTVAL", "TO_CONSTVAL", "TO_TABLE_PERCENT",'},  //decrease需要则定义,不需要不定义
            ],
            soc: [ 104 ],
            temp: [ 60 ],
            tempAlt: [ 58 ],
            current: [ 440 ],
            diag: [
                {type: "DCHG_DT", percent: [100, 100, 0, 100]},
                {type: "DCHG_DV", percent: [100, 100, 0, 100]},
                {type: "DCHG_HT", percent: [100, 100, 0, 100]},
                {type: "DCHG_HTV", percent: [100, 100, 0, 100]},
                {type: "DCHG_HV", percent: [100, 100, 0, 100]},
                {type: "DCHG_LT", percent: [100, 100, 0, 100]},
                {type: "DCHG_LTV", percent: [100, 100, 0, 0]},
                {type: "DCHG_LV", percent: [100, 100, 0, 0]},
                {type: "DCHG_OC", percent: [100, 100, 0, 100]},
                {type: "FB_OC", percent: [100, 100, 0, 100]},
                {type: "RELAY_ABNORMAL", percent: [100, 100, 0, 100]},
                {type: "SP_OC", percent: [100, 100, 0, 100]},
                {type: "TEMP_LINE", percent: [100, 100, 0, 100]},
                {type: "VOLT_LINE", percent: [100, 100, 0, 100]},

            ],
            config: {
                notes:[
                    {section:"yUseWhichValue", chs:"在yAxis和yAltAxis查询到的值取怎样取值，POWERM_YAXIS_USE_XX"},
                    {section:"yAxisIncUseAlt", chs:"在y轴增加时使用Alt表"},
                    {section:"xAxisSupportDir", chs:"X轴支持变动方向"},
                    {section:"xAxisIsLinear", chs:"X轴区间线性插值"},
                    {section:"increaseTime", chs:"功率输出时增加斜率时间参数"},
                    {section:"increaseStep", chs:"功率输出时增加斜率步进参数"},
                    {section:"decreaseTime", chs:"功率输出时减小斜率步进参数"},
                    {section:"decreaseStep", chs:"功率输出时减小斜率步进参数"},
                ],
                yUseWhichValue: "POWERM_YAXIS_USE_MIN",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_L2R",
                xAxisIsLinear: 0,
                increaseTime: 500,
                increaseStep: 50,
                decreaseTime: 500,
                decreaseStep: 10,
            },
        },
        dchgpeak: {
            soc: [ 104 ],
            temp: [ 60 ],
            tempAlt: [ 58 ],
            current: [ 880 ],
            diag: [
                {type: "DCHG_DT", percent: [100, 100, 0, 100]},
                {type: "DCHG_DV", percent: [100, 100, 0, 100]},
                {type: "DCHG_HT", percent: [100, 100, 0, 100]},
                {type: "DCHG_HTV", percent: [100, 100, 0, 100]},
                {type: "DCHG_HV", percent: [100, 100, 0, 100]},
                {type: "DCHG_LT", percent: [100, 100, 0, 100]},
                {type: "DCHG_LTV", percent: [100, 100, 0, 0]},
                {type: "DCHG_LV", percent: [100, 100, 0, 0]},
                {type: "DCHG_OC", percent: [100, 100, 0, 100]},
                {type: "FB_OC", percent: [100, 100, 0, 100]},
                {type: "RELAY_ABNORMAL", percent: [100, 100, 0, 100]},
                {type: "SP_OC", percent: [100, 100, 0, 100]},
                {type: "TEMP_LINE", percent: [100, 100, 0, 100]},
                {type: "VOLT_LINE", percent: [100, 100, 0, 100]},

            ],
            config: {
                yUseWhichValue: "POWERM_YAXIS_USE_MIN",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_L2R",
                xAxisIsLinear: 0,
                increaseTime: 500,
                increaseStep: 50,
                decreaseTime: 500,
                decreaseStep: 10,
            },
        },
        dchgfb: {
            soc: [ 104 ],
            temp: [ 60 ],
            tempAlt: [ 58 ],
            current: [ 440 ],
            diag: [

            ],
            config: {
                yUseWhichValue: "POWERM_YAXIS_USE_MIN",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_L2R",
                xAxisIsLinear: 0,
                increaseTime: 500,
                increaseStep: 50,
                decreaseTime: 500,
                decreaseStep: 10,
            },
        },
        chgcontinues: {
            soc: [ 104 ],
            temp: [ 60 ],
            tempAlt: [ 58 ],
            current: [ 440 ],
            diag: [

            ],
            config: {
                yUseWhichValue: "POWERM_YAXIS_USE_MIN",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_L2R",
                xAxisIsLinear: 0,
                increaseTime: 1000,
                increaseStep: 20,
                decreaseTime: 1000,
                decreaseStep: 10,
            },
        },
        dcchg: {
            soc: [ 104 ],
            temp: [ 60 ],
            tempAlt: [ 58 ],
            current: [ 440 ],
            diag: [
                {type: "CHG_DT", percent: [100, 50, 0, 100]},
                {type: "CHG_DV", percent: [100, 50, 0, 100]},
                {type: "CHG_HT", percent: [100, 100, 0, 100]},
                {type: "CHG_HTV", percent: [100, 100, 100, 0]},
                {type: "CHG_HV", percent: [100, 100, 100, 0]},
                {type: "CHG_LTV", percent: [100, 50, 0, 100]},
                {type: "CHG_LV", percent: [100, 50, 0, 100]},
                {type: "DOUBLE_CHG_CONNECT_FAULT", percent: [100, 100, 0, 100]},
                {type: "RELAY_ABNORMAL", percent: [100, 100, 0, 100]},
                {type: "TEMP_LINE", percent: [100, 100, 0, 100]},
                {type: "VOLT_LINE", percent: [100, 100, 0, 100]},

            ],
            config: {
                yUseWhichValue: "POWERM_YAXIS_USE_MIN",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_L2R",
                xAxisIsLinear: 0,
                increaseTime: 1000,
                increaseStep: 20,
                decreaseTime: 1000,
                decreaseStep: 10,
            },
            notes:[
                {section:"startval", chs:"进入这个阶段的值"},
                {section:"delaytime", chs:"这个阶段最小维持的时间"},
                {section:"type", chs:"降流方式:TO_PERCENT, TO_CONSTVAL"},
                {section:"param", chs:"降流参数，与type相关"},
                {section:"minval", chs:"该阶段最小值"},
            ],
            decrease: [

            ],
        },
        acchg: {
            soc: [100],
            temp: [0, 5, 10, 15, 55, 65],
            tempAlt: [-2, 3, 8, 13, 53, 63],
            current: [0.0, 60.0, 120.0, 160.0, 200.0, 90.0],
            diag: [
                {type: "AC_CHG_OC", percent: [100, 100, 0, 100]},
                {type: "CHG_DT", percent: [100, 50, 0, 100]},
                {type: "CHG_DV", percent: [100, 50, 0, 100]},
                {type: "CHG_HT", percent: [100, 100, 0, 100]},
                {type: "CHG_HTV", percent: [100, 100, 100, 0]},
                {type: "CHG_HV", percent: [100, 100, 100, 0]},
                {type: "CHG_LTV", percent: [100, 50, 0, 100]},
                {type: "CHG_LV", percent: [100, 50, 0, 100]},
                {type: "DOUBLE_CHG_CONNECT_FAULT", percent: [100, 100, 0, 100]},
                {type: "RELAY_ABNORMAL", percent: [100, 100, 0, 100]},
                {type: "TEMP_LINE", percent: [100, 100, 0, 100]},
                {type: "VOLT_LINE", percent: [100, 100, 0, 100]},

            ],
            config: {
                yUseWhichValue: "POWERM_YAXIS_USE_MIN",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_L2R",
                xAxisIsLinear: 0,
                increaseTime: 1000,
                increaseStep: 20,
                decreaseTime: 1000,
                decreaseStep: 10,
            },
            decrease: [
                                {startval: 3550, delaytime: 5000, type: "TO_CONSTVAL", param: 135, minval: 5,},
                {startval: 3580, delaytime: 5000, type: "TO_CONSTVAL", param: 30, minval: 5,},
                {startval: 3600, delaytime: 5000, type: "TO_CONSTVAL", param: 0, minval: 30,},

            ],
        },
        heater: {
            soc: [104],
            temp: [-10, 0, 15],
            tempAlt: [-12, -2, 13],
            current: [20, 10, 5],
            diag: [
                {type: "CHG_HEAT_OC", percent: [0, 0, 0, 0]},
            ],
            config: {
                yUseWhichValue: "POWERM_YAXIS_USE_LT",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_BDIR",
                xAxisIsLinear: 0,
                increaseTime: 500,
                increaseStep: 10,
                decreaseTime: 500,
                decreaseStep: 10,
            },
        },
        extern: {
            soc: [104],
            temp: [-20, 60],
            tempAlt: [-22, 58],
            current: [0, 300],
            diag: [
                {type: "DCHG_LV", percent: [60, 50, 10, 0]},
            ],
            config: {
                yUseWhichValue: "POWERM_YAXIS_USE_MIN",
                yAxisIncUseAlt: 0,
                xAxisSupportDir: "POWERM_XAXIS_SUPPORT_BDIR",
                xAxisIsLinear: 0,
                increaseTime: 500,
                increaseStep: 200,
                decreaseTime: 500,
                decreaseStep: 20,
            },
        },
    },

    Charge: {
        dc: {
            notes:[
                {section:"wakeup", chs:"唤醒信号", comments:'可选:"NONE", "ON", "DCC"'},
                {section:"protocol", chs:"协议", comments:'可选:"NONE", "USER", "GB2015"'},
                {section:"mode", chs:"连接模式", comments:'可选:"NONE", "USER", "COMM", "DIN", "CURRENT", "GB_DC"'},
                {section:"dintype", chs:"数字输入类型", comments:'只有mode为"DIN"时可用, 可选:"LEVEL", "SW", "WAKEUP_SIGNAL"'},
                {section:"dinChannel", chs:"数字输入通道", comments:'可选:"BCU_DIN1", "BCU_DIN2", "BCU_SW1", "BCU_SW2", "OBC", "DCC"'},
                {section:"userModeFunction", chs:"自定义充电连接", comments:'当mode为"USER"时，提供一个获取连接模式的头文件和函数'}
            ],
            wakeup: "NONE",
            protocol: "NONE",
            mode: "NONE",
            dintype: "NONE",
            dinChannel: "NONE",
            relay: "慢充正",
            userModeFunction: ["UserStrategy.h", "UserStrategy_DCConnected"],
        },

        ac: {
            notes:[
                {section:"wakeup", chs:"唤醒信号", comments:'可选:"NONE", "ON", "CP", "OBC"'},
                {section:"protocol", chs:"协议", comments:'可选:"NONE", "USER", "GB2015"'},
                {section:"mode", chs:"连接模式", comments:'可选:"NONE", "USER", COMM", "DIN", "CURRENT", "GB_MODE1_CONNECT_B", "GB_MODE2_CONNECT_B", "GB_MODE3_CONNECT_A", "GB_MODE3_CONNECT_B", "GB_MODE3_CONNECT_C"'},
                {section:"dintype", chs:"数字输入类型", comments:'只有mode为"DIN"时可用, 可选:"LEVEL", "SW", "WAKEUP_SIGNAL"'},
                {section:"dinChannel", chs:"数字输入通道", comments:'可选:"BCU_DIN1", "BCU_DIN2", "BCU_SW1", "BCU_SW2", "OBC", "DCC"'},
                {section:"userModeFunction", chs:"自定义充电连接", comments:'当mode为"USER"时，提供一个获取连接模式的函数'}
            ],
            wakeup: "OBC",
            protocol: "USER",
            mode: "COMM",
            dintype: "NONE",
            dinChannel: "NONE",
            relay: "慢充正",
            userModeFunction: ["UserStrategy.h", "UserStrategy_ACConnected"],
        },

        CurConnectPara: {
            notes:[
                {comments:'仅当"dc"或者"ac"连接"mode"配置为"CURRENT"时有效,dc与ac只允许一个连接"mode"为"CURRENT"'},
                {section:"cur_min", chs:"进入充电电流(A)"},
                {section:"rel_cur_min", chs:"退出充电电流(A)"},
                {section:"connect_delay", chs:"进入充电电流条件持续时间(ms)"},
                {section:"unconnect_delay", chs:"退出充电电流条件持续时间(ms)"},
            ],
            cur_min: 0,
            rel_cur_min: 0,
            connect_delay: 0,
            unconnect_delay: 0,
        },

        el: {
            enable: 0,
            driveType: "HBridge",
            driveType_comments: '驱动模式, 可选:"HBridge", "LowLevel", "HighLevel"',
            lockTime: 1,
            lockTime_comments: "锁止操作时间, 单位秒",
            unlockTime: 1,
            unlockTime_comments: "解锁操作时间, 单位秒",
            lockFeedback:['EL_FEEDBACK', 'HIGH'],
            lockFeedback_comments: "锁止时检查的DIO及状态",
            unlockFeedback:['EL_FEEDBACK', 'LOW'],
            unlockFeedback_comments: "解锁时检查的DIO及状态",
            ctlMax: 3,
            ctlMax_comments: "电子锁驱动最大驱动次数，大于1时驱动失败后会自动重试",
        }
    },

    Ntc: {
        //可选 "Shiheng_CWF4B_103F_3435B":时恒10K; "Soarwhale_10K3435B":由甲申田10K; "Kemit_CWF110KF3435":科敏10K; "Kemit_CWF110KF4150":科敏10K; Yonggui_YG691_51_00_50":永贵10K; "Shiheng_CWF4B_104F_3950":时恒100K; "MTG2_420F103H":MTG2_10K; "Kemit_CWF110KF3950":科敏10K; "BoDian_FW10K3950":铂电10K; "Soarwhale_SW_NTC_C4_103B1A1":由甲申田10K;
        //     "Shiheng_CWF4B_103F_3950":时恒10K; "Microtherm_TR29_NTC_ALW600_D3_2L15":麦柯泰姆10K; "Microtherm_RT29_NTC_ALW500_D3_2L15":麦柯泰姆100K-3950; "YiDa_BLMF103F3435F":苏州怡达10K; "CATL_10K_SMD":CATL 10K; "XLB_SK103F4R0300ST": 新立本10K; "Soarwhale_SW_NTC_104F_3590": 由甲申田100K; "Semitec_104KT1608_1P": Semitec100K; "TPS2_395F103F_150"：特普生10K；
        //     "Semitec_103KT1005_1P": Semitec10K; "Unknown1_10KRT13950": Unknown1 10K; "Karmo_KN1MF12140410_L400": 科润美10K; "Unknown2_10K_3950": Unknown2 10K; "Kemit_CWF1100KF4150": 科敏100K; "Thinking_NTSA3104FV057": 兴勤100K; "Ampron_APR_CWF103F3950FA250A": 安培龙10K;
        //     "PT1000_10K": PT1000, 10K下拉电阻分压; "PT1000_1K": PT1000, 1K下拉电阻分压; "HuaiDian_HA103F4R22": 淮电10K; "Kemit_CWF110KF3950_369ohm":科敏10K; "Kedite_MF5_4_103F3435F100MM": 科帝特10K; "WeiLian_TR29N313A4EL50": 上海维连100K; "Kedite_103F3950F100C": 科帝特10K;

        cellTempType: "HuaiDian_HA103F4R22", // 电池温感型号
        dcChgSckTempType:'Shiheng_CWF4B_103F_3435B', // 快充充电插座温感信号
        acChgSckTempType:'Shiheng_CWF4B_103F_3435B', // 慢充充电插座温感信号
        chgSckTempMap: {
            //SCH_T1,SCH_T2,SCH_T3,FCH_T1,FCH_T2,OFF
            AC_AL: 'OFF',
            AC_BN: 'OFF',
            AC_C: 'OFF',
            DC_POSITIVE: 'OFF',
            DC_NEGATIVE: 'OFF',
        }
    },

    BalanceM: {
        Charging: {
            enable: 1, // 是否使能
            startVoltage: 3500, // 开启电压
            minVoltage: 2700, // 开启最小电压
            minDiffVoltage: 10, // 开启的最小压差
            maxDiffVoltage: 800, // 开启的最大压差
        },

        Static: {
            enable: 0, // 是否使能
            startVoltage: 3500, // 开启电压
            minVoltage: 2700, // 开启最小电压
            minDiffVoltage: 10, // 开启的最小压差
            maxDiffVoltage: 800, // 开启的最大压差
        },
        Remote: {
            enable: 1, // 是否使能
            condition:{inc:"UserStrategy.h", func:"UserStrategy_RemoteBalanceConditionCheck"},
            enableDefaultBalanceTime:12*3600,//连续超过该时间没有接收到远程均衡指令，就打开默认均衡，单位S
        }
    },

    PwmCapture: {
        funcIncude: ['UserStrategy.h'],
        CP: {
            enable: true,
            risingFunc: '',
            fallingFunc: '',
        },

        SIGIN1: {
            enable: false, // 是否开启SIGIN1的占空比检测
            risingFunc: 'UserStrategy_CheckSRS', // 检测到上升沿调用的函数
            fallingFunc: '',  // 检测到下降沿调用的函数，如果与上升沿同一个函数，在函数里面可以判断沿
        },

        SIGIN2: {
            enable: false,
            risingFunc: '',
            fallingFunc: '',
        } // 请参考SIGIN2的注释
    },

    Diagnosis: {
        levelsupport: 4,
        configs: [
            {
                name: "充电单体电压高",
                levels: [4],
                cycle:100,
                //levelParaGetFunc: "NULL",
                //report_levels: [],
                events:[
                    {levels:[4,], event: "assert", action: "SocDiagCalib_FullCalibCbk"},
                    {levels:[4,], event: "deassert", action: "SocDiagCalib_FullCalibRelCbk"},
		    {levels:[4,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[4,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 4,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "放电单体电压高",
                levels: [2, 3],
                cycle:100,
                events:[


			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电单体电压低",
                levels: [1, 2, 3],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "放电单体电压低",
                levels: [1, 2, 3, 4],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "SocDiagCalib_EmptyCalibCbk"},
                    {levels:[3,], event: "deassert", action: "SocDiagCalib_EmptyCalibRelCbk"},


		    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[3,], event: "deassert", action: "UserStrategy_DischargeM_DiagnosisCtlEnableEventCbk"},
                    {levels:[4,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[4,], event: "deassert", action: "UserStrategy_DischargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 4,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电总压高",
                levels: [4],
                cycle:100,
                events:[
                    {levels:[4,], event: "assert", action: "SocDiagCalib_FullCalibCbk"},
                    {levels:[4,], event: "deassert", action: "SocDiagCalib_FullCalibRelCbk"},
		    {levels:[4,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[4,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 4,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "放电总压高",
                levels: [2, 3],
                cycle:100,
                events:[


			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电总压低",
                levels: [1, 2, 3],
                cycle:100,
                events:[
			{levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "放电总压低",
                levels: [1, 2, 3, 4],
                cycle:100,
                events:[


			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[3,], event: "deassert", action: "UserStrategy_DischargeM_DiagnosisCtlEnableEventCbk"},
                    {levels:[4,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[4,], event: "deassert", action: "UserStrategy_DischargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 4,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电单体压差",
                levels: [1, 2, 3],
                cycle:100,
                events:[
			{levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "放电单体压差",
                levels: [3],
                cycle:100,
                events:[


			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电总压压差",
                levels: [],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "放电总压压差",
                levels: [],
                cycle:100,
            }, {
                name: "充电高温",
                levels: [2, 3],
                cycle:100,
                events:[
			{levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "放电高温",
                levels: [2, 3],
                cycle:100,
                events:[


			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
            name: "充电低温",
                levels: [3],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "放电低温",
                levels: [3],
                cycle:100,
                events:[


			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电温差",
                levels: [2, 3],
                cycle:100,
                events:[
			{levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "放电温差",
                levels: [2, 3],
                cycle:100,
                events:[


			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[


			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "快充过流",
                levels: [],
                cycle:100,
                levelParaGetFunc: "UserStrategy_ChgDcOverCurrentParaGet",
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "慢充过流",
                levels: [1, 2, 3],
                cycle:100,
                levelParaGetFunc: "UserStrategy_ChgAcOverCurrentParaGet",
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
            }, {
                name: "回馈过流",
                levels: [3],
                cycle:100,
                levelParaGetFunc: "UserStrategy_DchgFeedBackCurrentParaGet",
                events:[


                    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[3,], event: "deassert", action: "UserStrategy_DischargeM_DiagnosisCtlEnableEventCbk"},


                ],
            }, {
                name: "持续放电过流",
                levels: [2, 3],
                cycle:100,
                levelParaGetFunc: "UserStrategy_DchgOverCurrentParaGet",
                events:[


		    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
            }, {
                name: "瞬时放电过流",
                levels: [2, 3],
                cycle:100,
                levelParaGetFunc: "UserStrategy_DchgSpOverCurrentParaGet",
                events:[


                    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
            }, {
                name: "充电电流异常",
                levels: [],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    // {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
            }, {
                name: "SOC过高",
                levels: [],
                cycle:100,
            }, {
                name: "SOC过低",
                levels: [1, 2, 3],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "绝缘漏电",
                levels: [2, 3],
                cycle:100,
                events:[




                ],
            }, {
                name: "电压排线脱落",
                levels: [2, 3],
                cycle:100,
                events:[
			{levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "温感排线脱落",
                levels: [2, 3],
                cycle:100,
                events:[
			{levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
			{levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
			{levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "内网通信",
                levels: [],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "快充正极高温",
                levels: [],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "快充负极高温",
                levels: [],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "慢充AL极高温",
                levels: [],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "慢充BN极高温",
                levels: [],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "慢充C极高温",
                levels: [],
                cycle:100,
                events:[
                    {levels:[2,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 2,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电加热高温",
                levels: [],
                cycle:100,
            }, {
                name: "放电加热高温",
                levels: [],
                cycle:100,
            }, {
                name: "充电加热温差",
                levels: [],
                cycle:100,
            }, {
                name: "放电加热温差",
                levels: [],
                cycle:100,
            }, {
                name: "充电加热超时",
                levels: [],
                cycle:100,
            }, {
                name: "放电加热超时",
                levels: [],
                cycle:100,
            }, {
                name: "充电加热过流",
                levels: [],
                cycle:100,
                events:[
                    {levels:[1,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
            }, {
                name: "放电加热过流",
                levels: [],
                cycle:100,
                events:[
                    {levels:[1,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
            }, {
                name: "SOC跳变",
                levels: [1],
                cycle:100,
            }, {
                name: "供电电压过低",
                levels: [1, 2, 3, 4],
                cycle:500,
                events:[




                    //{levels:[4,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    //{levels:[2,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                    //{levels:[4,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    //{levels:[2,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                    //{levels:[4,], event: "assert", action: "RuntimeM_LowPowerSupply"},
                ],
            }, {
                name: "供电电压过高",
                levels: [],
                cycle:500,
            }, {
                name: "供电电压异常",
                levels: [],
                cycle:500,
            }, {
                name: "充电极柱高温",
                levels: [],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "放电极柱高温",
                levels: [],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电极柱温差",
                levels: [],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "放电极柱温差",
                levels: [],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "充电机通信",
                levels: [],
                cycle:100,
                events:[




                ],
            }, {
                name: "整车通信",
                levels: [],
                cycle:100,
            }, {
                name: "满充",
                levels: [],
                cycle:100,
            }, {
                name: "充电插座温感异常",
                levels: [],
                cycle:100,
                events:[




                ],
            }, {
                name: "预充失败",
                levels: [],
                cycle:100,
                events:[




                ],
            }, {
                name: "电流异常",
                levels: [3],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "BMS初始化故障",
                levels: [3],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 3000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                        {event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "HVIL故障",
                levels: [],
                cycle:50,
                events:[
                    {levels:[1,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                    {levels:[1,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 1,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                        {event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "继电器故障",
                levels: [3],
                cycle:50,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
		    {levels:[3,], event: "assert", action: "DischargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[3,], event: "deassert", action: "DischargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
			{event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "加热故障",
                levels: [],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 1,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                        {event: "assert", action: "DischargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "DischargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "CC2连接故障",
                levels: [],
                cycle:100,
            }, {
                name: "CC连接故障",
                levels: [],
                cycle:100,
            }, {
                name: "CP连接故障",
                levels: [],
                cycle:100,
            }, {
                name: "加热温感异常",
                levels: [],
                cycle:100,
                events:[




                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[




                    ],
                },
            }, {
                name: "极柱温感异常",
                levels: [],
                cycle:100,
                events:[
                    {levels:[1,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 1,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "电子锁故障",
                levels: [],
                cycle:100,
                events:[
                    {levels:[1,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
                    // {levels:[1,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},
                ],
                startDiagnosis: {
                    timeout: 2000,
                    level: 1,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        // {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},
                    ],
                },
            }, {
                name: "多充电连接故障",
                levels: [3],
                cycle:100,
                events:[
                    {levels:[3,], event: "assert", action: "ChargeM_DiagnosisCtlDisableEventCbk"},
		    {levels:[3,], event: "deassert", action: "ChargeM_DiagnosisCtlEnableEventCbk"},


                ],
                startDiagnosis: {
                    timeout: 1000,
                    level: 3,
                    events:[
                        {event: "assert", action: "ChargeM_StartDiagCtlDisableEventCbk"},
                        {event: "deassert", action: "ChargeM_StartDiagCtlEnableEventCbk"},


                    ],
                },
            }, {
                name: "电池数不匹配",
                levels: [],
                cycle:100,
            }, {
                name: "温感数不匹配",
                levels: [],
                cycle:100,
            }, {
                name: "碰撞故障",
                levels: [],
                cycle:50,
            }, {
                name: "热失控",
                levels: [],
                cycle:100,
            },
        ],
    },

    ParameterM: {
        Diagnosis: [
            {
                name: "电压排线脱落",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 30],
                    [1, 1, 5, 30],
                    [1, 0, 30, 1]
                ]
            }, {
                name: "温感排线脱落",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 30],
                    [1, 1, 5, 30],
                    [1, 0, 30, 1]
                ]
            }, {
                name: "内网通信",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "快充正极高温",
                levels_comment: "配置格式 [触发阈值(℃), 释放阈值(℃), 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "快充负极高温",
                levels_comment: "配置格式 [触发阈值(℃), 释放阈值(℃), 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "慢充AL极高温",
                levels_comment: "配置格式 [触发阈值(℃), 释放阈值(℃), 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "慢充BN极高温",
                levels_comment: "配置格式 [触发阈值(℃), 释放阈值(℃), 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "慢充C极高温",
                levels_comment: "配置格式 [触发阈值(℃), 释放阈值(℃), 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [70, 60, 5.0, 5.0],
                    [80, 70, 5.0, 5.0],
                    [90, 80, 5.0, 5.0],
                    [100, 90, 5.0, 5.0],
                ]
            }, {
                name: "充电机通信",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "整车通信",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "满充",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 0, 1]
                ]
            }, {
                name: "充电插座温感异常",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "预充失败",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "电流异常",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 1],
                    [1, 1, 5, 1],
                    [1, 1, 5, 1]
                ]
            }, {
                name: "BMS初始化故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 2, 5],
                    [1, 1, 2, 5],
                    [1, 1, 2, 5]
                ]
            }, {
                name: "HVIL故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 0.1, 5]
                ]
            }, {
                name: "继电器故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 0, 1, 1],
                    [1, 0, 1, 1],
                    [1, 0, 1, 1]
                ]
            }, {
                name: "加热故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "CC2连接故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "CC连接故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "CP连接故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "加热温感异常",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[

                ]
            }, {
                name: "极柱温感异常",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "电子锁故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "多充电连接故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5],
                    [1, 1, 5, 5],
                    [1, 1, 5, 5]
                ]
            }, {
                name: "电池数不匹配",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "温感数不匹配",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "供电电压异常",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 1, 5, 5]
                ]
            }, {
                name: "碰撞故障",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 0, 0, 5],
                    [1, 0, 0, 5],
                    [1, 0, 0, 5],
                    [1, 0, 0, 5],
                ]
            }, {
                name: "热失控",
                levels_comment: "配置格式 [1, 1, 触发持续时间(s), 释放持续时间(s)]",
                levels:[
                    [1, 0, 0.5, 5],
                    [1, 0, 0.5, 5],
                    [1, 0, 0.5, 5],
                    [1, 0, 0.5, 5],
                ]
            }
        ]
    },

    CanDrv:[
        {
            notes:[
                {section:"hw", chs:"硬件单元", comments:"硬件Can控制器单元."},
                {section:"baud", chs:"波特率", comments:"125000,250000,500000,1000000."},
                {section:"bor_time", chs:"busoff 自恢复时间", comments:"0:autorecovery, 单位 ms"},
            ],
            hw:0, baud:250000,
        },

        {hw:1, baud: 250000},
        {hw:2, baud: 250000},
        {hw:3, baud: 250000},
    ],



    /* CanIf层可以接收的Id列表, 从机的Id会自动根据从机个数添加, 这里不需要配置 */
    CanIf:{
        notes:[
            {section:"rxsduids", chs:"接收SDUID配置", comments:'上层使用CanIf接收的SDUID配置'},
            {section:"txsduids", chs:"发送SDUID配置", comments:'上层使用CanIf发送的SDUID配置'},
        ],
        rxsduids:[
            {
                notes:[
                    {section:"name", chs:"SDUID名称", comments:'该值会传给上层的RxIndication'},
                    {section:"to", chs:"上层模块名称", comments:'假设定义成"XXX", 接收到数据时调用XXX_RxIndication'},
                    {section:"id", chs:"CanId", comments:'接收指定的CanId'},
                    {section:"hw", chs:"硬件Can控制器", comments:'接收使用的硬件Can控制器'},
                    {section:"define", chs:"SDUID的值", comments:"该值会传给上层的RxIndication"},

                ],
                name:"CANIF_RX_CANTERMIO",
                to:"CanTermio",
                id:0x7fe,
                hw:0,
                define:0,
            },
            {name:"CANIF_RX_RUNTIMEM", to:"RuntimeM", id:0x7fd, hw:0, define:0},
            {name:"CANIF_RX_CANECHO_0", to:"CanEcho", id:0x7f0, hw:1, define:0},
            {name:"CANIF_RX_CANECHO_1", to:"CanEcho", id:0x7f1, hw:2, define:1},
            {name:"CANIF_RX_CANECHO_2", to:"CanEcho", id:0x7f2, hw:3, define:2},
            //{name:"CANIF_RX_USERCURRENTSENSOR_1", to:"UserCurrentSensor", id:0x3c0, hw:3, define:0},
            //{name:"CANIF_RX_USERCURRENTSENSOR_2", to:"UserCurrentSensor", id:0x3c1, hw:3, define:1},
        ],
        txsduids:[
            {
                notes:[
                    {section:"name", chs:"SDUID名称", comments:'该值用于上层传给CanIf_Transmit'},
                    {section:"to", chs:"上层模块名称", comments:'假设定义成"XXX", 接收到数据时调用XXX_TxComfirmation'},
                    {section:"id", chs:"CanId", comments:'接收指定的CanId'},
                    {section:"hw", chs:"硬件Can控制器", comments:'接收使用的硬件Can控制器'},
                ],
                name:"CANIF_TX_CANTERMIO",
                from:"CanTermio",
                id:0x7fe,
                hw:0
            },
            {name:"CANIF_TX_RUNTIMEM", from:"RuntimeM", id:0x7fd, hw:0},
            {name:"CANIF_TX_CANECHO_0", from:"CanEcho", id:0x7f0, hw:1},
            {name:"CANIF_TX_CANECHO_1", from:"CanEcho", id:0x7f1, hw:2},
            {name:"CANIF_TX_CANECHO_2", from:"CanEcho", id:0x7f2, hw:3},
        ]
    },

    /* CanTp层的配置, 从机通信会自动根据从机个数添加, 这里不需要配置  */
    CanTp:[
        {
            notes:[
                {section:"name", chs:"SDUID名称", comments:'该值PDUR的配置'},
                {section:"ch", chs:"CanTp通道", comments:'指定使用的CanTp通道'},
                {section:"rx", chs:"接收CanId", comments:'接收CanId'},
                {section:"tx", chs:"发送CanId", comments:'发送CanId'},
                {section:"hw", chs:"硬件Can控制器", comments:'接收使用的硬件Can控制器'},
            ],
            name:"UdsPhy",
            ch:0,
            rx:0x740,
            tx:0x748,
            hw:0
        },
        {name:"UdsFunc", ch:0, rx:0x7df, hw:0},
    ],

    /* J1930Tp的配置*/
    J1939Tp:[
        {
            name:"Charger",
            self:0xf4,
            remote:0x56,
            hw:2,
            pgs:[
                // 国标充电机报文
                { pri:6, pgn: 0x2700, type:"tx"},
                { pri:7, pgn: 0x0200, type:"tx"},
                { pri:7, pgn: 0x0600, type:"tx"},
                { pri:4, pgn: 0x0900, type:"tx"},
                { pri:6, pgn: 0x1000, type:"tx"},
                { pri:7, pgn: 0x1100, type:"tx"},
                { pri:6, pgn: 0x1300, type:"tx"},
                { pri:7, pgn: 0x1500, type:"tx"},
                { pri:7, pgn: 0x1600, type:"tx"},
                { pri:7, pgn: 0x1700, type:"tx"},
                { pri:4, pgn: 0x1900, type:"tx"},
                { pri:6, pgn: 0x1C00, type:"tx"},
                { pri:2, pgn: 0x1E00, type:"tx"},
                { pri:6, pgn: 0x2600, type:"rx"},
                { pri:6, pgn: 0x0100, type:"rx"},
                { pri:6, pgn: 0x0700, type:"rx"},
                { pri:6, pgn: 0x0800, type:"rx"},
                { pri:4, pgn: 0x0A00, type:"rx"},
                { pri:6, pgn: 0x1200, type:"rx"},
                { pri:4, pgn: 0x1A00, type:"rx"},
                { pri:6, pgn: 0x1D00, type:"rx"},
                { pri:2, pgn: 0x1F00, type:"rx"},
                // 其他充电机报文自动选择协议后增加
            ]
        }, {
            name:"Vcu",
            hw:1,
            self:0x13,
            remote:0x14,
            pgs:[
                { pri: 7, pgn:0x0100, type:"rx"},
                { pri: 7, pgn:0x0200, type:"tx"},
                // { id: 0x92345678, type: "srx"},
                // { id:0x2F4, type: "stx"},
                // { id:0x4F4, type: "stx"},
                // { id:0x5F4, type: "stx"},
                // { id:0x7F4, type: "stx"},
            ]
        },
    ],

    GB32960: {
        apn: "",
        server: "dtu-2021.ievdata.com:30097",
        rtmsg_interval: 30,
        /* 发生故障时故障等级对应发送周期,单位S*/
        rtmsg_alarm_intervals: [5, 3, 2, 2],
    },

    /* DtuTp的配置, 目前只支持UDS的物理寻址数据连接 */
    DtuTp:{
        rxsduids:["UDS_PHYICAL"],
        txsduids:["UDS_PHYICAL"]
    },

    InnerTp: { },

    /* 数据路由表的配置 */
    PduR:{
        tx:[
            {upper:["Dcm", 0], middle:["CanTp", "UdsPhy"]},
            {upper:["Dcm", 1], middle:["CanTp", "UdsPhy"]},
            {upper:["Dcm", 2], middle:["DtuTp", "UDS_PHYICAL"]},
            {upper:["Dcm", 3], middle:["DtuTp", "UDS_PHYICAL"]},
            {upper:["ChargerComm", "GB_BHM"], middle:["J1939Tp", "Charger_PG_2700"]},
            {upper:["ChargerComm", "GB_BRM"], middle:["J1939Tp", "Charger_PG_0200"]},
            {upper:["ChargerComm", "GB_BCP"], middle:["J1939Tp", "Charger_PG_0600"]},
            {upper:["ChargerComm", "GB_BRO"], middle:["J1939Tp", "Charger_PG_0900"]},
            {upper:["ChargerComm", "GB_BCL"], middle:["J1939Tp", "Charger_PG_1000"]},
            {upper:["ChargerComm", "GB_BCS"], middle:["J1939Tp", "Charger_PG_1100"]},
            {upper:["ChargerComm", "GB_BSM"], middle:["J1939Tp", "Charger_PG_1300"]},
            {upper:["ChargerComm", "GB_BMV"], middle:["J1939Tp", "Charger_PG_1500"]},
            {upper:["ChargerComm", "GB_BMT"], middle:["J1939Tp", "Charger_PG_1600"]},
            {upper:["ChargerComm", "GB_BSP"], middle:["J1939Tp", "Charger_PG_1700"]},
            {upper:["ChargerComm", "GB_BST"], middle:["J1939Tp", "Charger_PG_1900"]},
            {upper:["ChargerComm", "GB_BSD"], middle:["J1939Tp", "Charger_PG_1C00"]},
            {upper:["ChargerComm", "GB_BEM"], middle:["J1939Tp", "Charger_PG_1E00"]},
            // {upper:["ChargerComm", "USER_TC"], middle:["J1939Tp", "Charger_ID_9806E5F4"]},

            // {upper:["VcuComm", 0], middle:["J1939Tp", "Vcu_ID_000002F4"]},
            // {upper:["VcuComm", 1], middle:["J1939Tp", "Vcu_ID_000004F4"]},
            // {upper:["VcuComm", 2], middle:["J1939Tp", "Vcu_ID_000005F4"]},
            // {upper:["VcuComm", 3], middle:["J1939Tp", "Vcu_ID_000007F4"]},
        ],


        rx:[
            {upper:["Dcm", 0], middle:["CanTp", "UdsPhy"]},
            {upper:["Dcm", 1], middle:["CanTp", "UdsFunc"]},
            {upper:["Dcm", 2], middle:["DtuTp", "UDS_PHYICAL"]},
            {upper:["ChargerComm", "GB_CHM"], middle:["J1939Tp", "Charger_PG_2600"]},
            {upper:["ChargerComm", "GB_CRM"], middle:["J1939Tp", "Charger_PG_0100"]},
            {upper:["ChargerComm", "GB_CTS"], middle:["J1939Tp", "Charger_PG_0700"]},
            {upper:["ChargerComm", "GB_CML"], middle:["J1939Tp", "Charger_PG_0800"]},
            {upper:["ChargerComm", "GB_CRO"], middle:["J1939Tp", "Charger_PG_0A00"]},
            {upper:["ChargerComm", "GB_CCS"], middle:["J1939Tp", "Charger_PG_1200"]},
            {upper:["ChargerComm", "GB_CST"], middle:["J1939Tp", "Charger_PG_1A00"]},
            {upper:["ChargerComm", "GB_CSD"], middle:["J1939Tp", "Charger_PG_1D00"]},
            {upper:["ChargerComm", "GB_CEM"], middle:["J1939Tp", "Charger_PG_1F00"]},
            // {upper:["ChargerComm", "USER_TC"], middle:["J1939Tp", "Charger_ID_98FF50E5"]},
        ],

    },
};
