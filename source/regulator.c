/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "./../header/messengerDEP.h"
#include "./../header/platformDEP32mk.h"
#include "./../header/pamet.h"
#include <stdbool.h>
#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
void initRegulator(REGULATOR *Ptr_reg) {
    Ptr_reg->K_I = 47; 
    Ptr_reg->K_P = 4800; 
    Ptr_reg->K_T = 0; 
    Ptr_reg->Zad_otacky=0; 
    Ptr_reg->hodnota_pro_PWM = 0;
    Ptr_reg->menic_nastaven=0;
    Ptr_reg->reg_rdy=0;
    Ptr_reg->reg_ochylka=0;
};


void runRegulatorOtacek(REGULATOR *Ptr_reg, CAPTURE_RTM *Ptr_CaptureRTM, int zatezovatel){ //zatezovatel odpovida pozadovane hodnote zatezovatele
    //doplnit cteni a prepoct z regulatoru
    static long long vypocet_otacek = 0;
    static float reg_odch = 0; //regulacni odchylka
    static float back_calc = 0; //suma iSum a proporcialni slozky 
    static float iSum = 0; //suma integracni slozky a dif_back_calc
    static float sat_back_calc = 0; //hodnota z backclac po saturaci
    static float dif_back_calc = 0; //rozdil saturovane a nesaturovane hodnoty
    
    //regulacni odchylka a vypocet back calculation
    if(Ptr_reg->reg_rdy == 1){
       

        vypocet_otacek = zatezovatel*3000; //vynasobim jmenovityma otackama motoru
        vypocet_otacek = vypocet_otacek/2047; //podelim zatezovatelem, cimz dostavam otacky
        Ptr_reg->Zad_otacky = vypocet_otacek;
        reg_odch = Ptr_reg->Zad_otacky - Ptr_CaptureRTM->otacky; //urcim si regulacni odchylku na zaklade rozdilu zadane a skutecne hodnoty
        Ptr_reg->reg_ochylka = reg_odch;
        back_calc = reg_odch*Ptr_reg->K_P;
        back_calc = back_calc+iSum; 
    
        //saturace back calculation a rozdil omezene a skutecne hodnoty
        sat_back_calc = back_calc;
        if(sat_back_calc >= 2047){ //saturuji
            sat_back_calc = 2047;
        }
        if(sat_back_calc <= -2047){ //saturuji
            sat_back_calc = -2047;
        }
        Ptr_reg->hodnota_pro_PWM = (int)sat_back_calc; //odesilam hodnotu pro PWM - mam nejakou stridu a motor se otaci na jednu nebo na druhou stranu
        dif_back_calc = sat_back_calc - back_calc; //urcim rozdil omezene a skutecne hodnoty
        
        //pocitani iSum
        iSum = iSum + Ptr_reg->K_I * reg_odch;
        iSum = iSum + Ptr_reg->K_T * dif_back_calc;
    }
    else{
        Ptr_reg->hodnota_pro_PWM = zatezovatel; //odesilam hodnotu pro PWM - zastavuji motor (mam stridu 50% tzn. casJednaPrenos = 3000)
    }
}





/* *****************************************************************************
 End of File
 */
