/*********************************************
**********  G E S T I O N    D E S  **********
********* I N T E R R U P T I O N S **********
**********************************************
**************** interrupts.c ****************
**********************************************
**** Thibaud LEMAIRE et Pierre CORDIEE-ROY ***
**** Mini-projet ISI 2009-2010 ***************
*********************************************/

#include "main.h"

void interrupt trait_inter(void)
{
	if(TMR2IF)
	{	
		if(tempos.bords.bords_con.bords_con_bits.enregistrement)
		{
			if(tempos.bords.bords_con.bords_con_bits.niveau_prediviseur == 4)
			{
				if(tempos.bords.bords_con.bords_con_bits.bord1)
					tempos.bords.bord1++;
				if(tempos.bords.bords_con.bords_con_bits.bord2)
					tempos.bords.bord2++;
				if(tempos.bords.bords_con.bords_con_bits.bord3)
					tempos.bords.bord3++;
				if(tempos.bords.bords_con.bords_con_bits.bord4)
					tempos.bords.bord4++;
				tempos.bords.bords_con.bords_con_bits.niveau_prediviseur = 0;
			}
			else
			tempos.bords.bords_con.bords_con_bits.niveau_prediviseur++;
		}
		if(tempos.tempo1.tempo1_con.tempo1_con_bits.tempo1_active)
		{
			if(tempos.tempo1.tempo1_con.tempo1_con_bits.niveau_prediviseur 
				>= tempos.tempo1.tempo1_con.tempo1_con_bits.prediviseur)
			{
				tempos.tempo1.tempo1_compteur--;
				if(tempos.tempo1.tempo1_compteur == 0)
					tempos.tempo1.tempo1_con.tempo1_con_bits.tempo1_active = 0;
				tempos.tempo1.tempo1_con.tempo1_con_bits.niveau_prediviseur = 0;
			}
			else
			tempos.tempo1.tempo1_con.tempo1_con_bits.niveau_prediviseur++;	
		}
		if(tempos.led.led_con.led_con_bits.active)
		{
			tempos.led.increment--;
			if(tempos.led.increment == 0)
			{
				tempos.led.increment = tempos.led.led_con.led_con_bits.increment;
				LATA6 = !LATA6;
			}
		}
		if(tempos.file_moteurs.compteur != 0)
		{
			tempos.file_moteurs.compteur--;
			if(tempos.file_moteurs.compteur == 0)
				tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.active = 1;
		}
		if(tempos.tempo_bouton != 0)
		{
			tempos.tempo_bouton--;	
		}
		if(tempos.tempo_aquisition != 0)
		{	
			tempos.tempo_aquisition--;
			if(tempos.tempo_aquisition == 0)
				GO_nDONE = 1;
		}

	TMR2IF=0;
	}

	// INTERRUPTION Analog->Digit
	if(ADIF)
	{
		if(ADCON0bits.CHS == 0b00100) // AN4
		{
			aquisitions.potard = ADRESH;
			ADCON0bits.CHS = 0b01011;
			GO_nDONE = 1;
		}
		else if(ADCON0bits.CHS == 0b01011) // AN11
		{
			aquisitions.capteur1 = ADRESH;
			ADCON0bits.CHS = 0b00000;
			GO_nDONE = 1;
		}
		else if(ADCON0bits.CHS == 0b00000) // AN0
		{
			aquisitions.capteur2 = ADRESH;
			ADCON0bits.CHS = 0b00100;
			tempos.tempo_aquisition = 21;
			etat.general.general_bits.aquisition = 1;
		}
		ADIF=0; // RAZ du Flag
	}
	
}
