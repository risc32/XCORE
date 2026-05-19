/*------------------------------------------------------------------------*/
/* ATA Driver with FatFs Disk I/O Wrappers                               */
/*------------------------------------------------------------------------*/

#include "diskio.h"
#include "ff.h"

/* Your ATA registers */
#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_FEATURES    0x1F1
#define ATA_SECTOR_COUNT 0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE_HEAD  0x1F6
#define ATA_STATUS      0x1F7
#define ATA_COMMAND     0x1F7

/* ATA commands */
#define ATA_CMD_READ     0x20
#define ATA_CMD_WRITE    0x30
#define ATA_CMD_IDENTIFY 0xEC

/* I/O port functions */
static inline uint8_t inb(uint16_t port)
{
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "d"(port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data)
{
    __asm__ volatile("outb %0, %1" : : "a"(data), "d"(port));
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t result;
    __asm__ volatile("inw %1, %0" : "=a"(result) : "d"(port));
    return result;
}

static inline void outw(uint16_t port, uint16_t data)
{
    __asm__ volatile("outw %0, %1" : : "a"(data), "d"(port));
}

/* Wait for drive ready */
static void ata_wait_bsy(BYTE drive)
{
    uint16_t port = (drive == 0) ? ATA_STATUS : (ATA_STATUS + 0x80);
    while (inb(port) & 0x80);
}

static void ata_wait_drq(BYTE drive)
{
    uint16_t port = (drive == 0) ? ATA_STATUS : (ATA_STATUS + 0x80);
    while (!(inb(port) & 0x08));
}

/*------------------------------------------------------------------------*/
/* Initialize ATA controller                                              */
/*------------------------------------------------------------------------*/

void ata_init(void)
{
    /* Reset ATA controller */
    outb(ATA_DRIVE_HEAD, 0xA0);
    for (volatile int i = 0; i < 1000; i++);
}

/*------------------------------------------------------------------------*/
/* Read sectors                                                           */
/*------------------------------------------------------------------------*/

int ata_read_sectors(BYTE drive, LBA_t sector, BYTE* buffer, UINT count)
{
    uint16_t port_base = (drive == 0) ? 0x1F0 : 0x170;
    uint16_t data_port = port_base + ATA_DATA;
    uint16_t status_port = port_base + ATA_STATUS;

    for (UINT i = 0; i < count; i++) {
        LBA_t current_sector = sector + i;

        ata_wait_bsy(drive);

        /* Send command */
        outb(port_base + ATA_DRIVE_HEAD, 0xE0 | ((current_sector >> 24) & 0x0F));
        outb(port_base + ATA_SECTOR_COUNT, 1);
        outb(port_base + ATA_LBA_LOW, (uint8_t)(current_sector));
        outb(port_base + ATA_LBA_MID, (uint8_t)(current_sector >> 8));
        outb(port_base + ATA_LBA_HIGH, (uint8_t)(current_sector >> 16));
        outb(port_base + ATA_COMMAND, ATA_CMD_READ);

        ata_wait_bsy(drive);
        ata_wait_drq(drive);

        /* Read 256 words (512 bytes) */
        uint16_t* buf16 = (uint16_t*)(buffer + (i * 512));
        for (int j = 0; j < 256; j++) {
            buf16[j] = inw(data_port);
        }

        /* Check for error */
        if (inb(status_port) & 0x01) {
            return 0;
        }
    }

    return 1;
}

/*------------------------------------------------------------------------*/
/* Write sectors                                                          */
/*------------------------------------------------------------------------*/

int ata_write_sectors(BYTE drive, LBA_t sector, const BYTE* buffer, UINT count)
{
    uint16_t port_base = (drive == 0) ? 0x1F0 : 0x170;
    uint16_t data_port = port_base + ATA_DATA;

    for (UINT i = 0; i < count; i++) {
        LBA_t current_sector = sector + i;

        ata_wait_bsy(drive);

        /* Send command */
        outb(port_base + ATA_DRIVE_HEAD, 0xE0 | ((current_sector >> 24) & 0x0F));
        outb(port_base + ATA_SECTOR_COUNT, 1);
        outb(port_base + ATA_LBA_LOW, (uint8_t)(current_sector));
        outb(port_base + ATA_LBA_MID, (uint8_t)(current_sector >> 8));
        outb(port_base + ATA_LBA_HIGH, (uint8_t)(current_sector >> 16));
        outb(port_base + ATA_COMMAND, ATA_CMD_WRITE);

        ata_wait_bsy(drive);
        ata_wait_drq(drive);

        /* Write 256 words (512 bytes) */
        const uint16_t* buf16 = (const uint16_t*)(buffer + (i * 512));
        for (int j = 0; j < 256; j++) {
            outw(data_port, buf16[j]);
        }
    }

    return 1;
}

/*------------------------------------------------------------------------*/
/* Get sector count                                                       */
/*------------------------------------------------------------------------*/

int ata_get_sector_count(BYTE drive, LBA_t* count)
{
    uint16_t port_base = (drive == 0) ? 0x1F0 : 0x170;
    uint16_t identify_data[256];

    /* Send IDENTIFY command */
    ata_wait_bsy(drive);
    outb(port_base + ATA_DRIVE_HEAD, 0xA0);
    outb(port_base + ATA_SECTOR_COUNT, 0);
    outb(port_base + ATA_LBA_LOW, 0);
    outb(port_base + ATA_LBA_MID, 0);
    outb(port_base + ATA_LBA_HIGH, 0);
    outb(port_base + ATA_COMMAND, ATA_CMD_IDENTIFY);

    ata_wait_bsy(drive);

    /* Check if drive exists */
    if (inb(port_base + ATA_STATUS) == 0) {
        return 0;
    }

    ata_wait_drq(drive);

    /* Read identify data */
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(port_base + ATA_DATA);
    }

    /* Get total sectors (28-bit) */
    *count = (LBA_t)identify_data[60] | ((LBA_t)identify_data[61] << 16);

    return (*count > 0);
}

