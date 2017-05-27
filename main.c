///*
//* Description : Dummy file - red led is HIGH
//* Usage       : using for purpose of testing
//* Author      : Baron Leonardo <mohamedayman.fcis@zoho.com>
//* License     : Copyright (C) 2016 RTOS From Scratch
//*
//*
//* This program is free software: you can redistribute it and/or modify
//* it under the terms of the GNU General Public License as published by
//* the Free Software Foundation, either version 3 of the License, or
//* (at your option) any later version.
//*
//* This program is distributed in the hope that it will be useful,
//* but WITHOUT ANY WARRANTY; without even the implied warranty of
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//* GNU General Public License for more details.
//*
//* You should have received a copy of the GNU General Public License
//* along with this program.  If not, see <http://www.gnu.org/licenses/>.
//*/

//#include "src/ff.h"
//#include "src/PLL.h"
//#include "Drivers/src/GPIO.h"
//#include "Drivers/src/UART.h"
//#include <stdlib.h>
//#include <string.h>

//FRESULT scan_files (
//    char* path        /* Start node to be scanned (***also used as work area***) */
//)
//{
//    FRESULT res;
//    DIR dir;
//    UINT i;
//    static FILINFO fno;


//    res = f_opendir(&dir, path);                       /* Open the directory */
//    if (res == FR_OK) {
//        for (;;) {
//            res = f_readdir(&dir, &fno);                   /* Read a directory item */
//            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
//            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
//                i = strlen(path);
//                SYS_UART_writeLine((byte *)fno.fname);
//                SYS_UART_write('\n');
//                res = scan_files(path);                    /* Enter the directory */
//                if (res != FR_OK) break;
//                path[i] = 0;
//            } else {                                       /* It is a file. */
//                SYS_UART_writeLine((byte *)fno.fname);
//                SYS_UART_write('\n');
//            }
//        }
//        f_closedir(&dir);
//    }

//    return res;
//}

//FATFS FatFs;

//int main()
//{
//    SYS_UART_init();

//    Driver *red = Driver_construct( GPIO, F1 );
//    GPIO_init( red, OUTPUT, true );
//    Driver *blue = Driver_construct( GPIO, F2 );
//    GPIO_init( blue, OUTPUT, true );
//    Driver *green = Driver_construct( GPIO, F3 );
//    GPIO_init( green, OUTPUT, true );

//   FATFS *FatFs = malloc(sizeof(FatFs));		/* FatFs work area needed for each volume */
//    FIL Fil;
//    UINT bytes_written;
//    UINT bytes_read;
//    byte buffer[50];

//    if( f_mount(&FatFs, "", 1) is FR_OK )
//    {
//        GPIO_write( blue, HIGH );
//        scan_files("/sara");

//        if(f_chdir("/sara") is FR_OK)
//        {
//            GPIO_write( blue, LOW );
//            GPIO_write(red, HIGH);
//           SYS_UART_writeLine(buffer);
//        }

//        if( f_open(&Fil, "sara.txt", FA_WRITE | FA_OPEN_APPEND) is FR_OK )
//        {
//            if(f_write(&Fil, "and she loves you much more\n", 50, &bytes_written) is FR_OK)
//            {
//                f_sync(&Fil);
//                f_close(&Fil);
//                f_open(&Fil, "sara.txt", FA_READ);
//                if( f_read(&Fil, buffer, 50, &bytes_read) is FR_OK)
//                {
//                    GPIO_write( blue, LOW );
//                    GPIO_write(green, HIGH);
//                    SYS_UART_writeLine(buffer);
//                    f_close(&Fil);
//                }
//            }
//        }

//        f_mount(NULL, "", 0);
//        while(1);
//    }

//   if( f_mount(FatFs, "", 0) is FR_OK )
//   {
//       GPIO_write(green, HIGH);

////        if( scan_files("/") is FR_OK )
////            GPIO_write(blue, HIGH);

////        if( f_open(&Fil, "sara.txt", FA_WRITE | FA_CREATE_ALWAYS) is FR_OK )
////        {
////            GPIO_write(green, LOW);
////            GPIO_write(blue, HIGH);
////            if(f_write(&Fil, "It worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt worksIt wo\r\n", 512, &bytes_written) is FR_OK)
////            {
////                f_sync(&Fil);
////                GPIO_write(green, HIGH);
////                f_mount(NULL, "", 0);
////                f_close(&Fil);
////            }

////            else
////            {
////                GPIO_write(green, LOW);
////                GPIO_write(blue, LOW);
////                GPIO_write(red, HIGH);
////                f_mount(NULL, "", 0);
////            }
////    ////        f_read( &Fil, buffer, 200, &bytes_read );
////    ////        SYS_UART_writeLine( buffer );
////            while(1);
////        }

////        if( f_open(&Fil, "/TEST.TXT", FA_READ) == FR_OK )
////        {
////            char buffer[200];
////            GPIO_write( blue, HIGH );
////            f_read( &Fil, buffer, 20, &bytes_read );
//////            f_gets(buffer, 200, &Fil);
////            SYS_UART_writeLine( (byte*)buffer );
////            f_close(&Fil);
////            f_mount(NULL, "", 0);
////            while(1);
////        }

//       f_mount(NULL, "", 0);

////        while(true);
//   }

//   else
//       GPIO_write(red, HIGH);


//    while(1)
//    {
//       GPIO_write(green, LOW);
//       GPIO_write(blue, LOW);
//       GPIO_write(red, HIGH);
//    }

//    return 0;
//}
