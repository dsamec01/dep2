/* ************************************************************************** */

#include <proc/PIC32MK-MC/p32mk1024mcf100.h>

/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <xc.h>
#include <limits.h> //knihovna ve ktere jsou mezni hodnoty promennych
#include <sys/attribs.h>
#include <stdbool.h>
#include "./../header/platformDEP32mk.h"
#include "./../header/Caputre.h"

unsigned int periodaTickPrenos = 0; //chci aby byla unisgned aby pretekla spolu s timerem - slouzi pro prenos z preruseni do fce
char smerOtaceniPrenos = 0; //slouzi pro prenos z preruseni do fce
int TMR9Preteceni = 0; //pomoci teto hodnoty urciuji zda se mi motorek otaci nebo nikoliv
bool stopaB = 0; //do teto hodnoty ukladam hodnotu z kanaluB
unsigned int CasNew = 0; //do teto hodnoty budu ukladat hodnoty z bufferu -  //musi byt unsigned aby promenna pretekala soucasne s timerem
unsigned int CasOld = 0; //do teto hodnoty budu ukladat hodnoty z bufferu - msui byt unsigned aby promenna pretekala soucasne s timerem (mam ted oboje 32 bit)
bool prvniPreruseniIC7 = 1; //zajistuje mi zda mam prvni preruseni (po uvedeni do chodu z klidu)
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
//inicializace struktury

void initVypocetOtacekASmeru(CAPTURE_RTM *Ptr_CaptureRTM) { //inicializace struktury
    Ptr_CaptureRTM->otacky = 0; //jelikoz mam pointer musim predat pomoci sipek, kdy stavu prirazuji pocatecni hodnotu
    Ptr_CaptureRTM->perioda = 0;
    Ptr_CaptureRTM->smerOtaceni = 0;
};


//inicializace Capture

void initCapture(void) {
    //nastaveni Timer pro mereni
    //nastavim T6CON - jelikoz beru 32 bitu (spojuji Timery) tak nastavuji Timer6, ale preruseni atd. mi vyvolava Timer7, tedy IF, IE, IP nastavuji pro Timer7 
    T6CONbits.ON = 0; //behem nastaveni nuluji
    T6CONbits.SIDL = 0; //nepouzivam
    T6CONbits. TGATE = 0; //chci 0 protoze chci posilat hodnotu procesoru
    T6CONbits.TCKPS = 0; //nepozivam preddelicku
    T6CONbits.T32 = 1; //beru 32 bitu (spojuji Timer6 a Timer7) 
    T6CONbits.TCS = 0; //chci posilat frekvenci procesoru          
    TMR6 = 0; //vynuluji casovy registr
    PR6 = 0xFFFFFFFF; //nactu PR6 a PR7 32 bitovou hodnotou (2 na 32)
    //inicializace Interrupt - hodnoty k preruseni najdu v tabulce 8-3 - od tohoto timeru zakazuji preruseni (pouzivam ho pouze jako referencni)
    IFS2bits.T7IF = 0; //Interrupt flag nastavuji na 0, nemam na zacatku flag
    IEC2bits.T7IE = 0; //Zakazuji preruseni - nebudu ho vubec pouzivat - priorita nehraje roli nebot nepouzivam preruseni

    //nastavuji Timer pro chybovy hlaseni
    //nastavim T9CON - staci mi 16 bitu -> pokud ma 1:1 pretece za 1,1ms jelikoz ale chci max periodu 20ms tak potrebuji preddelicku 1:32  
    T9CON = 0x0; //Zastavim timer a vynuluji vsechny registry - nepouzivam preddelicku
    T9CONbits.TCKPS = 0b101; //ve vynulovanem registru nastavim pouze preddelicku na 1:32 -> diky tomu mi to pretece kazdych 32x1,1 ms
    TMR9 = 0; //vynuluji casovy registr
    PR9 = 0xFFFF; //nactu hodnotu do period registru - odpovida to 2na 16 - coz odpovida tomu ze to pretece za 1ms
    //inicializace Interruptu pro timer 9 - hodnoty najdu v tabulce 8-3
    IFS2bits.IC9IF = 0; //Interrupt flag nastavuji na 0, nemam na zacatku flag
    IPC22bits.T9IP = 4; //nastavuji prioritu na 4
    IPC22bits.T9IS = 1; //nastavuji sub-prioritu na 1
    IEC2bits.T9IE = 1; //Povoluji preruseni

    //nastavuji Capture Unitu 7
    IC7CONbits.ON = 0; //behem nastavovani mam jednotku vyplou
    IC7CONbits.SIDL = 0; //nepouzivam
    IC7CONbits.FEDGE = 0; //nepouzivam
    IC7CONbits.C32 = 1; //pouzivam 32 bitovy timer
    IC7CONbits.ICTMR = 0; //nepouzivam - pokud bych pouzival 16 bitovy casovac tak bych si volil mezi dvema sousednima casovacema, ktery bude jako referencni
    IC7CONbits.ICI = 0b00; //chci preruseni na kazdou nabeznou hranu
    IC7CONbits.ICM = 0b011; //pouzivam rezim detekce nabeznych hran
    //nastavim referencni casovac pro Compare Unitu - tabulka 18-1 (nastavuji ze pouzvam 32 bitovy timer6)
    // CFGCONbits.ICACLK=1;  //toto je nastaveno v PlatformDEP, jinak bych musel nastavit
    //nastaveim preruseni od Capture Unit
    IFS2bits.IC7IF = 0; //shazuji Interrup flag
    IPC20bits.IC7IP = 4; //nastavuji prioritu na 4
    IPC20bits.IC7IS = 2; //nastavuji subprioritu na 2
    IEC2bits.IC7IE = 1; //povoluji preruseni
    //nastavim pro jistotu i priority pro chybovy stav (IC7IPE, IC7ISE atd.) ackoliv mi to bude zarizovat Timer9
    IFS2bits.IC7EIF = 0; //shazuji Interrup flag
    IPC20bits.IC7EIP = 6; //nastavuji prioritu na 6
    IPC20bits.IC7EIS = 1; //nastavuji subprioritu na 1
    IEC2bits.IC7EIE = 1; //povoluji preruseni

    unsigned int zahod = 0;
    while (IC7CONbits.ICBNE != 0) { //vycistim buffer pri inicializaci abych mel jistotu ze tam nic neni
        zahod = IC7BUF;
    }

    //nakonec spoustim timery a Capture
    T6CONbits.ON = 1;
    T9CONbits.ON = 1;
    IC7CONbits.ON = 1;

}

