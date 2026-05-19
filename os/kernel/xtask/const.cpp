#pragma once

#define LAPIC_ID                  0x020
#define LAPIC_VERSION             0x030
#define LAPIC_TPR                 0x080
#define LAPIC_APR                 0x090
#define LAPIC_PPR                 0x0A0
#define LAPIC_EOI                 0x0B0
#define LAPIC_RRD                 0x0C0
#define LAPIC_LDR                 0x0D0
#define LAPIC_DFR                 0x0E0
#define LAPIC_SPURIOUS            0x0F0
#define LAPIC_ISR_BASE            0x100
#define LAPIC_TMR_BASE            0x180
#define LAPIC_IRR_BASE            0x200
#define LAPIC_ESR                 0x280
#define LAPIC_ICR_LO              0x300
#define LAPIC_ICR_HI              0x310
#define LAPIC_LVT_TIMER           0x320
#define LAPIC_LVT_THERMAL         0x330
#define LAPIC_LVT_PERFMON         0x340
#define LAPIC_LVT_LINT0           0x350
#define LAPIC_LVT_LINT1           0x360
#define LAPIC_LVT_ERROR           0x370
#define LAPIC_TIMER_INIT_COUNT    0x380
#define LAPIC_TIMER_CURR_COUNT    0x390
#define LAPIC_TIMER_DIV_CONF      0x3E0

#define LAPIC_MASKED              (1 << 16)
#define LAPIC_PERIODIC            (1 << 17)
