/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"        /* FatFs lower layer API */
#include <stdint.h>
#include "Drivers/src/SPI.h"
#include "Drivers/src/inner/__SPI.h"
#include "Drivers/src/GPIO.h"
#include "Misc/src/definitions.h"
#include <stdbool.h>
#include <stddef.h>
#include "ff.h"
#include "tm4c123gh6pm.h"
#include <stdlib.h>

#define __SD_TYPE_NONE   0
#define __SD_TYPE_SDHC   1
#define __SD_TYPE_SDSC   2
#define __SD_TYPE_MMC    3

// TODO: need to be set from CMake
#define spi_module SPI0
DSTATUS cardState = STA_NOINIT;
byte cardType = __SD_TYPE_NONE;

/* Definitions for MMC/SDC command */
#define CMD0                        (0x40+0)    /* GO_IDLE_STATE - init card in spi mode if CS low */
#define CMD1                        (0x40+1)    /* SEND_OP_COND */
#define CMD8                        (0x40+8)    /* SEND_IF_COND - verify SD Memory Card interface operating condition.*/
#define CMD9                        (0x40+9)    /* SEND_CSD - read the Card Specific Data (CSD register)*/
#define CMD10                       (0x40+10)   /* SEND_CID - read the card identification information (CID register)*/
#define CMD12                       (0x40+12)   /* STOP_TRANSMISSION */
#define CMD16                       (0x40+16)   /* SET_BLOCKLEN */
#define CMD17                       (0x40+17)   /* READ_SINGLE_BLOCK */
#define CMD18                       (0x40+18)   /* READ_MULTIPLE_BLOCK */
#define CMD23                       (0x40+23)   /* SET_BLOCK_COUNT */
#define CMD24                       (0x40+24)   /* WRITE_BLOCK */
#define CMD25                       (0x40+25)   /* WRITE_MULTIPLE_BLOCK */
#define CMD41                       (0x40+41)   /* SEND_OP_COND (ACMD) */
#define CMD55                       (0x40+55)   /* APP_CMD */
#define CMD58                       (0x40+58)   /* READ_OCR */

#define SD_SECTOR_SIZE              512

/* status for card in the ready state */
/* NOTE: this part taken from Arduino `SD` library */
#define R1_READY_STATE          0X00
/* status for card in the idle state */
#define R1_IDLE_STATE           0X01
/* status bit for illegal command */
#define R1_ILLEGAL_COMMAND      0X04
/* start data token for read or write single block*/
#define DATA_START_BLOCK        0XFE
/* stop token for write multiple blocks*/
#define STOP_TRAN_TOKEN         0XFD
/* start data token for write multiple blocks*/
#define WRITE_MULTIPLE_TOKEN    0XFC
/* mask for data response tokens after a write block operation */
#define DATA_RES_MASK           0X1F
/* write data accepted token */
#define DATA_RES_ACCEPTED       0X05

/* Protect block zero from write if nonzero */
#define SD_PROTECT_BLOCK_ZERO 1
/* init timeout ms */
#define SD_INIT_TIMEOUT 2000
/* erase timeout ms */
#define SD_ERASE_TIMEOUT 10000
/* read timeout ms */
#define SD_READ_TIMEOUT 300
/* write time out ms */
#define SD_WRITE_TIMEOUT 600

static void Delay1ms(unsigned long msec)
{
// write this function
    unsigned long counter;

    while( msec > 0 )
    {
        counter = SYS_CLK_SPEED / 10000;
        while(counter-- > 0);
        msec--;
    }
}


/*************************************************************************/
//static void SPI_sendByte( byte data )
//{
//    SPI_write(SPI_driver, data);
//}

//static byte SPI_receiveByte()
//{
//    SPI_write(SPI_driver, 0xFF);
//    byte data = SPI_read(SPI_driver);
//    return data;
//}

//static inline byte SPI_transfer( SPI_driver, byte data)
//{
//    /* write dummy data */
//    SPI_write(SPI_driver, 0xFF);

//    /* read data frm rx fifo */
//    return SPI_read(SPI_driver);
//}

//void SPI_releaseBus()
//{
//    SPI_setChipSelectState( SPI_driver, HIGH );
//    SPI_transfer( SPI_driver, 0xFF );
//}