void __ISR(_INPUT_CAPTURE_7_VECTOR, IPL4SOFT) IC7_IntHandler(void) { //po detekci jedne hrany mi to vyvola preruseni
    unsigned int zahod = 0; //pomoci teto promenne budu zahazovat hodnoty
    
    //vypoet periody
    CasOld = CasNew; //ukladam do stare hodnoty predchozi hodnotu - vzdy pri dalsim preruseni se mi ta hodnota prepise (po prvnim prichodu mam 0 a v dalsim prichodu do preruseni prepisuji)
    stopaB = getIrcChannelBValue(); //do stopyB si nactu hodnotu ze stopyB
    CasNew = IC7BUF; //nacitam novou hodnotu z bufferu
    //urcim rozdil hodnot
    if (prvniPreruseniIC7 == 1) { //pokud mam prvni preruseni po stavu kdy se motor netocil tak periodu nepocitam
        while (IC7CONbits.ICBNE != 0) { //vycistim buffer - po tom co mam prvni preruseni tak vycistim buffer a jdu od znova 
            zahod = IC7BUF;
        }
        prvniPreruseniIC7 = 0; //shodim do 0 - uz mi bude chodit do elsu
    } else {
        periodaTickPrenos = CasNew - CasOld; //pokud nemam prvni preruseni po stavu kdy se motor netocil tak normalne pocitam rozdil dob - jelikoz mi to preteka zpet, tak ani kdyz mam dva casy blizko u sebe (nevejde se mi to do 32bitu) tak nemusim resit pretypovani na 64 bitu a pretece mi to zpet do 32 bitu (jinak bych resil ze pokud casNew> casOld tak jed normalne a v opacnem pripade bych odecetl casOld od maxima timeru a pricetl k tomu casNew)
    }

    //urceni smeru otaceni - hodnota 1 = kladny/-1 = zaporny
    if (stopaB == 1) { //pokud mam hranu na A (nastalo preruseni) a zaroven mam B v 1 tak se mi to otaci po smeru (clockwise)
        smerOtaceniPrenos = 1; //otacim se po smeru (mam 1)
    } else { //pokud mam hranu na A (nastalo preruseni) a zaroven mam B v 0 tak se mi to otaci proti smeru (counterclockwise)
        smerOtaceniPrenos = -1; //otacim se po smeru (mam -1)
    }
    TMR9Preteceni = 0; //uz se mi motor otaci
    IFS2CLR = _IFS2_IC7IF_MASK; //shazuji flag capture jednotky atomicky
}

