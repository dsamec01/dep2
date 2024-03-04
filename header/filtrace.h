/* ************************************************************************** */
//included files
#include <stdbool.h>

// struktura pro filtraci tlacitka - vytvorim typ, ktery vychazi ze struktury a ta ma dva cleny
    typedef struct {
        /* Describe structure member. */
        unsigned char stav; //hodnota 0 az 255 (vlastne vnitrni hodnota)
        bool tlacitkozmacknuto; //zadefinuji hodnoty TRUE, FALSE a pak to tomuto budu prirazovat (vystupni hodnota). To zda je talcitko zmacknuto nabyva jen hodnot 0 a 1
                   } FILTR_TLACITKA; // potrebuji znat v jakym stavu jsem a jestli mam 0 nebo 1 na vstupu
   
                   
    //zadefinuji vyctovy typ enum
    enum {s0,s1,s2,s3,s4,s5,s6}; //stav do ktereho se mohu dostat
                   
    //prototyp funkce      
    void initFiltr(FILTR_TLACITKA *Ptr_tlacitko, bool pocStav);
    void runFiltr(FILTR_TLACITKA *Ptr_tlacitko, bool vstuptlacitka); 
    bool getFiltrOutput(FILTR_TLACITKA *Ptr_tlacitko); //vraci to hodnotu 0/1
    

                   


