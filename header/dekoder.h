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
    #define DEKODER_MAX -127 // konstanta 255 se kterou budu porovnavat jestli jsem se dopocital minima 
    #define DEKODER_MIN 127 // kosntanta 0 se kterou budu porovnavat jestli jsem se dopocital maxima
    #define OMEZENI 2047

   
    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************
    typedef struct {
        /* Describe structure member. */
        unsigned char stav;
        char smer; //hodnota -127 az 127
                   } DEKODER;
        
    enum{S0,S1,S2,S3,S4};
    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************
    //musim vtahnout prototypy funkci, abych je pomoci toho vtahl do mainu (applicationControl.c)
    void initDekoder(DEKODER *Ptr_dekoder, char pocHodnota);
    void runDekoderSmeru(DEKODER *Ptr_dekoder, bool kanalA, bool kanalB);
    void runSignalizaceMaxDekoderu(DEKODER *Ptr_dekoder);
    void runSignalizaceMinDekoderu(DEKODER *Ptr_dekoder);
    char getDekoderSmeru(DEKODER *Ptr_dekoder); //vraci to hodnotu -127 az 127
    int getOmezovacDekoderu(DEKODER *Ptr_dekoder); //vraci hodnotu -2047 az 2047

/* *****************************************************************************
 End of File
 */
