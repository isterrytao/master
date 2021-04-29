#ifndef _CHECKFIRMWARE_H_
#define _CHECKFIRMWARE_H_

#include "Platform_Types.h"

void chkFwClearSavedBurnId(void);
void chkFwReserveBurnId(uint8 *appInfoTag, uint16 len);
boolean checkFirmware(uint32 appInfoTagAddr);

#endif // _CHECKFIRMWARE_H_