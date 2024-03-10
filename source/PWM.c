/* ************************************************************************** */

#include <proc/PIC32MK-MC/p32mk1024mcf100.h>

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "./../header/PWM.h"


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */




/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
void initPWM(void){
    //incializace timeru
    T4CONbits.ON=0; //na zacatku mam timer vyply
    T4CONbits.SIDL=0; //nepouzivam
    T4CONbits.TGATE=0; //chci 0 protoze chci posilat hodnotu procesoru
    T4CONbits.TCKPS=0;//nepozivam preddelicku
    T4CONbits.T32=0; //beru 16bit timer, do toho se mi hodnota vejde
    T4CONbits.TCS=0; //chci posilat frekvenci procesoru
    
    TMR4=0; //vynuluji casovy registr
    PR4= timeis100mikro - 1; //musim odecist 1, jelikoz citam od 0 - do tolika mi bude citat period regist
    
    //inicializace Interrupt - hodnoty k preruseni najdu v tabulce 8-3
    IFS0bits.T4IF = 0; //Interrupt flag nastavuji na 0, nemam na zacatku flag
    IEC0bits.T4IE = 1; //Povoluji preruseni
    IPC4bits.T4IP = 4; //nastavuji prioritu na 4
    IPC4bits.T4IS = 3; //nastavuji sub-prioritu na 3

    //inicializace Compare registru - najdu v 19-1
    OC1CONbits.ON=0; //Povolim Compare unit
    OC1CONbits.SIDL=0; //Nepouzivam
    OC1CONbits.OC32=0; //Pouzivam 16 bit timery
    OC1CONbits.OCFLT=0; //Nepouzivam
    OC1CONbits.OCTSEL=0; //Timerx je zdrojem casovace
    OC1CONbits.OCM = 0b110; //Pouzivam PWM mode
    
    
    //na konci poustim casovac a compare jednotku
    T4CONbits.ON=1; //timer pustim
    OC1CONbits.ON=1; //compare jednotku pustim
}


/*
void _ISR(_TIMER_4_VECTOR, IPL4SOFT) timer4Handler(void){

    IFS0bits.T4IF=0;
}*/


/* *****************************************************************************
 End of File
 */
