
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
#include "./../header/messengerDEP.h"
#include "./../header/Hrad_pole.h"
//#include "./../header/Caputre.h" zahrnuto v pamet.h
#include "./../header/protoypRTM.h"

//-- platform Function prototypes are in "platrformDEP32mk" ---------------------

//--- User function prototypes -------------------------------------------------


//--- Macros ------------------------------------------------------------------


//--- Constants ---------------------------------------------------------------

//--- New variable types ------------------------------------------------------
//deklarace promennych



//--- Global vars -------------------------------------------------------------    
    FILTR_TLACITKA dekoderStopaA; //zakladam promennou typu FILTR_TLACITKA s nazvem dekoderStopaA, je to promenna tohoto typu, kdy se mi zalozi vse co obsahuje tato struktura
    FILTR_TLACITKA dekoderStopaB; //-||-
    FILTR_TLACITKA tlacitkoS3;//-||-
    FILTR_TLACITKA tlacitkoS4;//-||-
    FILTR_TLACITKA tlacitkoS5;//-||-
    FILTR_TLACITKA tlacitkoS6;//-||-
    FILTR_TLACITKA tlacitkoS7;//-||-
    FILTR_TLACITKA tlacitkoS8;//-||-
    DEKODER vystupDekoderu;//-||-
    DETEKCE_HRANY pametS3;//-||-
    DETEKCE_HRANY pametS4;//-||-
    DETEKCE_HRANY pametS5;//-||-
    DETEKCE_HRANY pametS6;//-||-
    ZATEZOVATEL zat;//-||-
    bool prepinac_RTM; //zalozim si boolovskou promennou pomoci ktere budu prepinat switch u RTM
    CAPTURE_RTM captureRTM;
    PRECH_CHAR PrechCharData;
    

//--- External vars -----------------------------------------------------------

//---- Functions --------------------------------------------------------------

void configApplication(void){//------------------------------------------------
  //--- User defined functions ---//deje se to jen jednou na zacatku (davam tam to co chci volat jen jednou) - inicializace, statovani timeru
   //dekoderStopaA.stav=0; //kdybych nemel funkce, tak bych musel takto inicializovat jednotlive promenne struktury (vzdy napisu nazev promenne a teckou vstupim dovnitr do struktury)
   initFiltr(&dekoderStopaA, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&dekoderStopaB, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&tlacitkoS3, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&tlacitkoS4, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&tlacitkoS5, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&tlacitkoS6, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&tlacitkoS7, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initFiltr(&tlacitkoS8, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initDekoder(&vystupDekoderu, 0); //volam si funkci, kdy vstupem je adresa struktury a zaroven si nastavim pocatecni hodnotu
   initPametTlacitka(&pametS3, 0);
   initPametTlacitka(&pametS4, 0);
   initPametTlacitka(&pametS5, 0);
   initPametTlacitka(&pametS6, 0);
   initZat(&zat, 0, 0, 0); //inicializace struktury obsahujici zatezovatele
   initPWM(); //nastavim periferie pro PWM
   initPrepinacRTM(&prepinac_RTM); //inicializuji si prepinac RTM
   initCapture();
   initVypocetOtacekASmeru(&captureRTM);
   initPrechChar(&PrechCharData);
  
  
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
  
 //volani funkci na tlacitko S3
  runFiltr(&tlacitkoS3,getButtonS3());
  runPametTlacitka(&pametS3, getFiltrOutput(&tlacitkoS3));
  runIRCneboPWM(&pametS3); 
  
  //volani funkci na tlacitko S4
  runFiltr(&tlacitkoS4,getButtonS4());
  runPametTlacitka(&pametS4, getFiltrOutput(&tlacitkoS4));
  signalizaceLED(&pametS4, getPrepocetDekoderu(&vystupDekoderu), &zat);//siganlizace led, funkce rozsvici ledky na zaklade nacteni dekoderu nebo potaku
  
 //volani funkci pro PWM
 runPWMPrepoctiAPredej(getZatezovatel(&zat, &pametS4, &prepinac_RTM, &PrechCharData)); //do funkce poslu hodnotu zatezovatele na zaklade prepinace
 
  
 //volani funkci pro tlacitko S5
 runFiltr(&tlacitkoS5,getButtonS5());
 runPametTlacitka(&pametS5, getFiltrOutput(&tlacitkoS5));
 
 //volani funkci pro tlacitko S6
 runFiltr(&tlacitkoS6,getButtonS6());
 runPametTlacitka(&pametS6, getFiltrOutput(&tlacitkoS6));
 
 //volani funkci pro tlacitko S7
 runFiltr(&tlacitkoS7,getButtonS7());
 
 //volani funkci pro tlacitko S8
 runFiltr(&tlacitkoS8,getButtonS8());
  
 //prepinac k Hradlovemu poli s vypocty Capture jednotky
 runHradPole(getPametTlacitkaOutput(&pametS5), getPametTlacitkaOutput(&pametS6), getFiltrOutput(&tlacitkoS7), getFiltrOutput(&tlacitkoS8));
 runVypocetOtacekASmeru(&captureRTM);
 
 //volani funkce pro chod prechodove charakteristiky
 runPrechodChar(&PrechCharData, &captureRTM);
 
 //RTM monitor
 runKomunikaceRTM(&zat, getZatezovatel(&zat, &pametS4, &prepinac_RTM,&PrechCharData), &prepinac_RTM, &captureRTM, &PrechCharData); //do funkce poslu strukturu se zatezovatelema do ktery budu ukladat, zaroven tam poslu hodnotu vysledneho zatezovatele ktery budu odesilat a posilam tam hodnotu do ktere budu cist v jake fazi je prepinac u RTM
 
}// runApplication() END)

//--- applicationControl.c file END -------------------------------------------


