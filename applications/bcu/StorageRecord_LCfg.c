#include "StorageRecord.h"



#pragma push
#pragma DATA_SEG __GPAGE_SEG F4_STACK
/**
 * 要确保内存空间不小于Flash的最小写入单元
 */
static uint8 record_data_cache[512];
#pragma pop
static struct StorageCycleContextType record_ctx = {
    record_data_cache,
    {
        0U,0U,0U,0U
    },
    0U,
    0U,
    0U,
    0UL,
    0UL,
};

const struct StorageCycleObjectType StorageRecordObject = {
    STORAGEM_RECORD_ID,
    &record_ctx
};

static struct StorageCycleContextType record_ctx_read;
const struct StorageCycleObjectType StorageRecordObjectRead = {
    STORAGEM_RECORD_ID,
    &record_ctx_read
};