//void SPI_holdBus()
//{
//    SPI_setChipSelectState( SPI_driver, LOW );
////    SPI_transfer( SPI_driver, 0xFF);
////    if( SD_isReady() )
////        return 1;

//}

// Function to get the SDCard into SPI mode
static void connect_SD_to_SPI()
{
//    SPI_releaseBus();
//    SPI_setChipSelectState( spi_module, HIGH );
//    __SPI_setMOSI( spi_module, HIGH );

    /* TODO: Switch the SPI TX line to a GPIO and drive it high too. */
//    GPIO_PORTA_AFSEL_R &= ~0b10000;
//    GPIO_PORTA_ODR_R &= ~0b10000;

    /*
     * Send 10 bytes over the SPI bus. This causes the clock to toggle several
     * times to get the SD Card into SPI mode.
     */
    for (int iii = 0; iii < 10; iii++)
        SPI_transfer(spi_module, 0xFF);

//    Delay1ms(10);

//    GPIO_PORTA_AFSEL_R |= 0b10000;

    /* TODO: Revert to hardware control of the SPI TX line. */
}

/*************************************************************************/

/*************************************************************************/
static bool SD_waitForToken( byte token, int timeout )
{
    byte result;
    int counter = 0;
    // TODO: timeout = 500ms
//    SPI_transfer( SPI_driver, 0xFF);

    do{
        result = SPI_transfer(spi_module, 0xFF);
//        Delay1ms(timeout);
        counter++;
    } while( (result is_not token) and (counter < timeout) );

    if( counter == timeout )
        return false;

    return true;
}

static bool SD_dataBlock_recieve(byte* buffer, size_t len)
{
//    byte token;

//    do {
////         TODO: timeout = 100 ms
////         send dummy data
//        token = SPI_transfer( spi_module, 0xFF );
//    } while(token == 0xFF);

//    if(token != DATA_START_BLOCK)
//        return false;

//    Delay1ms(100);

    /* If not valid data token, return error */
    if(!SD_waitForToken( DATA_START_BLOCK, 500 ))
        return false;

    do {
        *(buffer++) = SPI_transfer( spi_module, 0xFF );
    } while(--len);

//    for( uint16_t iii = 0; iii < len; ++iii )
//    {
//        buffer[iii] = SPI_transfer( SPI_driver, 0xFF );
//    }

    /* discard CRC */
    SPI_transfer( spi_module, 0xFF );
    SPI_transfer( spi_module, 0xFF );

//    SPI_setChipSelectState( SPI_driver, HIGH );

    return true;
}

/* @param  token       command token to be sent to the SD card prior to
 *                      sending the data block. The available tokens are:
 *                      DATA_START_BLOCK
 *                      WRITE_MULTIPLE_TOKEN
 *                      STOP_TRAN_TOKEN
 */
static bool SD_dataBlock_send(const byte *buffer, byte token)
{
    uint8_t counter = 0;
    byte response;

    if( SD_waitForToken(0xFF, 500) is false )
        return false;

    SPI_transfer( spi_module, token );

    /* Send data only when token != STOP_TRAN_TOKEN */
    if (token != STOP_TRAN_TOKEN)
    {
        /* Transferring 512 byte blocks using a 8 bit counter */
        do {
            SPI_transfer( spi_module, *buffer++ );
            SPI_transfer( spi_module, *buffer++ );
        } while(--counter);
//        for(uint16_t iii = 0; iii < 512; iii++)
//        {
//            SPI_transfer( spi_module, buffer[iii] );
//        }

        /* discard CRC */
        SPI_transfer( spi_module, 0xFF );
        SPI_transfer( spi_module, 0xFF );

        // Reveive data response
        response = SPI_transfer( spi_module, 0xFF );

        if( (response & DATA_RES_MASK) != DATA_RES_ACCEPTED )
        {
            SPI_setChipSelectState( spi_module, HIGH );
            return false;
        }
    }

    return true;
}

