/*------------------------------------------------------------------------*/
/* OS Dependent Functions for FatFs - XCore OS Full Implementation       */
/*------------------------------------------------------------------------*/

#include "ff.h"
//#include "os/kernel/memory/memory.h"
//#include "os/kernel/xtask/xtask.h"  /* For mutex support */
struct datetime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t weekday;
};

// Ваша функция получения времени из RTC
datetime get_rtc_time() {
    datetime dt = {0};

    // Чтение из CMOS RTC
    outb(0x70, 0x00);
    dt.second = inb(0x71);
    outb(0x70, 0x02);
    dt.minute = inb(0x71);
    outb(0x70, 0x04);
    dt.hour = inb(0x71);
    outb(0x70, 0x07);
    dt.day = inb(0x71);
    outb(0x70, 0x08);
    dt.month = inb(0x71);
    outb(0x70, 0x09);
    dt.year = inb(0x71);

    // Конвертация BCD в binary если нужно
    // ...

    return dt;
}
/*------------------------------------------------------------------------*/
/* Memory Allocation (Required for LFN=3)                                 */
/*------------------------------------------------------------------------*/

#if FF_USE_LFN == 3

void* ff_memalloc (UINT msize)
{
    /* Use your kernel heap allocator */
    return malloc(msize);
}

void ff_memfree (void* mblock)
{
    /* Use your kernel heap deallocator */
    free(mblock);
}

#endif /* FF_USE_LFN == 3 */


/*------------------------------------------------------------------------*/
/* Mutex Support (Required for FF_FS_REENTRANT=1)                         */
/*------------------------------------------------------------------------*/

#if FF_FS_REENTRANT

/* Mutex table - one per volume + one system mutex */
static xtask::spinlock mutex_table[FF_VOLUMES + 1];

int ff_mutex_create (int vol)
{
    /* Initialize mutex for this volume */
    /* Your spinlock is already initialized to 0 */
    mutex_table[vol]._lock = 0;
    return 1;  /* Success */
}

void ff_mutex_delete (int vol)
{
    /* Nothing to clean up for spinlock */
    (void)vol;
}

int ff_mutex_take (int vol)
{
    /* Try to acquire mutex with timeout */
    uint32_t start = get_tick_count();  /* You need to implement this */

    while (!mutex_table[vol].try_acquire()) {
        if (get_tick_count() - start > FF_FS_TIMEOUT) {
            return 0;  /* Timeout */
        }
        /* Yield CPU to other tasks */
        xtask::yield();  /* You need to implement this */
    }

    return 1;  /* Success */
}

void ff_mutex_give (int vol)
{
    mutex_table[vol].release();
}

#endif /* FF_FS_REENTRANT */


/*------------------------------------------------------------------------*/
/* Helper Functions (Replace stdlib dependencies)                         */
/*------------------------------------------------------------------------*/

/* String functions without stdlib */
static int ff_strcmp(const char* s1, const char* s2)
{
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

static int ff_strncmp(const char* s1, const char* s2, size_t n)
{
    while (n-- && *s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return n == (size_t)-1 ? 0 : (unsigned char)*s1 - (unsigned char)*s2;
}

static char* ff_strcpy(char* dest, const char* src)
{
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

static size_t ff_strlen(const char* s)
{
    size_t len = 0;
    while (*s++) len++;
    return len;
}

/* Code page conversion tables would go here */
/* For now, just pass-through for ASCII */

/*------------------------------------------------------------------------*/
/* Code Page Conversion (for FF_CODE_PAGE=0)                              */
/*------------------------------------------------------------------------*/

/* This is a minimal implementation. For full Unicode support,
   you'd need conversion tables. This handles basic ASCII only. */

WORD ff_convert (WORD chr, UINT dir)
{
    /* dir = 0: OEM to Unicode, dir = 1: Unicode to OEM */
    if (chr < 0x80) return chr;  /* ASCII */

    /* For non-ASCII, return '?' */
    return '?';
}

WCHAR ff_wtoupper (WCHAR chr)
{
    /* Simple ASCII uppercase conversion */
    if (chr >= 'a' && chr <= 'z') {
        return chr - 0x20;
    }
    return chr;
}