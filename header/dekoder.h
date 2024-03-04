/* ************************************************************************** */

//#ifndef dekoder    /* Guard against multiple inclusion */
//#define dekoder


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdbool.h>

    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */
    #define DEKODER_MAX 255 // konstanta 255 se kterou budu porovnavat jestli jsem se dopocital minima 
    #define DEKODER_MIN 0 // kosntanta 0 se kterou budu porovnavat jestli jsem se dopocital maxima

   
    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************
    typedef struct {
        /* Describe structure member. */
        unsigned char stav; //hodnota 0 az 255 (vlastne vnitrni hodnota)
        unsigned char smer; 
                   } DEKODER;
        
    enum{S0,S1,S2,S3,S4};
    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************
    //musim vtahnout prototypy funkci, abych je pomoci toho vtahl do mainu (applicationControl.c)
    void initDekoder(DEKODER *Ptr_dekoder, int pocHodnota);
    void runDekoderSmeru(DEKODER *Ptr_dekoder, bool kanalA, bool kanalB);
    void runSignalizaceMaxDekoderu(DEKODER *Ptr_dekoder);
    void runSignalizaceMinDekoderu(DEKODER *Ptr_dekoder);
    unsigned char getDekoderSmeru(DEKODER *Ptr_dekoder); //vraci to hodnotu 0-255

/* *****************************************************************************
 End of File
 */