static byte SD_send_cmd(byte cmd, uint32_t arg)
{
    byte CRC;
    byte counter;
    byte result;

    // select the card
    SPI_setChipSelectState(spi_module, LOW);
//    Delay1ms(1);

    // wait up to 300 ms if busy
    SD_waitForToken(0xFF, 300);

//     if (cmd & 0x80)
//     {
//         // ACMD<n> is the command sequense of CMD55-CMD<n>
//         cmd &= 0x7F;
//         result = SD_send_cmd(CMD55, 0);
//         if (result > 1) return result;
//     }

      // Select the card and wait for ready except to stop multiple block read
//     if (cmd != CMD12)
//         SD_waitForToken(0xFF);
//    SPI_transfer(spi_module, 0xFF);
//    Delay1ms(10);
//    SPI_transfer(spi_module, 0xFF);
//    SPI_transfer(spi_module, 0xFF);

    // send packet data
    SPI_transfer( spi_module, cmd );                // send commnad
    SPI_transfer( spi_module, (byte)(arg >> 24) );  // arg[31..24]
    SPI_transfer( spi_module, (byte)(arg >> 16) );  // arg[23..16]
    SPI_transfer( spi_module, (byte)(arg >> 8) );   // arg[15..8]
    SPI_transfer( spi_module, (byte)(arg) );        // arg[7..0]

    if(cmd is CMD0)
        CRC = 0x95;
    else if(cmd is CMD8)
        CRC = 0x87;
    else
        CRC = 0xFF;

    SPI_transfer( spi_module, CRC );

    // Receive command response
    if(cmd == CMD12)
        // skip a byte
        SPI_transfer( spi_module, 0xFF );

    counter = 0xFF;
    do {
        result = SPI_transfer( spi_module, 0xFF );
//        Delay1ms(1);
    } while( (result & 0x80) && --counter );

    return result;
}

static void powerOn()
{
    Delay1ms(10);

    // initialize SPI
//    SPI_driver = Driver_construct(SPI, spi_module);
//    SPI_driver = malloc(sizeof(Driver));
//    SPI_driver->driverName = SPI;
//    SPI_driver->module = spi_module;
//    SPI_driver->config = 0;
//    SPI_driver->port = -1;

    SPI_initAsMaster( spi_module, SPI_SPEED_NOCONFIG, false );

    // set CS High
    // NOTE: this can be done even without SPI be enabled
    SPI_setChipSelectState(spi_module, HIGH);
//    __SPI_setMOSI(spi_module, HIGH);

    /*
     * Configure the SPI bus to 400 kHz as required per SD specs. This frequency
     * will be adjusted later once the SD card has been successfully initialized
     */
    // TODO: MSB first
    SPI_setClkSpeedManually(spi_module, 250E3);
//    GPIO_PORTA_PUR_R  |= 0b01000;
//    GPIO_PORTA_ODR_R  |= 0b10000;
//    GPIO_PORTA_DR4R_R |= 0b11110;

    SPI_enable(spi_module);

//    Delay1ms(1);

    // raise the chip select

    // TODO: drive number should be send instead of the second zero
//    if( f_mount(&fileSystem, "00:\0", 0) != FR_OK)
//    {
//        SPI_deinit(SPI_driver);
//        return RES_ERROR;
//    }

    // TODO: check if there is a card in the socket

    // initialize SD card for SPI
    connect_SD_to_SPI();

    // NOTE: this can be done even without SPI be enabled
//    SPI_setChipSelectState(SPI_driver, HIGH);
}

/*************************************************************************/


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv        /* Physical drive nmuber to identify the drive */
)
{
    return cardState;
}



