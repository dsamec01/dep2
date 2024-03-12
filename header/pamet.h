/* ************************************************************************** */
//deklarace pro pamet.c

//included files
#include <stdbool.h>
#define OMEZENI_ADC 1950 //kvuli šumu musim mit na potaku mensi hodnotu nez 2047 -> musim jakoby doraz posunout
#define PREPOCET_ADC 2047 //pomoci OMEZENI_ADC a PREPOCTU_ADC dostanu hodnoty z rozsahu -2047 az 2047

/* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

// *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************
typedef struct{
unsigned char stav; //aktualni hodnota stavu
bool hrana; //indikuje mi to zda prisla hrana a tlacitko je tak v 1 nebo v 0 (tuto hodnotu si pak pamatuji a podle toho mam tlacitko v 1 nebo v 0)
}DETEKCE_HRANY;

typedef struct{ //definuji si strukturu do ktere budu ukladat zatezovatele z dekoderu, monitoru a potaku
    int zatPO; //zatezovatel od potaku
    int zatRO; //zatezovatel od dekoderu
    int zatKO; //zatezovatel od monitoru
}ZATEZOVATEL;


//prototyp funkce
void initZat(ZATEZOVATEL *Ptr_zat, int pocHodnotaPO, int pocHodnotaRO, int pocHodnotaKO);
void initPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool pocHodnota);
void runPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool tlacitkozmacknuto);
bool getPametTlacitkaOutput(DETEKCE_HRANY *Ptr_hrana);
void signalizaceLED(DETEKCE_HRANY *Ptr_hrana, int prepoctenyDekoder, ZATEZOVATEL *Ptr_zat);
int getZatezovatel(ZATEZOVATEL *Ptr_zat, DETEKCE_HRANY *Ptr_hrana);
    

  



/* *****************************************************************************
 End of File
 */
