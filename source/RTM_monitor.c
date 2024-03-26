/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "./../header/messengerDEP.h"
#include "./../header/platformDEP32mk.h"
#include "./../header/pamet.h"
#include "./../header/dekoder.h"
#include "./../header/Caputre.h"
#include "./../header/protoypRTM.h"
#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>



/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
unsigned char odesli[MAX_DELKA]; //pole pro odesilani
unsigned char prijmi[MAX_DELKA]; //pole pro prijimani
unsigned short Command = 0; //hodnota 0-7, do teto promenne ukladam z jakeho kanalu ctu


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
void initPrepinacRTM(bool *Ptr_prepinac){
*Ptr_prepinac =0;
}

void runKomunikaceRTM(ZATEZOVATEL *Ptr_zat, int zatezovatel, bool *Ptr_prepinac, CAPTURE_RTM *Ptr_CaptureRTM){
    //staticke promenne aby mi drzeli hodnotu pri dalsim volani fce - aby mi drzeli hodnotu celych 50ms
    static unsigned char citac50ms = 0; //citac musi byt static aby si pamatoval predchozi hodnotu
    static unsigned char citacCyklu = 0; //do teto promenne budu ukladat hodnoty 0 az 1 a podle toho budu odesilat do tableTerminalu/grafickeho Terminalu
    static long ZatezovatelPrenos = 0; //sem budu ukladat data z RTM a z toho pak budu ukladat data do struktury
    static unsigned char delkaZpravy = 0; //seb budu ukladat delku zpravy
    static bool tmpPrepinacRTM =0; //pomocna promenna do ktere ukladam hodnotu z pointeru na globalni promennou
    static bool komunikace = 0; //timto ovladam jestli zpravu prijimam nebo ne
    static int otacky = 0; //do teto promenne budu ukladat hodnotu otacek ze struktury
    static int perioda = 0; //do teto promenne budu kladat hodnotu periody ze struktury
    static char smerOtaceni = 0; //do teto promenne budu ukladat informaci o smeru ze struktury
    
    if (citac50ms ==COUNT_MAX){
        citac50ms = 0;
        if(getMessageUSB(prijmi, COM_GO) == TRUE){ //testuji zda dostanu zpravu - pokud ano jdu dal - vykonava se jen kdyz dostanu zpravu
            delkaZpravy = prijmi[0]; //ukladam nultou hodnotu pole, ktera mi urcuje delku zpravy - podle toho urcim zda mam int nebo ne
            Command = bytesToInteger(&prijmi[1]); //zjistuje mi jakej prijimam kanal
            if((delkaZpravy == RTM_INT_DELKA_PRIJEM)&& (Command == 0)){
                komunikace = 0; //pokud prijde hodnota z kanalu 0 zastavuji komunikaci (nevysilam)
            } 
            if((delkaZpravy == RTM_INT_DELKA_PRIJEM)&& (Command == 1)){
                komunikace = 1; //pokud mi prijde zprava z kanalu 1, tak spoustim komunikaci
            } 
            if((delkaZpravy == RTM_INT_DELKA_PRIJEM)&& (Command == 2)){ //pokud je delka 7, tak pouzivam int, pokud je to pak vetsi tak je to float, tzn. ja chci pouzivat jen pro int - diky teto podmince musim komunikovat prostrednictvim toho kanalu ktery se rovna Command (volim si z jakeho kanalu chci prijimat)
                 
                 //ctu parametry z RTM
                 *Ptr_prepinac = bytesToInteger(&prijmi[3]); //odpovida prvnimu parametru, kdy na zaklade teto hodnoty prepinam jestli chci zatezovatel z RTM nebo ne (ukladam prvni parametr pole)
                 ZatezovatelPrenos = bytesToInteger(&prijmi[5]); //pomoci teto hodnoty urcuji velikost zatezovatele (ukladam 2. parametr z pole)
                 
                 //nyni ifama omezuji hodnotu aby byla v rozsahu -2047 az 2047
                 if (ZatezovatelPrenos >= OMEZENI){
                    ZatezovatelPrenos = OMEZENI;
                 }
                 if (ZatezovatelPrenos <= -OMEZENI){
                    ZatezovatelPrenos = -OMEZENI;
                 }
                 else{
                    ZatezovatelPrenos;
                 }
                 Ptr_zat->zatKO = ZatezovatelPrenos; //ulozim do struktury hodnotu zatezovatele z monitoru 
             }
        }
        
            
            if(komunikace == 1) { //pokud mam v komunikaci 1 tak odesilam jinak se nic nevykonava
                //nyni odesilam na RTM nezavisle na tom jestli dostanu zpravu
                switch(citacCyklu){
                    case 0: {//odesilani do grafiky
                    odesli[0] = RTM_DELKA_ODESLI; //odesle mi ze posilam dve hodnoty (nastavuji prvni hodnotu pole)
                    tmpPrepinacRTM = *Ptr_prepinac;//ulozim si ukazatel do pomocne promenne, kterouu pak odeslu
                    integerToBytes(tmpPrepinacRTM, &odesli[1]); //odesilam v jakem stavu mam prepinac RTM
                    integerToBytes(zatezovatel, &odesli[3]); //odesilam jakou mam hodnotu zatezovatele, ktery mi vratil return ze struktury
                    otacky = Ptr_CaptureRTM -> otacky;
                    smerOtaceni = Ptr_CaptureRTM -> smerOtaceni;
                    otacky = otacky*smerOtaceni; //az tohle povolim tak vymazat smerOtaceni z case2
                    integerToBytes(otacky, &odesli[5]);
                    sendMessageUSB(odesli, COM_GO); //odesilam hodnotu po komunikaci 
                    citacCyklu = 1;
                    break;
                    }
                    
                    case 1: {//odesilani do TableTerminalu - musim po jedne hodnote
                    perioda = Ptr_CaptureRTM -> perioda;
                    char per[40]; //zakladam pole charu pro periodu
                    sprintf(per, "perioda = %4d [us]", perioda); //prevadi mi to na string, ktery budu vysilat to TableTerminalu
                    sendTableTerminalMessageUSB("1A", per);
                    citacCyklu = 2;
                    break;
                    }
                    
                    case 2: {//odesilani do TableTerminalu
                    //smerOtaceni = Ptr_CaptureRTM -> smerOtaceni;
                    char sm[40];//zakladam pole charu pro smer
                    sprintf(sm, "smer otaceni je %4d", smerOtaceni);//prevadi mi to na string, ktery budu vysilat to TableTerminalu
                    sendTableTerminalMessageUSB("2A", sm);
                    citacCyklu = 3;
                    break;
                    }
                    
                    case 3: {//odesilani do TableTerminalu
                    char ot[40];//zakladam pole charu pro smer
                    sprintf(ot, "otacky = %4d [ot/min]", otacky);//prevadi mi to na string, ktery budu vysilat to TableTerminalu
                    sendTableTerminalMessageUSB("3A", ot);
                    citacCyklu = 0;
                    break;
                    }
                }           
                
            }
        
            
    }//konec ifu kde kontroluji stav citace
    
    else{
        citac50ms++;
    }    

}//konec funkce








/* *****************************************************************************
 End of File
 */