/*-----------------------------------------------------------------------*
 * Inidialize a Drive
 * Function to initialize the SD Card.  This function is called by the FatFs
 * module and must not be called by the application!
 *-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv                /* Physical drive nmuber to identify the drive */
)
{
    byte OCR[4];

    // No card in the socket
    if( cardState & STA_NODISK )
        return cardState;

    // Force socket power on
    powerOn();

//    SPI_transfer( SPI_driver, 0xFF );
    SPI_setChipSelectState(spi_module, LOW);
//    SPI_holdBus();

    // send CMD0 -> put SD card in `IDLE` state
    if( SD_send_cmd( CMD0, 0 ) is 1 )
    {
        /*
         * Determine what SD Card version we are dealing with
         * Depending on which SD Card version, we need to send different SD
         * commands to the SD Card, which will have different response fields.
         */

        if(SD_send_cmd(CMD8, 0x1AA) is 1)
        {
            // SDC Ver2+
            for( byte iii = 0; iii < 4; iii++ )
                OCR[iii] = SPI_transfer( spi_module, 0xFF );

            /*
             * Ensure that the card's voltage range is valid
             * The card can work at vdd range of 2.7-3.6V
             */
            if( (OCR[2] is 0x01) and (OCR[3]) is 0xAA )
            {
                // TODO: timeout = 1ms
                do {
                    if( (SD_send_cmd(CMD55, 0) <= 1) &&
                        (SD_send_cmd(CMD41, 1UL << 30) == 0) )
                    {
                        break;
                    }
                } while(true);

                /*
                 * Check CCS bit to determine which type of capacity we are
                 * dealing with
                 */
                if( SD_send_cmd(CMD58, 0) is 0 )
                {
                    for( byte iii = 0; iii < 4; iii++ )
                        OCR[iii] = SPI_transfer( spi_module, 0xFF);

                    cardType = (OCR[0] & 0x40) ? __SD_TYPE_SDHC : __SD_TYPE_SDSC;
                }
            }
        }

        /* SDC Ver1 or MMC */
        else
        {
            /*
             * The card version is not SDC V2+ so check if we are dealing with a
             * SDC or MMC card
             */
            if( (SD_send_cmd(CMD55, 0) <= 1) &&
                (SD_send_cmd(CMD41, 0) <= 1))
                cardType = __SD_TYPE_SDSC;

            else
                cardType = __SD_TYPE_MMC;

            // TODO: timeout = 1ms
            do {
                if(cardType is __SD_TYPE_SDSC)
                {
                    /* ACMD41 */
                    if( (SD_send_cmd(CMD55, 0) <= 1) &&
                        (SD_send_cmd(CMD41, 0) == 0))
                        break;

                    /* CMD1 */
                    else
                        if(SD_send_cmd(CMD1, 0) is 0)
                            break;
                }
            } while(true);

            // Select R/W block length
            if( SD_send_cmd(CMD16, SD_SECTOR_SIZE) is_not 0 )
                cardType = __SD_TYPE_NONE;
        }
    }


//    SPI_releaseBus();

    // IDLE (Release D0)
//    SPI_transfer( spi_module, 0xFF );

    // Check to see if a card type was determined
    if(cardType is_not __SD_TYPE_NONE)
    {
        SPI_setClkSpeedManually(spi_module, 8E6);
//        SPI_setClkSpeed(spi_module, SPI_SPEED_FULL);
        cardState &= ~STA_NOINIT;
    }

    SPI_setChipSelectState(spi_module, HIGH);

    return cardState;
}



