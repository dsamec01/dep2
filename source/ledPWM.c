/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdbool.h>
#include "./../header/platformDEP32mk.h"
#include "../header/ledPWM.h"
#include "../header/dekoder.h"
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
//inicializace struktury, kdybych nemel ve funkci, tak to musim napsat do mainu
  void initPWM(LED_PWM *Ptr_LED){
    Ptr_LED->stav=0; //jelikoz mam pointer musim predat pomoci sipek
    Ptr_LED->timer=0;
    };

//rozsveceni led s promennou periodou
void runLedPWM(LED_PWM *Ptr_LED,unsigned char vystup_dekoderu){ //do vstupu poslu sktrukturu (chci mit informaci o stavu, timeru) a zaroven tam poslu vystup z dekoderu, kterym urcuji dobu periody
    float perioda; //pomocna promenna, musi byt float 
    float doba_sviceni; //pomocna promenna, musi byt float 
    perioda = MIN_PERIODA +((MAX_PERIODA_PREPOCET/DEKODER_MAX)*vystup_dekoderu); //preskaluji na rozsah 200 - 2000 ms tzn. k minimalni hodnote periody pripoctu dobu v zavislosti na natoceni dekoderu (max pricitam 1800 aby dalo 2000)
    doba_sviceni = perioda/2;
        switch(Ptr_LED->stav){
            case 0: {
                if(Ptr_LED->timer < perioda){ 
                    //dochazi k volani funkce kazdou 1ms, takze pokud nastane is1ms a hodnota timeru bude mensi nez ma byt perioda tak dojde k pricteni a jdu do stavu 1 
                    Ptr_LED->timer++;
                    Ptr_LED->stav = 1;
                }
                else{ //pokud nastane is1ms a casovac bude vetsi nebo roven dobe periody, tak k pricteni nedojde a dojde k jeho vynulovai a jedu od znova (jdu do stavu 0)
                    Ptr_LED->timer=0;
                    Ptr_LED->stav =0;
                }
                break;
            }
            
            case 1: {//se se dostanu pokud je hodnota timeru mensi nez je doba periody
                if (Ptr_LED->timer < doba_sviceni){ 
                    //pokud mam timer mensi nez je pozadovana doba sviceni, tak mi led sviti. (mam pulperiodu kdy mam svitit) 
                    setLedV5(1);
                    Ptr_LED->stav =0;
                    }
                else{ //pokud mam timer vetsi nebo roven pozadovane dobe sviceni, tak mi led nesviti (mam pulperiodu kdy ma byt vypla)
                    setLedV5(0);
                    Ptr_LED->stav =0;
                }
                break;
    
            }
    
        }//switch

Ptr_LED->stav;


}//funkce


//funkce pro predani - nedela nic jineho nez ze mi vraci hodnotu z funkce ledPWM (hodnotu do funkce predam a tu stejnou hodnotu mi to vrati)
unsigned char getledPWM(LED_PWM *Ptr_LED){
    return Ptr_LED->timer;
}


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */



/* *****************************************************************************
 End of File
 */
