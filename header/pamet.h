/* ************************************************************************** */
//deklarace pro pamet.c

//included files
#include <stdbool.h>


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
    int zatPO;
    int zatRO;
    int zatKO;
}ZATEZOVATEL;


//prototyp funkce
void initZat(ZATEZOVATEL *Ptr_zat, int pocHodnotaPO, int pocHodnotaRO, int pocHodnotaKO);
void initPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool pocHodnota);
void runPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool tlacitkozmacknuto);
bool getPametTlacitkaOutput(DETEKCE_HRANY *Ptr_hrana);
void signalizaceLED(DETEKCE_HRANY *Ptr_hrana, int vystupOmezovace, ZATEZOVATEL *Ptr_zat);
    

  



/* *****************************************************************************
 End of File
 */
