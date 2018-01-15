#include "CurrentM.h"
#include "Hv.h"
#include "SystemConnection_Lcfg.h"
#include "Soc.h"
#include "CellDataM.h"
#include "statistic.h"
#include "Hmi.h"

static uint16 MDBSgetTotalVoltage(const Modbus_RegistersRegionType *p, uint16 addr) {
    uint16 rc;
    (void)addr;
    (void)p;

    rc = Statistic_GetBcu100mvTotalVoltage();
    rc = (rc + 5U) / 10U;
    return rc;
}

static uint16 MDBSgetTotalCurrent(const Modbus_RegistersRegionType *p, uint16 addr) {
    sint16 rc;
    (void) addr;
    (void)p;

    rc =  (sint16)CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);

	if(rc > (-32766L))	{
		if (rc < 0) {
			rc = (rc - 5) / 10;
		} else {
			rc = (rc + 5) / 10;
		}
	}
	else{

	}
    
    return (uint16)rc;
}

static uint16 MDBSgetSOC(const Modbus_RegistersRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;

    rc = Soc_Get();
    rc = (uint16)((rc + 5U) / 10U);
    return rc;
}

static uint16 MDBSgetCellNum(const Modbus_RegistersRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;

    return (uint16)SYSTEM_BATTERY_CELL_NUM;
}

static uint16 MDBSgetTempSensorNum(const Modbus_RegistersRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;

    return (uint16)SYSTEM_TEMP_CELL_NUM;
}

static uint16 MDBSgetHeatSensorNum(const Modbus_RegistersRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;

    return (uint16)SYSTEM_HEAT_TEMP_NUM;
}

static uint16 MDBSgetPoleTempNum(const Modbus_RegistersRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;

    return (uint16)SYSTEM_POLE_TEMP_NUM;
}

/*2000-2399*/
static uint16 MDBSget_CellDataM_GetVoltage(const Modbus_RegistersRegionType *p, uint16 addr) {
    uint16 rc;
    rc = CellDataM_GetVoltage(addr - p->startaddr);
    return rc;
}

/*3000,3199*/
static uint16 MDBSget_CellDataM_GetTemperature(const Modbus_RegistersRegionType *p, uint16 addr) {
    sint16 rc;

    rc = (sint16)CellDataM_GetTemperature(addr - p->startaddr);
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {

    }
    return (uint16)rc;
}

/*3500,3549,*/
static uint16 MDBSget_CellDataM_GetHeatTemperature(const Modbus_RegistersRegionType *p, uint16 addr) {
    sint16 rc;

    rc = (sint16)CellDataM_GetHeatTemperature(addr - p->startaddr);
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {

    }
    return (uint16)rc;
}

/*3600,3649,*/
static uint16 MDBSget_CellDataM_GettPoleTemperature(const Modbus_RegistersRegionType *p, uint16 addr) {
    sint16 rc;

    rc = (sint16)CellDataM_GetPoleTemperature(addr - p->startaddr);
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {

    }

    return (uint16)rc;
}

static const Modbus_RegistersRegionType x04registerTable[] = {

    {2000U, 2399U, MDBSget_CellDataM_GetVoltage},
    /*	{2400, 2999, get2},*/
    {3000U, 3199U, MDBSget_CellDataM_GetTemperature},
    /*	{3200,3499, getCellVolVisable},*/
    {3500U, 3549U, MDBSget_CellDataM_GetHeatTemperature},
    /*	{3550,3599, getCellVolVisable},*/
    {3600U, 3649U, MDBSget_CellDataM_GettPoleTemperature},
    /*	{3650,3699, getCellVolVisable},*/

    {1000U, 1000U, MDBSgetTotalVoltage},
    {1001U, 1001U, MDBSgetTotalCurrent},
    {1002U, 1002U, MDBSgetSOC},
    {1003U, 1003U, MDBSgetCellNum},
    {1004U, 1004U, MDBSgetTempSensorNum},
    {1005U, 1005U, MDBSgetHeatSensorNum},
    {1006U, 1006U, MDBSgetPoleTempNum},
};


const Modbus_RegistersTableType Modbus_X04registerTable = {
    x04registerTable,
    (uint8)ARRAY_SIZE(x04registerTable),
    1U,
};

static uint16 MDBSgetalarm(const Modbus_RegistersRegionType *p, uint16 addr) {
    uint8 i, j;
    uint16 trc, rc = 0U;

    for (i = 0U; i < 16U; i++) {
        if (Diagnosis_GetLevel((Diagnosis_ItemType)(i + addr - p->startaddr))) {
            rc = (rc | ((uint16)0x0001U << i));
        }
    }
    trc = rc & 0x00ffU;
    j = (uint8)(rc >> 8U);
    rc = (trc << 8U) | j;
    return rc;
}

static const Modbus_RegistersRegionType x02registerTable[] = {
    {2000U, 2399U, MDBSgetalarm},
};

const Modbus_RegistersTableType Modbus_X02registerTable = {
    x02registerTable,
    (uint8)ARRAY_SIZE(x02registerTable),
    16U,
};

