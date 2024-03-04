/* ************************************************************************** */
/** funkce k filtraci


/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */#include "./../header/filtrace.h" //musim zpet abych se dostal do headeru
   #include <stdbool.h>




/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */



/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */

//inicializace struktury
  void initFiltr(FILTR_TLACITKA *Ptr_tlacitko, bool pocStav){ //posilam tam i vstupni hodnotu protoze to bude potreba v uloze 4
    Ptr_tlacitko->stav=pocStav; //jelikoz mam pointer musim predat pomoci sipek, kdy stavu prirazuji pocatecni hodnotu
    Ptr_tlacitko->tlacitkozmacknuto=0;
    };
    
//vykonna funkce filtru
void runFiltr(FILTR_TLACITKA *Ptr_tlacitko, bool vstupTlacitka){ 
//pri vbehu do funkce se mi zalozi pointer ktery bude odkazovat na danou stopu A/B tzn. v main pri volani funkce do funkce predam strukturu ktera reprezentuje danou stopu
//zaroven budu mit promennou tlacitko, ktera mi bude rikat zda prisla 0/1, tuto promennou budu muset v main pri volani funkce priradit na vstup snimace
    char dalsi_stav = s0; //na zacatku si nastavim ze pocatecni stav je s0
    
    switch(Ptr_tlacitko->stav) //do switche poslu aktualni stav a podle toho jesli prijde 0/1 na vstup jdu do dalsiho stavu
	{
	case s0: {
		if(vstupTlacitka ==1) //pokud byla 3x za sebou 1 tlacitko se mi sepne 
		{
		dalsi_stav = s1;
		}
		else
		{
		dalsi_stav=s4;
		}
		break;
			}
	case s1: {
		if(vstupTlacitka ==1)
		{
		dalsi_stav = s2;
		}
		else
		{
		dalsi_stav=s4;
		}
		break;
			}
	case s2: {
		if(vstupTlacitka ==1)
		{
		dalsi_stav = s3;
		Ptr_tlacitko ->tlacitkozmacknuto = 1; //s treti 1 bylo tlacitko bylo zmacknuto a zustane seple
		}		
		else
		{
		dalsi_stav=s4;
		}
		break;
			}
	case s3: {
		if (vstupTlacitka ==1) //pokud 3x za sebou byla 0, talcitko se mi vypne
		{
		dalsi_stav = s3;
		}
		else
		{
		dalsi_stav =s4;
		}
		break;
			}
	case s4: {
		if(vstupTlacitka ==0)
		{
		dalsi_stav = s5;
		}
		else
		{
		dalsi_stav = s1;
		}
		break;
			}
	case s5: {
		if(vstupTlacitka==0)
		{
		dalsi_stav = s6;
		Ptr_tlacitko -> tlacitkozmacknuto = 0; //s prichodem treti nuly tlacitko nebylo zmackunuto a zustane vyple
		}
		else
		{
		dalsi_stav=s1;
		}
		break;
			}
	case s6: {
		if(vstupTlacitka ==0)
		{
		dalsi_stav=s6;
		}
		else
		{
		dalsi_stav=s1;
		}
		break;
			}

	}//switch
Ptr_tlacitko->stav = dalsi_stav; //predam do stavu a pak mi to vleze do stavu po probehnuti predchoziho switche
};

//funkce pro predani - nedela nic jineho nez ze mi vraci hodnotu z funkce filtr (hodnotu do funkce predam a tu stejnou hodnotu mi to vrati)
bool getFiltrOutput(FILTR_TLACITKA *Ptr_tlacitko){ //vezmu adresu struktury z filtru 
    return Ptr_tlacitko -> tlacitkozmacknuto; //a vratim hodnotu struktury
}




/* *****************************************************************************
 End of File
 */
