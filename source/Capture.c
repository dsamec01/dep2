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

unsigned int PeriodaPrepocet = 0; //chci aby byla unisgned aby pretekla spolu s timerem
bool stopaA = 0; //do teto hodnoty si nacitam zda prisla hrana na stopeA
int dlouhaPerioda = 0; //pomoci teto hodnoty urciuji zda se mi motorek otaci nebo nikoliv
bool stopaB = 0; //do teto hodnoty ukladam hodnotu z kanaluB
bool PreruseniIC7 = 0;//tato promenna mi bude signalizovat zda nastalo preruseni
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
void initVypocetOtacekASmeru(CAPTURE_RTM *Ptr_CaptureRTM){ //inicializace struktury
    Ptr_CaptureRTM->otacky=0; //jelikoz mam pointer musim predat pomoci sipek, kdy stavu prirazuji pocatecni hodnotu
    Ptr_CaptureRTM->perioda=0;
    Ptr_CaptureRTM->smerOtaceni=0;
    };


//inicializace Capture
void initCapture(void){
    //nastaveni Timer pro mereni
    //nastavim T6CON - jelikoz beru 32 bitu (spojuji Timery) tak nastavuji Timer6, ale preruseni atd. mi vyvolava Timer7, tedy IF, IE, IP nastavuji pro Timer7 
    T6CONbits.ON = 0; //behem nastaveni nuluji
    T6CONbits.SIDL = 0; //nepouzivam
    T6CONbits. TGATE = 0;//chci 0 protoze chci posilat hodnotu procesoru
    T6CONbits.TCKPS=0;//nepozivam preddelicku
    T6CONbits.T32=1; //beru 32 bitu (spojuji Timer6 a Timer7) 
    T6CONbits.TCS=0; //chci posilat frekvenci procesoru          
    TMR6=0; //vynuluji casovy registr
    PR6=0xFFFFFFFF; //nactu PR6 a PR7 32 bitovou hodnotou (2 na 32)
    //inicializace Interrupt - hodnoty k preruseni najdu v tabulce 8-3 - od tohoto timeru zakazuji preruseni (pouzivam ho pouze jako referencni)
    IFS2bits.T7IF = 0; //Interrupt flag nastavuji na 0, nemam na zacatku flag
    IEC2bits.T7IE = 0; //Zakazuji preruseni - nebudu ho vubec pouzivat

    //nastavuji Timer pro chybovy hlaseni
    //nastavim T9CON - staci mi 16 bitu -> pokud ma 1:1 pretece za 1,1ms jelikoz ale chci max periodu 20ms tak potrebuji preddelicku 1:32  
    T9CON = 0x0; //Zastavim timer a vynuluji vsechny registry - nepouzivam preddelicku
    T9CONbits.TCKPS=0b101; //ve vynulovanem registru nastavim pouze preddelicku na 1:32 -> diky tomu mi to pretece kazdych 32x1,1 ms
    TMR9=0; //vynuluji casovy registr
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
    CFGCONbits.ICACLK=1;  //navic??
    //nastaveim preruseni od Capture Unit
    IFS2bits.IC7IF =0; //shazuji Interrup flag
    IPC20bits.IC7IP = 4; //nastavuji prioritu na 4
    IPC20bits.IC7IS = 2; //nastavuji subprioritu na 2
    IEC2bits.IC7IE = 1; //povoluji preruseni
    //nastavim pro jistotu i priority pro chybovy stav (IC7IPE, IC7ISE atd.) ackoliv mi to bude zarizovat Timer9
    IFS2bits.IC7EIF = 0;   //shazuji Interrup flag
    IPC20bits.IC7EIP = 6; //nastavuji prioritu na 6
    IPC20bits.IC7EIS = 1; //nastavuji subprioritu na 1
    IEC2bits.IC7EIE= 1; //povoluji preruseni
    
    //nakonec spoustim timery a Capture
    T6CONbits.ON = 1; 
    T9CONbits.ON = 1;
    IC7CONbits.ON = 1;
}

void __ISR(_INPUT_CAPTURE_7_VECTOR, IPL4SOFT) IC7_IntHandler(void){ //po detekci jedne hrany mi to vyvola preruseni
    //v preruseni si nactu hodnotu z bufferu a jdu dal
    PeriodaPrepocet = IC7BUF; //nacitam hodnotu z bufferu
    stopaB = getIrcChannelBValue(); //do stopy A si nactu hodnotu ze stopyB
    stopaA = 1; //pokud nastane preruseni. ulozim si do stopyA jednicku (vim ze mam hranu) - po urceni smeru to shodim do 0
    PreruseniIC7 = 1; //rekne mi to ze nastalo preruseni
    IFS2CLR = _IFS2_IC7IF_MASK; //shazuji flag capture jednotky atomicky
} 

void __ISR(_INPUT_CAPTURE_7_ERROR_VECTOR, IPL6SOFT) IC7IntErrorHandler (void){//po nacitani mi to vynuluje buffer
    unsigned int zahod;
    while(IC7CONbits.ICBNE!= 0){
        zahod=IC7BUF;
    }
        IFS2CLR = _IFS2_IC7EIF_MASK;   // Interrupt flag status bit  
}

