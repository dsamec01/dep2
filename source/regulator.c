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
    Ptr_reg->K_I = 0; 
    Ptr_reg->K_P = 0; 
    Ptr_reg->K_T = 0; 
    Ptr_reg->Zad_otacky=0; 
    Ptr_reg->hodnota_pro_PWM = 0;
    Ptr_reg->menic_nastaven=0;
    Ptr_reg->Otacky_zadany=0;
    Ptr_reg->reg_rdy=0;
    Ptr_reg->reg_ochylka=0;
};


void runRegulatorOtacek(REGULATOR *Ptr_reg, CAPTURE_RTM *Ptr_CaptureRTM){ //zatezovatel odpovida pozadovane hodnote zatezovatele
    //doplnit cteni a prepoct z regulatoru
    static int reg_odch = 0; //regulacni odchylka
    static int back_calc = 0; //suma iSum a proporcialni slozky 
    static int iSum = 0; //suma integracni slozky a dif_back_calc
    static int sat_back_calc = 0; //hodnota z backclac po saturaci
    static int dif_back_calc = 0; //rozdil saturovane a nesaturovane hodnoty
    
    //regulacni odchylka a vypocet back calculation
    if(Ptr_reg->reg_rdy == 1){
        reg_odch = Ptr_reg->Zad_otacky - Ptr_CaptureRTM->otacky; //urcim si regulacni odchylku na zaklade rozdilu zadane a skutecne hodnoty
        Ptr_reg->reg_ochylka = reg_odch;
        back_calc = reg_odch*Ptr_reg->K_P;
        back_calc = back_calc+iSum; 
        back_calc = back_calc/PREPOCET_NA_MS; //jelikoz mam konstanty z RTM ve forme unsigned int tak to prevedu na ms
    
        //saturace back calculation a rozdil omezene a skutecne hodnoty
        sat_back_calc = back_calc;
        if(sat_back_calc >= 2047){ //saturuji
            sat_back_calc = 2047;
        }
        if(sat_back_calc <= -2047){ //saturuji
            sat_back_calc = -2047;
        }
        Ptr_reg->hodnota_pro_PWM = sat_back_calc; //odesilam hodnotu pro PWM
        dif_back_calc = sat_back_calc - back_calc; //urcim rozdil omezene a skutecne hodnoty
        Ptr_reg->K_T = Ptr_reg->K_I; //K_T nastavuji na K_I
        
        //pocitani iSum
        iSum = iSum + Ptr_reg->K_I * reg_odch;
        iSum = iSum + Ptr_reg->K_T * dif_back_calc;
        iSum = iSum/PREPOCET_NA_MS; //jelikoz hodnoty casovych konstant zadavam v jednotach, musim to podelit aby to bylo opet v ms
    }
    else{
        Ptr_reg->hodnota_pro_PWM = 0; //odesilam hodnotu pro PWM
    }
    
    
}




/* *****************************************************************************
 End of File
 */