/*-----------------------------------------------------------------------*
 * Read Sector(s)
 * Function to perform a disk read from the SDCard. This function is called by
 * the FatFs module and must not be called by the application!
 *-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Start sector in LBA */
    UINT count        /* Number of sectors to read */
)
{
    // 0 sectors to read
    if(!count)
        return RES_PARERR;

    // not initialzed
    if(cardState & STA_NOINIT)
        return RES_NOTRDY;

    /*
     * On a SDSC card, the sector address is a byte address on the SD Card
     * On a SDHC card, the sector address is address by sector blocks
     */
    if( cardType is_not __SD_TYPE_SDHC )
        sector *= SD_SECTOR_SIZE;

//    SPI_setChipSelectState( spi_module, LOW );
//    SPI_holdBus();
//    Delay1ms(5);

    // single block read
    if( count EQUAL 1 )
    {
        if( (SD_send_cmd(CMD17, sector) EQUAL 0) &&
            (SD_dataBlock_recieve(buff, SD_SECTOR_SIZE)) )
            count = 0;
    }

    // multiple block read
    else
    {
        if(SD_send_cmd(CMD18, sector) EQUAL 0)
        {
            do {
                if( SD_dataBlock_recieve( buff, SD_SECTOR_SIZE ) is false )
                    break;
                buff += SD_SECTOR_SIZE;
            } while(--count);

            // stop transmition
            SD_send_cmd(CMD12, 0);
        }
    }

    SPI_setChipSelectState( spi_module, HIGH );
//        SPI_releaseBus();

        // Idle (Release D0);
//    SPI_transfer( spi_module, 0xFF );

    return (count ? RES_ERROR : RES_OK);
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    BYTE pdrv,            /* Physical drive nmuber to identify the drive */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Start sector in LBA */
    UINT count            /* Number of sectors to write */
)
{
    // 0 sectors to read
    if(!count)
        return RES_PARERR;

    // not initialzed
    if( cardState and STA_NOINIT)
        return RES_NOTRDY;

    // disk is protected
    if( cardState and STA_PROTECT )
        return RES_WRPRT;

    /*
     * On a SDSC card, the sector address is a byte address on the SD Card
     * On a SDHC card, the sector address is address by sector blocks
     */
    if( cardType is_not __SD_TYPE_SDHC )
        sector *= SD_SECTOR_SIZE;

//    SPI_setChipSelectState( spi_module, LOW );
//    SPI_holdBus();

    // single block read
    if( count EQUAL 1 )
    {
        if( (SD_send_cmd(CMD24, sector) EQUAL 0) and
            (SD_dataBlock_send( buff, DATA_START_BLOCK) ) )
            count = 0;
    }

    // multiple block read
    else
    {
        if( (cardType is __SD_TYPE_SDSC) or (cardType is __SD_TYPE_SDHC) )
        {
            SD_send_cmd(CMD55, 0);
            SD_send_cmd(CMD23, count);
        }

        if(SD_send_cmd(CMD25, sector) EQUAL 0)
        {
            do {
                if(!SD_dataBlock_send(buff, WRITE_MULTIPLE_TOKEN))
                    break;
                buff += SD_SECTOR_SIZE;
            } while(--count);

            // `STOP_TRAN` token
            if( !SD_dataBlock_send(0, STOP_TRAN_TOKEN) )
                count = 1;
        }
    }

    SPI_setChipSelectState( spi_module, HIGH );
//        SPI_releaseBus();

        // Idle (Release D0);
//    SPI_transfer( spi_module, 0xFF);

    return (count ? RES_ERROR : RES_OK);
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    DRESULT result = RES_ERROR;
    byte CSD[16];
    WORD size;
    byte num;

    if(cardState and STA_NOINIT)
        return RES_NOTRDY;

//    SPI_setChipSelectState(spi_module, LOW);
//    SPI_holdBus();

    switch(cmd)
    {
        case GET_SECTOR_COUNT:
            // Get number of sectors on the disk (uint32_t)
            if( (SD_send_cmd(CMD9, 0) EQUAL 0) and
                (SD_dataBlock_recieve(CSD, 16)) )
            {
                /* SDC ver 2.00 */
                if( (CSD[0] >> 6) EQUAL 1)
                {
                    size = CSD[9] + ( (WORD)CSD[8] << 8 ) + 1;
                    *(uint32_t *)buff = (uint32_t)size << 10;
                }

                /* MMC or SDC ver 1.XX */
                else
                {
                    num = ( CSD[5] & 15 ) +
                          ( (CSD[10] & 128) >> 7 ) +
                          ( (CSD[9] & 3) << 1 ) + 2;

                    size = ( CSD[8] >> 6 ) +
                           ( (WORD)( CSD[7] << 2 ) ) +
                           ( (WORD)(CSD[6] & 3) << 10 ) + 1;

                    *((uint32_t *)buff) = (uint32_t)size << (num - 9);
                }

                result = RES_OK;
            }
            break;

        case GET_SECTOR_SIZE:
            // Get sectors on the disk (WORD)
            *(WORD*)buff = SD_SECTOR_SIZE;
            result = RES_OK;
            break;

        case CTRL_SYNC:
            // Make sure that data has been written
            if(SD_waitForToken(0xFF, 500) is true)
                result = RES_OK;
            else
                result = RES_NOTRDY;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    SPI_setChipSelectState( spi_module, HIGH );
//    SPI_releaseBus();

    // Idle
//    SPI_transfer( spi_module, 0xFF);

    return result;
}

WCHAR ff_convert (WCHAR chr, UINT dir)
{
    return chr;
}

WCHAR ff_wtoupper (WCHAR chr)
{
    return chr;
}

void* ff_memalloc (UINT msize)
{
    return malloc( msize );
}

void ff_memfree (void* mblock)
{
//    free(mblock);
}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */

DWORD get_fattime (void)
{
    return    ((2007UL-1980) << 25)     // Year = 2007
            | (6UL << 21)               // Month = June
            | (5UL << 16)               // Day = 5
            | (11U << 11)               // Hour = 11
            | (38U << 5)                // Min = 38
            | (0U >> 1)                 // Sec = 0
            ;

}
