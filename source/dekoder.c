/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "./../header/dekoder.h" //musim zpet abych se dostal do headeru
#include <stdbool.h>
#include "./../header/platformDEP32mk.h"


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
//inicializace struktury
  void initDekoder(DEKODER *Ptr_dekoder, int pocHodnota){
    Ptr_dekoder->stav=0; //jelikoz mam pointer musim predat pomoci sipek
    Ptr_dekoder->smer=pocHodnota; //smer nastavuji na pocHodnota (obecne je to nejaka pocatecni hodnota, ale ted je to 0)
   };

void runDekoderSmeru(DEKODER *Ptr_dekoder, bool kanalA, bool kanalB){
    //pri vbehu do funkce se mi zalozi pointer ktery bude odkazovat na danou strukturu, ta obsahuje info o stavu a smeru
   //zaroven do funkce budu posilat vyfilrovane hodnoty z kanaluA a kanaluB na zaklade kterych budu inkremetovat/dekrementoavt
    char stav_next = S0; //na pocatku budu ve stavu 0
   
    switch(Ptr_dekoder->stav){ //do switche predam informaci o stavu a na zaklade toho to pujde do daneho stavu
        case S0: {
            if(kanalA == 0 && kanalB == 0 ){ //prvni podminka vzdy pro kladny smer , vzdy resim hodnoty na kanalech a zda jeste muzu inkremetovat
                stav_next = S1;
                if(Ptr_dekoder->smer < DEKODER_MAX){ //podminku musim mit vevnitr abych mohl pote pripadne odcitat
                Ptr_dekoder->smer++;
                }
            }
            if(kanalA ==0 && kanalB == 1){ //druha podminka vzdy pro reverzni smer, vzdy resim hodnoty na kanalech a zda jeste muzu dekrementovat
                stav_next = S4;
                if(Ptr_dekoder->smer > DEKODER_MIN){
                Ptr_dekoder->smer--;
                }
            }
            if(kanalA ==1 && kanalB == 1){ //dalsi dve podminky jsou pro chybove stavy, viz nakres
                stav_next = S3;    
                }
            if(kanalA ==1 && kanalB ==0){
                stav_next = S2;
                }
            break; //vyjdu z case S0 a jdu na dalsi case S1
        }
            
        case S1: {
            if(kanalA ==1 && kanalB == 0 ){
                stav_next = S2;
                if(Ptr_dekoder->smer < DEKODER_MAX){
                Ptr_dekoder->smer++;
                }
            }
            if(kanalA ==0 && kanalB == 1){
                stav_next = S4;
                if(Ptr_dekoder->smer > DEKODER_MIN){
                Ptr_dekoder->smer--;
                }
            }
            if(kanalA ==1 && kanalB == 1){
                stav_next = S0;    
                }
            if(kanalA ==0 && kanalB == 0){
                stav_next = S1;    
                }
            break;
        }
            
        case S2: {
            if(kanalA ==1 && kanalB == 1){
                stav_next = S3;
                if(Ptr_dekoder->smer < DEKODER_MAX){
                Ptr_dekoder->smer++;
                }
            }
            if(kanalA ==0 && kanalB == 0){
                stav_next = S1;
                if(Ptr_dekoder->smer > DEKODER_MIN){
                Ptr_dekoder->smer--;
                }
            }
            if(kanalA ==0 && kanalB == 1){
                stav_next = S0;  
                }
            if(kanalA ==1 && kanalB == 0){
                stav_next = S2;  
                }
            break;
        }
        
		case S3: {
            if(kanalA ==0 && kanalB == 1){
                stav_next = S4;
                if(Ptr_dekoder->smer < DEKODER_MAX){
                Ptr_dekoder->smer++;
                }
            }
            if(kanalA ==1 && kanalB == 0){
                stav_next = S2;
                if(Ptr_dekoder->smer > DEKODER_MIN){
                Ptr_dekoder->smer--;
                }
            }
            if(kanalA ==0 && kanalB == 0){
                stav_next = S0;  
                }
            if(kanalA ==1 && kanalB == 1){
                stav_next = S3;  
                }
            break;
        }
			
        
        case S4: {
            if(kanalA ==0 && kanalB == 0){
                stav_next = S1;
                if(Ptr_dekoder->smer < DEKODER_MAX){
                Ptr_dekoder->smer++;
                }
            }
            if(kanalA ==1 && kanalB == 1){
                stav_next = S3;
                if(Ptr_dekoder->smer > DEKODER_MIN){
                Ptr_dekoder->smer--;
                }
            }
            if(kanalA ==1 && kanalB == 0){
                stav_next = S0;  
                }
            if(kanalA ==0 && kanalB == 1 ){
                stav_next = S4;  
                }
            break;
        }
       
    }//switch
    Ptr_dekoder->stav = stav_next; //po probehnuti switche predam do stavu stav S0 a jedu kolo znova
    //takto zde musim mit vsechny podminky, slo by udelat ze bych tam nemel pomocnou promennou stav_next, ale rovnou bych zapisoval do stavu a takto by mi to slo dokola (obdobne jako to mam u ledPWM)
    //tzn. nemusel bych predavat za strukturou stav s0, ale predaval bych tam ten predchozi stav, diky cemuz by mi to slo furt dokola a do s0 bych pak uz sel jen v pripade chyby

}//funkce 

//pokud hoddnota smeru je rovna 255 sviti limit maxima
void runSignalizaceMaxDekoderu(DEKODER *Ptr_dekoder){
    if(Ptr_dekoder->smer == DEKODER_MAX ){
        setCoderLedHL(1);
    }
    else{
        setCoderLedHL(0);
    }
}
//pokud hoddnota smeru je rovna 0 sviti limit minima
void runSignalizaceMinDekoderu(DEKODER *Ptr_dekoder){
    if(Ptr_dekoder->smer == DEKODER_MIN ){
        setCoderLedLL(1);
    }
    else{
        setCoderLedLL(0);
    }
}

//funkce pro predani - nedela nic jineho nez ze mi vraci hodnotu z funkce dekoderSmeru (hodnotu do funkce predam a tu stejnou hodnotu mi to vrati)
unsigned char getDekoderSmeru(DEKODER *Ptr_dekoder){ //vezmu adresu struktury z dekoderu
    return Ptr_dekoder->smer; //a vratim hodnotu struktury
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */



/* *****************************************************************************
 End of File
 */
