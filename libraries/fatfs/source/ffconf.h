/*---------------------------------------------------------------------------/
/  Configurations of FatFs Module
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	80386	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* 0:Read/Write, 1:Read-only */

#define FF_FS_MINIMIZE	0
/* 0: All functions enabled
   1: Remove f_stat, f_getfree, f_unlink, f_mkdir, f_truncate, f_rename
   2: Remove f_opendir, f_readdir, f_closedir in addition to 1
   3: Remove f_lseek in addition to 2 */

#define FF_USE_FIND		0
/* 0: Disable f_findfirst/f_findnext
   1: Enable with matching name
   2: Enable with matching altname[] too */

#define FF_USE_MKFS		1
/* 0: Disable f_mkfs, 1: Enable f_mkfs */

#define FF_USE_FASTSEEK	1
/* 0: Disable fast seek, 1: Enable fast seek */

#define FF_USE_EXPAND	1
/* 0: Disable f_expand, 1: Enable f_expand */

#define FF_USE_CHMOD	0
/* 0: Disable f_chmod/f_utime, 1: Enable */

#define FF_USE_LABEL	1
/* 0: Disable f_getlabel/f_setlabel, 1: Enable */

#define FF_USE_FORWARD	1
/* 0: Disable f_forward, 1: Enable */

#define FF_USE_STRFUNC	0
/* 0: Disable f_gets/f_putc/f_puts/f_printf
   1: Enable without LF-CRLF conversion
   2: Enable with LF-CRLF conversion */
#define FF_PRINT_LLI	0
/* 1: Enable long long argument in f_printf */
#define FF_PRINT_FLOAT	0
/* 1: Enable floating point argument in f_printf */
#define FF_STRF_ENCODE	3
/* 0: ANSI/OEM in current CP
   1: UTF-16LE
   2: UTF-16BE
   3: UTF-8 */


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	866

#define FF_USE_LFN		3
/* 0: Disable LFN
   1: LFN with static working buffer on BSS
   2: LFN with dynamic working buffer on STACK
   3: LFN with dynamic working buffer on HEAP */
#define FF_MAX_LFN		255
/* Maximum LFN length (12-255) */

#define FF_LFN_UNICODE	0
/* 0: ANSI/OEM (TCHAR = char)
   1: UTF-16 (TCHAR = WCHAR)
   2: UTF-8 (TCHAR = char)
   3: UTF-32 (TCHAR = DWORD) */

#define FF_LFN_BUF		255
/* LFN buffer size for FILINFO */
#define FF_SFN_BUF		12
/* SFN buffer size for FILINFO */

#define FF_FS_RPATH		2
/* 0: Disable relative path
   1: Enable with f_chdir/f_chdrive
   2: Enable with f_getcwd in addition to 1 */

#define FF_PATH_DEPTH	10
/* Max directory depth (for exFAT, 1-255) */


/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		2
/* Number of logical drives (1-10) */

#define FF_STR_VOLUME_ID	0
/* 0: Use numeric drive IDs
   1: Use string volume IDs from FF_VOLUME_STRS
   2: Use string volume IDs with user table */
#define FF_VOLUME_STRS		"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
/* Volume ID strings (used only if FF_STR_VOLUME_ID >= 1) */

#define FF_MULTI_PARTITION	0
/* 0: Single partition per physical drive
   1: Multiple partitions per physical drive */

#define FF_MIN_SS		512
#define FF_MAX_SS		512
/* Sector size range (512, 1024, 2048, 4096) */

#define FF_LBA64		0
/* 0: 32-bit LBA, 1: 64-bit LBA */

#define FF_MIN_GPT		0x10000000
/* Minimum sectors to use GPT when formatting */

#define FF_USE_TRIM		0
/* 0: Disable TRIM, 1: Enable TRIM */


/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* 0: Normal buffer mode, 1: Tiny buffer mode */

#define FF_FS_EXFAT		0
/* 0: Disable exFAT, 1: Enable exFAT (requires LFN >= 1 and LBA64 = 1) */

#define FF_FS_NORTC		1
/* 0: Use get_fattime() for timestamp
   1: Use fixed timestamp */
#define FF_NORTC_MON	1
#define FF_NORTC_MDAY	1
#define FF_NORTC_YEAR	2025
/* Fixed timestamp values (used when FF_FS_NORTC = 1) */

#define FF_FS_CRTIME	0
/* 0: Disable creation time, 1: Enable creation time */

#define FF_FS_NOFSINFO	0
/* 0: Use FSINFO if available
   1: Do not trust FSINFO (scan FAT for free space)
   2: Use last allocated cluster number from FSINFO
   3: Combine 1 and 2 */

#define FF_FS_LOCK		0
/* 0: Disable file lock
   >0: Number of files that can be opened simultaneously */

#define FF_FS_REENTRANT	0
/* 0: Disable reentrancy (not thread-safe)
   1: Enable reentrancy (thread-safe) */
#define FF_FS_TIMEOUT	1000
/* Timeout in OS ticks (used only if FF_FS_REENTRANT = 1) */


/*--- End of configuration options ---*/