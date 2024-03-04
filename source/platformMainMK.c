/*
 * Project: platformRefDemo.X
 * j.z.
 * 
 * The main() function is here
 * 
 * MPLABX.IDE v5.10, XC32 v3.00, HW: DEP-PIC32-MK, fcyc = 120 MHz
 * 
 * Created:  2023.02.19
 * Modified: 2023.03.10
 * 
 * 
 * DEBUGGED
 */

/*=== History trace buffer ===================================================-

2023.02.19-Modified: Inserted __builtin_enable_interrupts(),
            disble interupts (i.e. __builtin_disable_interrupts()) is newly in
            configCPU32mk(void).
2023.02.19-Renamed:platformInCaptureOutCompareDemo.X copied and renamed
            to platformRefDemo.X project.
2023.02.03-Added: Input capture functions.
2023.02.02-Added: Output compare functions. 
2023.02.02-Added: ADC functions.
2022.12.01-Added: User defined Timer2 tick.
2023.01.15-New: applicationControl project
===== History trace buffer END ===============================================*/

#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "./../header/configCPU32mk.h"
#include "./../header/platformDEP32mk.h"

//------ Compilation control --------------------------------------------------
//#define AUTO

#ifndef AUTO
#define USER
#endif

//------ Externals ------------------------------------------------------------

//------ Function prototypes --------------------------------------------------

//------ Macros ---------------------------------------------------------------

//------ Consstants -----------------------------------------------------------

//------ Global vars ----------------------------------------------------------

int main(void) {//-------------------------------------------------------------
    //=== Call the functions in this order ===
    configCPU32mk();
#ifdef AUTO  
    configPlatformAutoMode();
#else
    configPlatformUserMode();
#endif
    configApplication();
    __builtin_enable_interrupts();
    Nop();
    //--- Background loop ---
    while (true) {
        while (is1ms()) {
            runPlatform();
            runApplication(); //vola se kdyz dojde k 1ms
        }// is1ms END
    }
}// main() END)

//--- platformMainMK.c file END ------------------------------------------------
