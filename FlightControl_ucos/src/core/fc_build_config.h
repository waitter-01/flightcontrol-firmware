#ifndef FC_BUILD_CONFIG_H
#define FC_BUILD_CONFIG_H

#ifndef FC_CFG_DANGEROUS_OUTPUTS_ENABLED
#define FC_CFG_DANGEROUS_OUTPUTS_ENABLED 0
#endif

#ifndef FC_CFG_HW_WATCHDOG_ENABLED
#define FC_CFG_HW_WATCHDOG_ENABLED 0
#endif

#ifndef FC_CFG_HEALTH_FAULT_INJECTION_ENABLED
#define FC_CFG_HEALTH_FAULT_INJECTION_ENABLED 0
#endif

#if (FC_CFG_DANGEROUS_OUTPUTS_ENABLED != 0)
#error "Dangerous outputs are not implemented or approved in this phase"
#endif

#if (FC_CFG_HW_WATCHDOG_ENABLED != 0)
#error "The hardware watchdog backend is not implemented or approved in this phase"
#endif

#if ((FC_CFG_HEALTH_FAULT_INJECTION_ENABLED != 0) && \
     (FC_CFG_HEALTH_FAULT_INJECTION_ENABLED != 1))
#error "FC_CFG_HEALTH_FAULT_INJECTION_ENABLED must be 0 or 1"
#endif

#endif
