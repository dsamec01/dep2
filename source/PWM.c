/* ************************************************************************** */

#include <proc/PIC32MK-MC/p32mk1024mcf100.h>

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "./../header/PWM.h"
#include <xc.h>
#include <sys/attribs.h>
#include "./../header/pamet.h"
#include "./../header/platformDEP32mk.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
int casJednaPrenos = 0; //mam globalni promennou, do ktere zapisuji pocet casu, kdy mam mit pulz v 1

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
void initPWM(void){
    //incializace timeru najdu v 15-1
    //T4CONCLR=0xFFFFFFF;
    T4CONbits.ON=0; //na zacatku behem nastavovani mam timer vyply
    T4CONbits.SIDL=0; //nepouzivam
    T4CONbits.TGATE=0; //chci 0 protoze chci posilat hodnotu procesoru
    T4CONbits.TCKPS=0;//nepozivam preddelicku
    T4CONbits.T32=0; //beru 16 bitu, pac se mi to tam vejde (?????abych nemusel spojovat dva timery pro Compare, ktera je 32 bitova??????)
    T4CONbits.TCS=0; //chci posilat frekvenci procesoru
    
    TMR4=0; //vynuluji casovy registr
    PR4= timeis100mikro - 1; //musim odecist 1, jelikoz citam od 0 - do tolika mi bude citat period regist - odpovida to hodnote preteceni timeru
    
    //inicializace Interrupt - hodnoty k preruseni najdu v tabulce 8-3
    IFS0bits.T4IF = 0; //Interrupt flag nastavuji na 0, nemam na zacatku flag
    IEC0bits.T4IE = 1; //Povoluji preruseni
    IPC4bits.T4IP = 4; //nastavuji prioritu na 4
    IPC4bits.T4IS = 3; //nastavuji sub-prioritu na 3

    //inicializace Compare registru - najdu v 19-1
    OC1CONbits.ON=0; //Behem nastavovani mam comapre unit vyplou
    OC1CONbits.SIDL=0; //Nepouzivam
    OC1CONbits.OC32=0; //Pouzivam 16 bit timery, takze OCxR a OCxRS se pouzivaji pro porovnani s 16-bit timery (pokud by T32 = 1 tak musim mit OC32 = 1)
    OC1CONbits.OCFLT=0; //Nepouzivam
    OC1CONbits.OCTSEL=0; //Timerx je zdrojem casovace
    OC1CONbits.OCM = 0b110; //Pouzivam PWM mode 
    OC1R = PERIOD_PUL; //nastavim compare registr, abych mel stredni hodnotu na zacatku nulovou (aby se mi motor netocil ani na jednu stranu) -> tato hodnota tam vlastne bude na zacatku
    OC1RS = PERIOD_PUL; //nastavim druhy Compare registr na nejakou hodnotu, ale je to jedno. Prepise se v prvnim cyklu
    
    //na konci kdyz mam inicializaci, poustim casovac a compare jednotku
    T4CONSET=_T4CON_ON_MASK; //pustim timer
    OC1CONSET = _OC1CON_ON_MASK; //compare jednotku pustim
}



void __ISR(_TIMER_4_VECTOR, IPL4SOFT) T4_IntHandler(void){ //casovac nacita jednu periodu, vyvola preruseni
 //v preruseni mu reknu jak dlouho ma byt v 1 na zaklade prepoctu, ktery si udelam v mainu a hodim FLAG do 0
    //OC1R v PWM potom vubec nenastavuji, automaticky se do nej predava //popis viz 16.3.3 - po inicializaci se z OC1R stava read-only
    OC1RS = casJednaPrenos; //funguje to tak, ze ja to poslu do OC1RS, pak se mi to presune do OC1R, ten mi nastavi 1 a po nacitani patricneho zatezovatele mi to hodi do 0
    IFS0CLR = _IFS0_T4IF_MASK;
} 

int* getPtrCasJednaPrenos(){ //predam pointer na int
    return &casJednaPrenos; //pokud budu chtit v Application control predat, tak si zavolam funkci a ta mi vrati pointer na int, tedy mi vrati vlastne adresu
} //pracuji s tim jako s pointerem, je to pointer, kdyz to budu predavat funkci, tak musi cekat pointer, ne hdonotu

void runPWMPrepoctiAPredej(int zatezovatel){
    long casJedna = 0;
    casJedna = (PERIOD_MIN - PERIOD_PUL)*zatezovatel; //prepocitam na tiky kdy ma byt v 1
    casJedna=casJedna/ZAT_MIN+PERIOD_PUL; //stale prepocet
    IEC0CLR = _IEC0_T4IE_MASK; //zakazu Interrupt
    casJednaPrenos = casJedna; //predam do globalni promenne
    IEC0SET = _IEC0_T4IE_MASK; //povolim Interrupt
}



/* *****************************************************************************
 End of File
 */
