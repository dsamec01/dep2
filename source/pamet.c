/* ************************************************************************** */

/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdbool.h>
#include "./../header/pamet.h"
#include "./../header/dekoder.h"
#include "./../header/platformDEP32mk.h"


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
void initPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool pocHodnota) {
    Ptr_hrana->stav = 0; //jelikoz mam pointer musim predat pomoci sipek
    Ptr_hrana->hrana = pocHodnota; //to zda si pamatuji ze je tlacitko zmacknuto nebo ne nastavuji na pocHodnota (obecne je to nejaka pocatecni hodnota, ale ted je to 0)
};

void initZat(ZATEZOVATEL *Ptr_zat, int pocHodnotaPO, int pocHodnotaRO, int pocHodnotaKO) {
    Ptr_zat->zatPO = pocHodnotaPO; //jelikoz mam pointer musim predat pomoci sipek
    Ptr_zat->zatRO = pocHodnotaRO; //zatezovatel na zacatku nastavuji na pocHodnota (obecne je to nejaka pocatecni hodnota, ale ted je to 0)
    Ptr_zat->zatKO = pocHodnotaKO;
};

void runPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool tlacitkozmacknuto) //vstupem je jednak adresa struktury kde drzim stav a informaci o tom zda mi prisla hrana nebo ne a druhak je to informace o tom zda mam tlacitko zmackle nebo nikoliv
{
switch(Ptr_hrana->stav)
	{
	case S0:
		{
			if(tlacitkozmacknuto ==FALSE) //pokud se tlacitko nezmacklo tak stav next bude S0
				{
				Ptr_hrana->stav = S0;		

				}
			else
				{
				Ptr_hrana->stav = S1;
				Ptr_hrana ->hrana = TRUE; //pokud zmacknu tlacitko detekovala se hrana (tlacitko umozni prepnuti prepinace)
				}
			break;
		}
	case S1:
		{
			if(tlacitkozmacknuto ==TRUE)
				{
				Ptr_hrana->stav = S1;		
				Ptr_hrana ->hrana = TRUE; //pokud mam tlacitko zmackly furt tam mam tu predchozi hranu (prepinac je preplej, furt tam tam tu predchozi detekovanou nabeznou hranu)
				}
			else
				{
				Ptr_hrana->stav = S2; //pokud tlacitko pustim jdu do stavu S2 (prepinac je stale preplej, furt tam tam tu predchozi detekovanou nabeznou hranu)
				
				}
			break;
		}
	case S2:
		{
			if(tlacitkozmacknuto ==FALSE)
				{
				Ptr_hrana->stav = S2;		//pokud je tlacitko nezmacky nic se nedeje (prepinac je stale preplej, mam tam stale tu predchozi detekovanou nabeznou hranu)
				
				}
			else
				{
				Ptr_hrana->stav = S3; //pokud tlacitko zmacknu znovu jdu do s3 a hrana jde dolu (prepinac se prepne zpet)
				Ptr_hrana ->hrana = FALSE;
				}
			break;
		}
	case S3:
		{
			if(tlacitkozmacknuto ==TRUE)
				{
				Ptr_hrana->stav = S3;		
				Ptr_hrana ->hrana = FALSE; //pokud tlacitko drzim, tak hrana je furt dole (prepinac je v puvodnim stavu, mam stale predchozi detekovanou sestupnou hranu)
				}
			else
				{
				Ptr_hrana->stav = S0; //pokud tlacitko pustim jdu na zacatek (prepinac je v puvodnim stavu, mam stale predchozi detekovanou sestupnou hranu)
				
				}
			break;
		}
	}
Ptr_hrana->stav; //predavam predchozi vystup automatu na vstup
}

bool getPametTlacitkaOutput(DETEKCE_HRANY *Ptr_hrana){
 return Ptr_hrana ->hrana;
}

//u potaku misto OMEZENI dam OMEZENI_ADC
void signalizaceLED(DETEKCE_HRANY *Ptr_hrana, int prepoctenyDekoder, ZATEZOVATEL *Ptr_zat){ //do signalizace poslu strukturu zatezovatele, vystup z omezovace a hodnotu na zaklade ktere urcuji stav prepinace
    if (Ptr_hrana ->hrana == 0){ //na zaklade stavu prepinace, pokud je v 0 tak ctu z dekoderu(kdy vystupOmezovace je prepoctena hodnota z dekoderu)
    setFpgaVxValue(prepoctenyDekoder);
    Ptr_zat->zatPO=prepoctenyDekoder; //prepoctenou hodnotu zatezovatele si ulozim do struktury
    }
    if (Ptr_hrana ->hrana == 1){//na zaklade stavu prepinace, pokud je v 1 tak ctu z potenciometru
        int potenciometrValue = getPotentiometerValue(); //ulozim si hodnotu z potenciometru do pomocne promenne
        if (potenciometrValue  > OMEZENI) { //pokud mam o neco vetsi nez je 2047 tak to oriznu
        potenciometrValue  = OMEZENI; 
        } 
        if (potenciometrValue < -OMEZENI) { //pokud mam o neco mensi nez 2047, tak to oriznu
        potenciometrValue  = -OMEZENI;
        }
        //potenciometrValue = potenciometrValue*(2047/OMEZENI);
        setFpgaVxValue(potenciometrValue); //vyslednou hodnotu si rozsvitim LED
        Ptr_zat->zatRO=potenciometrValue; //a zaroven si ji ulozim do struktury
    }  
}

int getZatezovatel(ZATEZOVATEL *Ptr_zat, DETEKCE_HRANY *Ptr_hrana){ //dostanu hodnotu zatezovatele
    if (Ptr_hrana ->hrana == 0){ //na zaklade toho v jakem stavu mam prepinac, tak podle toho mi to vraci hodnotu zatezovatele pro PWM
        return Ptr_zat->zatPO;
    }
    
    if (Ptr_hrana ->hrana == 1){
         return Ptr_zat->zatRO;
    }

}//jeste doplnit rozhodovani na zaklade hodnoty z modulu


/* *****************************************************************************
 End of File
 */
