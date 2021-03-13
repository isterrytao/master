#include "DtuModelFota.h"
#include "Async_Event.h"
#include "DtuComm_M35.h"
// #include "DtuTp_Ota.h"
#include "atcmd_ec20.h"
#include "GB32960.h"
#include "Diagnosis.h"
#include "string.h"
#include "ParameterM.h"

DtuModelFota_InnerDataType DtuModelFota_innerData;
static Async_EvnetCbkReturnType DtuModelFota_Poll(Async_EventType *event, uint8 byWhat);
static boolean isNeedFota(void);
static void fota_main_poll(void);

void DtuModelFota_Init(Async_LooperType *looper)
{
	DtuModelFota_innerData.State = DTU_MODEL_FOTA_INIT;
	if (looper != NULL)
	{
		if (E_OK == Async_EventInit(&DtuModelFota_innerData.event, looper, DtuModelFota_Poll, 10000UL))
		{
			(void)Async_EventRegisterToLooper(&DtuModelFota_innerData.event);
		}
	}
}

static Async_EvnetCbkReturnType DtuModelFota_Poll(Async_EventType *event, uint8 byWhat)
{
	(void)event;
    (void)byWhat;

    fota_main_poll();

    return ASYNC_EVENT_CBK_RETURN_OK;
}

// static char url[] = "https://img.udantech.com/lte_cat1_fota/EC200TCNDAR02A15M16_fota.bin";
// static char url[] = "https://img.udantech.com/lte_cat1_fota/EC200TCNDAR02A13M16_fota.bin";

static boolean isNeedFota(void)
{
	const char *p, *p1;
	char line[17] = {0};
	boolean flag = FALSE;
	uint16 temp = GB32960_FotaCommandGet();
	boolean res = FALSE;

	if (DtuComm_M35_innerData.mVersions.VersionIsGet == TRUE &&
		DtuModelFota_innerData.State == DTU_MODEL_FOTA_INIT)
	{
		if (temp == FORCE_FOTA_COMMAND)
		{
			flag = TRUE;
		}
		else if (temp == NORMAL_FOTA_COMMAND)
		{
			if (Diagnosis_GetDiagLevelMax() == DIAGNOSIS_LEVEL_NONE)
			{
				flag = TRUE;
			}
		}
		else
		{

		}

		if (flag)
		{
			p = GB32960_FotaPackageUrlGet();
			// p = url;
			if (strlen(p) > 0U)
			{
				uint16 len = sizeof(line) - 1U;
				p1 = DtuComm_M35_innerData.mVersions.SoftwareVersion;
				(void)memcpy(line, p1, len);
				if (strstr(p, line) != NULL)
				{
					(void)ParameterM_EeepWrite(PARAMETERM_EEEP_FOTA_COMMAND_INDEX, temp);
					res = TRUE;
				}
			}
		}
	}

    return res;
}

static void fota_main_poll(void)
{
	DtuModelFota_InnerDataType *data = &DtuModelFota_innerData;
	// char *p = url;
	const char *p = GB32960_FotaPackageUrlGet();
	uint32 nowtime;
	uint16 temp;

	switch(data->State)
	{
		case DTU_MODEL_FOTA_INIT:
			if (isNeedFota())
			{
				DtuModelFota_innerData.State = DTU_MODEL_FOTA_START;
			}
			else
			{
				if (ParameterM_EeepRead(PARAMETERM_EEEP_FOTA_COMMAND_INDEX, &temp) == E_OK)
				{
					if (temp > 0U && temp < 3U)
					{
						DtuModelFota_innerData.State = DTU_MODEL_CLOSE_TCP;
					}
				}
			}
			break;
		case DTU_MODEL_FOTA_START:
			Async_EventSetTimeout(&data->event, 1000UL);
			// 关闭其他网络任务
			GB32960_TaskStop();
			// 开始OTA
			if (DtuM35_FotaStart(p, strlen(p)) == E_OK)
			{
				DtuModelFota_innerData.State = DTU_MODEL_FOTA_STATE_CHECK;
			}
			DtuModelFota_innerData.StartOtaTimeTick = OSTimeGet();
			break;
		case DTU_MODEL_CLOSE_TCP:
			Async_EventSetTimeout(&data->event, 1000UL);
			// 关闭其他网络任务
			GB32960_TaskStop();
			DtuModelFota_innerData.StartOtaTimeTick = OSTimeGet();
			DtuModelFota_innerData.State = DTU_MODEL_FOTA_STATE_CHECK;
			break;
		case DTU_MODEL_FOTA_STATE_CHECK:
			// 读取OTA状态
			nowtime = OSTimeGet();
			if (DtuComm_M35_innerData.mFotaParameter.fota_http_error_code == 0U)
			{
				if (DtuComm_M35_innerData.mFotaParameter.fota_progress == 100U)
				{
					if (DtuComm_M35_innerData.mFotaParameter.fota_fota_error_code == 0U)
					{
						DtuModelFota_innerData.State = DTU_MODEL_FOTA_FINISH;
					}
				}
			}
			if (MS_GET_INTERNAL(DtuModelFota_innerData.StartOtaTimeTick, nowtime) > 10UL * 60UL * 1000UL)
			{
				(void)ParameterM_EeepWrite(PARAMETERM_EEEP_FOTA_COMMAND_INDEX, 0U);
				GB32960_FotaCommandSet(0U);
				if (GB32960_FotaCommandGet() > 0U)
				{
					DtuModelFota_innerData.State = DTU_MODEL_FOTA_FINISH;
				}
				else
				{
					DtuModelFota_innerData.State = DTU_MODEL_FOTA_FINISH;
				}
			}
			break;
		case DTU_MODEL_FOTA_FINISH:
			// 开启其他网络任务
			GB32960_TaskRecover();
			// OTA_Start();
			DtuModelFota_innerData.State = DTU_MODEL_FOTA_INIT;
			Async_EventSetTimeout(&data->event, 10000UL);
			GB32960_FotaCommandSet(0U);
			(void)ParameterM_EeepWrite(PARAMETERM_EEEP_FOTA_COMMAND_INDEX, 0U);
			DtuModelFota_innerData.FotaFaultNum = 0U;
			// 更新版本号
			break;
		case DTU_MODEL_FOTA_ERROR:
			DtuModelFota_innerData.State = DTU_MODEL_FOTA_INIT;
			Async_EventSetTimeout(&data->event, 10000UL);
			if (DtuModelFota_innerData.FotaFaultNum < 3U)
			{
				DtuModelFota_innerData.FotaFaultNum++;
			}
			else
			{
				GB32960_TaskRecover();
				GB32960_FotaCommandSet(0U);
				DtuModelFota_innerData.FotaFaultNum = 0U;
			}
			break;
	}
}