void __ISR(_INPUT_CAPTURE_7_ERROR_VECTOR, IPL6SOFT) IC7IntErrorHandler(void) {//po nacitani mi to vynuluje buffer
    unsigned int zahod = 0; //pomoci teto promenne budu vyhozvat hodnoty z bufferu v pripade poruchy
    while (IC7CONbits.ICBNE != 0) { //dokud registr nebude vynulovany tak nacitej do zahod, cimz zahodis tuto hodnotu
        zahod = IC7BUF;
    }
    IFS2CLR = _IFS2_IC7EIF_MASK; // Interrupt flag status bit shazuji atomicky 
}

void __ISR(_TIMER_9_VECTOR, IPL4SOFT) T9_IntHandler(void) { //casovac nacita jednu periodu, vyvola preruseni - pri preruseni kontroluji zda mam zaroven plny buffer
    unsigned int zahod = 0; //pomoci teto promenne budu vyhozvat hodnoty z bufferu v pripade poruchy
    TMR9Preteceni++; //pricita se kazde preruseni (po 32ms)
    if (IC7CONbits.ICOV == 1) { //pokud dojde k vyvolani preruseni od timeru 9 (vim ze mam 32ms) a zaroven mi to pretece, mam chybovy stav tak registr musim vynulovat
        while (IC7CONbits.ICBNE != 0) { //dokud registr nebude vynulovany tak nacitej do zahod, cimz zahodis tuto hodnotu
            zahod = IC7BUF;
        }
    }
    IFS2CLR = _IFS2_T9IF_MASK; //shazuji flag atomicky
}

void runVypocetOtacekASmeru(CAPTURE_RTM *Ptr_CaptureRTM) {
    //nastaveni lokalnich promennych
    unsigned int tmpPeriodaTick = 0; //sem budu ukladat hodnoty z tiku
    static int tmpOtacky = 0; //do teto hodnoty budu ukladat hodnotu otacek
    char tmpSmerOtaceni = 0; //do teto hodnoty budu ukladat smer

    //urceni periody a smeru- hodnoty a vypoet zapisuji v preruseni
    IEC2CLR = _IEC2_IC7IE_MASK; //zakazu Interrupt od Capture
    IEC2CLR = _IEC2_T9IE_MASK; //zakazu Interrupt od Timeru9 (abych si to kdyztak hned nevyhodil pomoci zahod)
    tmpPeriodaTick = periodaTickPrenos; //prenesu si hodnoty
    tmpSmerOtaceni = smerOtaceniPrenos; //prenesu si hodnoty
    IEC2SET = _IEC2_IC7IE_MASK; //povolim Interrupt od Capture
    IEC2SET = _IEC2_T9IE_MASK; //polovim Interrupt od Timeru9

    if (TMR9Preteceni >= 2) { //pokud je hodnota delsi nez 64ms tak se motor netoci
        Ptr_CaptureRTM->perioda = 0; //motor se netoci
        Ptr_CaptureRTM->otacky = 0; //motor se netoci
        Ptr_CaptureRTM->smerOtaceni = 0; //motor se netoci
        prvniPreruseniIC7 = 1; //zase se mi bude pocitat perioda az pri dalsim preruseni
    } else {//jinak se motor toci a urcuji otacky - vztah n= 60f (pp = 1)
        Ptr_CaptureRTM->perioda = tmpPeriodaTick / 60; //tim dostavam hodnotu mezi pulzy v us
        if (tmpPeriodaTick != 0) { //osetrim pred delenim nulou
            tmpOtacky = OTACKY_PREPOC / tmpPeriodaTick; //pri prepoctu na sekundy tiky podelim 60 Mhz, cimz dostanu hodnotu v sekundach, hodnotu tiku vynasobim 500, coz je rozliseni inkrementalniho cidla (500 im/ot) - udelam prevracenou hodnotu, mam frekvenci a tu vynasobim 60
            Ptr_CaptureRTM->otacky = tmpOtacky; //predam si to do globalni promenne, kterou poslu do RTM
        }
        Ptr_CaptureRTM->smerOtaceni = tmpSmerOtaceni; //predavam si do globalni promenne, kterou poslu do RTM
    }

}

CAPTURE_RTM getVypocetOtacekASmeru(CAPTURE_RTM *Ptr_CaptureRTM) {
    return *Ptr_CaptureRTM; //vrati mi to strukturu -> tzn v RTM pak k tomu pristupuji teckovou notaci
}
/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */







/* *****************************************************************************
 End of File
 */