void __ISR(_TIMER_9_VECTOR, IPL4SOFT) T9_IntHandler(void){ //casovac nacita jednu periodu, vyvola preruseni - pri preruseni kontroluji zda mam zaroven plny buffer
    unsigned int zahod = 0; //pomoci teto promenne budu vyhozvat hodnoty z bufferu v pripade poruchy
    dlouhaPerioda++;//pricita se kazde preruseni (po 32ms)
    if (IC7CONbits.ICOV ==1){ //pokud dojde k vyvolani preruseni od timeru 9 (vim ze mam 32ms) a zaroven mi to pretece, mam chybovy stav a registr musim vynulovat
        while(IC7CONbits.ICBNE != 0){ //dokud registr nebude vyvolany tak nacitej do zahod, cimz zahodis tuto hodnotu
        zahod = IC7BUF;
        }            
    }
    IFS2CLR = _IFS2_T9IF_MASK; //shazuji flag atomicky
} 

void runVypocetOtacekASmeru(CAPTURE_RTM *Ptr_CaptureRTM){ 
    //nastaveni lokalnich promennych
    static unsigned int casOld = 0; //musi byt unsigned aby promenna pretekala soucasne s timerem
    static unsigned int casNew = 0; //msui byt unsigned aby promenna pretekala soucasne s timerem (mam ted oboje 32 bit)
    unsigned int periodaTick = 0; //uz chci znamenkovou - sem budu ukladat hodnoty z tiku
    unsigned long long dlouhaperiodaTick = 0; //po pripad ze bych mel casOld pred pretecenim skoro stejny jako casNew po preteceni tak si to ulozim do 64 bitove promenne protoze se to do 32bit nevejde
    static long long tmpOtacky = 0; //do teto hodnoty budu ukladat hodnotu otacek
    
    //urceni periody 
    IEC2CLR = _IEC2_IC7IE_MASK; //zakazu Interrupt od Capture
    IEC2CLR = _IEC2_T9IE_MASK; //zakazu Interrupt od Timeru9 (abych si to kdyztak hned nevyhodil pomoci zahod)
    casNew = PeriodaPrepocet; //prenesu globalni promennou do lokalni promenne
    IEC2SET = _IEC2_IC7IE_MASK; //povolim Interrupt od Capture
    IEC2SET = _IEC2_T9IE_MASK; //polovim Interrupt od Timeru9
    if (PreruseniIC7 == 1){
        if (casNew > casOld){//kdyz mi to nepretece plati vzdy    
            periodaTick = casNew - casOld;//dostanu hodnotu v tikach casovace
            Ptr_CaptureRTM->perioda = periodaTick/60; //tim dostavam hodnotu mezi pulzy v us
        }
        if (casOld>casNew){//po preteceni se mi stane ze mam casOld pred preteceni a casNew je po preteceni tedy je tato podminka splnena
            periodaTick = ULONG_MAX - casOld; //urcim si rozdil maximalni hodnoty timeru a stareho casu pred pretecenim
            dlouhaperiodaTick = periodaTick + casNew; //prictu k tomu novy cas a mam periodu - zaroven diky long longu mam osetren pripad kdy asOld pred pretecenim skoro stejny jako casNew po preteceni a neveslo by se mi to do 32 bitu
            Ptr_CaptureRTM->perioda = dlouhaperiodaTick/60; //tim dostavam hodnotu mezi pulzy v us
        }
        casOld = casNew; //ulozim do prechozi hodnoty tu aktualni
        PreruseniIC7 =0;//shodim Flag ze mam preruseni od IC7
    }
    //vypocet otacek (ot/min)
    if(dlouhaPerioda >=2){ //pokud je hodnota delsi nez 64ms tak se motor netoci
        Ptr_CaptureRTM->otacky = 0; //motor se netoci
        Ptr_CaptureRTM->smerOtaceni=0; //motor se netoci
    }
    //jinak se motor toci a urcuji otacky - vztah n= 60f (pp = 1)
    if(periodaTick != 0){ //pri prepoctu na sekundy tiky podelim 60 Mhz, cimz dostanu hodnotu v sekundach, hodnotu tiku vynasobim 500, coz je rozliseni inkrementalniho cidla (500 im/ot) - udelam prevracenou hodnotu, mam frekvenci a tu vynasobim 60
        tmpOtacky = OTACKY_PREPOC/periodaTick;  
        Ptr_CaptureRTM->otacky = tmpOtacky; //predam si to do globalni promenne, kterou poslu do RTM
    }
        
    //urceni smeru otaceni - hodnota 1 = kladny/-1 = zaporny
    if (stopaA == 1 && stopaB == 1){ //pokud mam hranu na A a zaroven mam B v 1 tak se mi to otaci po smeru (clockwise)
        Ptr_CaptureRTM->smerOtaceni=1; //otacim se po smeru (mam 1)
        dlouhaPerioda =0; //uz se mi motor otaci
        stopaA = 0; //zaroven stopuA vynuluji a cekam na dalsi udalost
    }
    if (stopaA == 1 && stopaB == 0){ //pokud mam hranu na A a zaroven mam B v 0 tak se mi to otaci proti smeru (counterclockwise)
        Ptr_CaptureRTM->smerOtaceni=-1; //otacim se po smeru (mam -1)
        dlouhaPerioda =0; //uz se mi motor otaci
        stopaA = 0; //zaroven stopuA vynuluji a cekam na dalsi udalost
    }
    
    
}


CAPTURE_RTM getVypocetOtacekASmeru(CAPTURE_RTM *Ptr_CaptureRTM){
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
