#include "utypes.h"

#ifdef __LINUX__
#include <time.h>
#endif

#ifdef __VXWORKS__
#include "vxworks.h"
#include <tickLib.h>
#include <sysLib.h>
#endif

DWORD GetMs(void)
{
	DWORD dwMs = 0; 

#ifdef __LINUX__
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	dwMs = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
#ifdef __VXWORKS__
	DWORD _dwSec = tickGet() / sysClkRateGet();
	DWORD _dwMs = tickGet() % sysClkRateGet() * 1000 / sysClkRateGet();
	dwMs = _dwSec * 1000 + _dwMs;
#endif

	//ERROR

	return dwMs;
}

void EnterCritical(void)
{

}
void LeaveCritical(void)
{

}

void SMP_MB(void)
{
	
}
void SMP_WMB(void)
{

}
void SMP_RMB(void)
{

}




