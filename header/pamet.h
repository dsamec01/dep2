/* ************************************************************************** */
//deklarace pro pamet.c

//included files
#include <stdbool.h>
#include "Caputre.h"
#define OMEZENI_ADC 1950 //kvuli šumu musim mit na potaku mensi hodnotu nez 2047 -> musim jakoby doraz posunout
#define PREPOCET_ADC 3000 //pomoci OMEZENI_ADC a PREPOCTU_ADC dostanu hodnoty z rozsahu -3000 az 3000 - to odpovida zadane hodnote otacek pri regulaci
#define PREPOCET_NA_FLOAT 1000.0 //pomoci teto promenne prepocitavam konstanty regulatoru na milisekundy ----kdyztak kdyby delalo zuby tak zvetsit na 1000000, pak bude v mikro, Kp je timto pouze prenasobena (nemusim diky tomu delat float mode)
#define OMEZENI_ZATEZOVATELE 2047 //pomoci teto hodnoty omezuji hodnotu zatezovatele pro prechodovou charakteristiku
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

//definice k Prechodove charakteristice
typedef struct{ //definuji si strukturu do ktere budu ukladat hodnotu zatezovatele z RTM a periodu vzorkovani
    int zetezovatelPrechChar; //zatezovatel pro prechodovou charakteristiku
    unsigned char periodaVzorkovani; //zadana perioda vzorkovani z RTM
    bool validDataPrechChar; //bude mi signalizovat zda mam nacteny data z RTM
    int dataChar[101]; //inicializace pole (musi mit 100 platnych vzorku)
    bool odesli; //bude mi signalizovat zda mam odesilat do RTM
    bool odeslano; //bude mi signalizovat ze mam odeslano do RTM
    bool runPrechChar; //signalizuji ze chci zatezovatel z RTM (Command3)
}PRECH_CHAR;

//definice k regulatoru
typedef struct{
    //hodnoty zadavane z RTM
    float K_P; //proporcialni konstanta
    float K_I; //integracni konstanta
    float K_T; //sledovací konstanta
    int Zad_otacky; //zadana hodnota otacek
    //hodnota pro PWM
    int hodnota_pro_PWM; //vysledna hodnota zatezovatele kterou poslu PWM
    int reg_ochylka;
    //flagy
    bool menic_nastaven;
    bool reg_rdy;
}REGULATOR;

//prototyp funkce
void initZat(ZATEZOVATEL *Ptr_zat, int pocHodnotaPO, int pocHodnotaRO, int pocHodnotaKO);
void initPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool pocHodnota);
void runPametTlacitka(DETEKCE_HRANY *Ptr_hrana, bool tlacitkozmacknuto);
bool getPametTlacitkaOutput(DETEKCE_HRANY *Ptr_hrana);
void signalizaceLED(DETEKCE_HRANY *Ptr_hrana, int prepoctenyDekoder, ZATEZOVATEL *Ptr_zat);
int getZatezovatel(ZATEZOVATEL *Ptr_zat, DETEKCE_HRANY *Ptr_hrana, bool *Ptr_prepinac, PRECH_CHAR *Ptr_PrechCharData);
void runIRCneboPWM(DETEKCE_HRANY *Ptr_hrana);

//prototypy k prechodove charakteristice
void initPrechChar(PRECH_CHAR *Ptr_PrechCharData);
void runPrechodChar(PRECH_CHAR *Ptr_PrechCharData, CAPTURE_RTM *Ptr_CaptureRTM);

//prototypy k regulatorum
void initRegulator(REGULATOR *Ptr_reg);
void runRegulatorOtacek(REGULATOR *Ptr_reg, CAPTURE_RTM *Ptr_CaptureRTM, int zatezovatel);

    

  



/* *****************************************************************************
 End of File
 */