/*------------------------------------------------------------------------*/
/* Get sector size                                                        */
/*------------------------------------------------------------------------*/

int ata_get_sector_size(BYTE drive, WORD* size)
{
    (void)drive;
    *size = 512;
    return 1;
}

/*------------------------------------------------------------------------*/
/* Get disk status                                                        */
/*------------------------------------------------------------------------*/

int ata_status(BYTE drive)
{
    uint16_t port_base = (drive == 0) ? 0x1F0 : 0x170;
    uint8_t status = inb(port_base + ATA_STATUS);

    /* Check if drive exists (status != 0) */
    return (status != 0xFF && status != 0);
}

/*------------------------------------------------------------------------*/
/* I/O Control                                                            */
/*------------------------------------------------------------------------*/

int ata_ioctl(BYTE drive, BYTE cmd, void* buff)
{
    (void)drive;
    (void)buff;

    switch (cmd) {
        case CTRL_SYNC:
            /* Wait for all writes - nothing to do for PIO mode */
            return 1;
        case CTRL_TRIM:
            /* TRIM not supported in this stub */
            return 0;
        default:
            return 0;
    }
}

/*------------------------------------------------------------------------*/
/* FatFs Disk I/O Functions - WRAPPERS                                    */
/*------------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv)
{
    (void)pdrv;

    ata_init();

    if (!ata_status(pdrv)) {
        return STA_NODISK;
    }

    return 0;
}

DSTATUS disk_status(BYTE pdrv)
{
    (void)pdrv;

    if (!ata_status(pdrv)) {
        return STA_NODISK;
    }

    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
    (void)pdrv;

    if (!buff || count == 0) {
        return RES_PARERR;
    }

    if (ata_read_sectors(pdrv, sector, buff, count)) {
        return RES_OK;
    }

    return RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
    (void)pdrv;

    if (!buff || count == 0) {
        return RES_PARERR;
    }

    if (ata_write_sectors(pdrv, sector, buff, count)) {
        return RES_OK;
    }

    return RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
    (void)pdrv;

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            if (!ata_get_sector_count(pdrv, (LBA_t*)buff)) {
                return RES_ERROR;
            }
            return RES_OK;

        case GET_SECTOR_SIZE:
            if (!ata_get_sector_size(pdrv, (WORD*)buff)) {
                return RES_ERROR;
            }
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}