
/*
 * File:    applicationControl.c
 * j.z.
 *  
 * MPLABX.IDE v5.10, XC32 v3.00, HW: DEP-PIC32-MK
 * 
 * Created:  2022.12.19
 * Modified: 2023.02.13
 * 
 */

/*==== History trace buffer ===================================================

2023.02.10-New: applicationControl.c TEMPLATE for students DEP 
==== History trace buffer END ================================================*/

//--- Includes ----------------------------------------------------------------
#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "./../header/platformDEP32mk.h"
#include "./../header/filtrace.h"
#include "./../header/dekoder.h"
#include "./../header/pamet.h"
#include "./../header/PWM.h"


//-- platform Function prototypes are in "platrformDEP32mk" ---------------------

//--- User function prototypes -------------------------------------------------


//--- Macros ------------------------------------------------------------------


//--- Constants ---------------------------------------------------------------

//--- New variable types ------------------------------------------------------
//deklarace promennych



//--- Global vars -------------------------------------------------------------    
    FILTR_TLACITKA dekoderStopaA; //zakladam promennou typu FILTR_TLACITKA s nazvem dekoderStopaA, je to promenna tohoto typu, kdy se mi zalozi vse co obsahuje tato struktura
    FILTR_TLACITKA dekoderStopaB; //-||-
    FILTR_TLACITKA tlacitkoS4;//-||-
    DEKODER vystupDekoderu;//-||-
    DETEKCE_HRANY pametS4;//-||-
    ZATEZOVATEL zat;//-||-

//--- External vars -----------------------------------------------------------

//---- Functions --------------------------------------------------------------

void configApplication(void){//------------------------------------------------
  //--- User defined functions ---//deje se to jen jednou na zacatku (davam tam to co chci volat jen jednou) - inicializace, statovani timeru
   //dekoderStopaA.stav=0; //kdybych nemel funkce, tak bych musel takto inicializovat jednotlive promenne struktury (vzdy napisu nazev promenne a teckou vstupim dovnitr do struktury)
   initFiltr(&dekoderStopaA, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&dekoderStopaB, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&tlacitkoS4, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initDekoder(&vystupDekoderu, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initPametTlacitka(&pametS4, 0);
   initZat(&zat, 0, 0, 0); //inicializace struktury obsahujici zatezovatele
   initPWM(); //nastavim periferie pro PWM
  
}// configApplication() END 

//===>>>> Call it every 1ms <<<<===

void runApplication(void) {//je volanou kazdou 1ms v platformMainMK sem pisu moje funkce
  //volani funkci na filtraci pro dekoder
  runFiltr(&dekoderStopaA,getCoderChannelA()); //volam si filtr kdy parametrem (vstupem) je adresa struktury dekoderstopaA a druhym vstupem je funkce ktera mi cita vstupy
  runFiltr(&dekoderStopaB,getCoderChannelB());//
  setCoderLedA(getFiltrOutput(&dekoderStopaA)); //volam si funkci, ktera mi rozsviti led kdyz doslo k filtraci tlacitka na kanalu A
  setCoderLedB(getFiltrOutput(&dekoderStopaB)); //volam si funkci, ktera mi rozsviti led kdyz doslo k filtraci tlacitka na kanalu B
  //volani funkci na kvadraturni dekoder
  runDekoderSmeru(&vystupDekoderu, getFiltrOutput(&dekoderStopaA), getFiltrOutput(&dekoderStopaB));
  runSignalizaceMaxDekoderu(&vystupDekoderu);
  runSignalizaceMinDekoderu(&vystupDekoderu);
  
 //volani funkci na tlacitko S4
  runFiltr(&tlacitkoS4,getButtonS4());
  runPametTlacitka(&pametS4, getFiltrOutput(&tlacitkoS4));
  signalizaceLED(&pametS4, getPrepocetDekoderu(&vystupDekoderu), &zat);//siganlizace led, funkce rozsvici ledky na zaklade nacteni dekoderu nebo potaku
  
 //volani funkci pro PWM
 runPWMPrepoctiAPredej(getZatezovatel(&zat, &pametS4)); //do funkce poslu hodnotu zatezovatele na zaklade prepinace
 setTestPinIRCAasPwmOutput();//kontrola vystupu PWM 
  
  
  
  
  

}// runApplication() END)

//--- applicationControl.c file END -------------------------------------------


