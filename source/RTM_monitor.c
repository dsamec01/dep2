/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "./../header/messengerDEP.h"
#include "./../header/platformDEP32mk.h"
#include "./../header/pamet.h"
#include "./../header/dekoder.h"
#include "./../header/protoypRTM.h"
#include <xc.h>
#include <sys/attribs.h>



/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
unsigned char odesli[MAX_DELKA]; //pole pro odesilani
unsigned char prijmi[MAX_DELKA]; //pole pro prijimani
signed short txCmdInteger = 0; //hodnota 0-7, do teto promenne ukladam z jakeho kanalu ctu


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
void initPrepinacRTM(bool *Ptr_prepinac){
*Ptr_prepinac =0;
}

void runKomunikaceRTM(ZATEZOVATEL *Ptr_zat, int zatezovatel, bool *Ptr_prepinac){
    static unsigned char citac50ms = 0; //citac musi byt static aby si pamatoval predchozi hodnotu
    //static bool komunikace = FALSE;
    static long ZatezovatelPrenos = 0; //sem budu ukladat data z RTM a z toho pak budu ukladat data do Struktury
    static unsigned char delkaZpravy = 0;
    static bool tmpPrepinacRTM =0; //pomocna promenna do ktere ukladam hodnotu z pointeru na globalni promennou
    
    if (citac50ms ==COUNT_MAX){
        citac50ms = 0;
        if(getMessageUSB(prijmi, COM_GO) == TRUE){ //testuji zda dostanu zpravu
            delkaZpravy = prijmi[0]; //ukladam nultou hodnotu pole, ktera mi urcuje delku zpravy - podle toho urcim zda mam int nebo ne
            txCmdInteger = bytesToInteger(&prijmi[1]); //zjistuje mi jakej prijimam kanal
             
            if((delkaZpravy == RTM_INT_DELKA_PRIJEM)&& (txCmdInteger == 0)){ //pokud je delka 7, tak pouzivam int, pokud je to pak vetsi tak je to float, tzn. ja chci pouzivat jen pro int - diky teto podmince musim komunikovat prostrednictvim toho kanalu ktery se rovna txCmdInteger (volim si z jakeho kanalu chci prijimat)
                 //komunikace= TRUE; //pomocna promenna kterou overuji zda je podminka splnena
                 
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
            //else{
            //    komunikace = FALSE;
            //}
             
        }
        //nyni odesilam na RTM nezavisle na tom jestli dostanu zpravu
        odesli[0] = RTM_DELKA_ODESLI; //odesle mi ze posilam dve hodnoty (nastavuji prvni hodnotu pole)
        tmpPrepinacRTM = *Ptr_prepinac;//ulozim si ukazatel do pomocne promenne, kterouu pak odeslu
        integerToBytes(tmpPrepinacRTM, &odesli[1]); //odesilam v jakem stavu mam prepinac RTM
        integerToBytes(zatezovatel, &odesli[3]); //odesilam jakou mam hodnotu zatezovatele, ktery mi vratil return ze struktury
        sendMessageUSB(odesli, COM_GO); //odesilam hodnotu po komunikaci    
    }
    else{
        citac50ms++;
    }    

}//konec funkce








/* *****************************************************************************
 End of File
 */
