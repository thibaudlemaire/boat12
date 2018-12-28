/*********************************************
*** F O N C T I O N   P R I N C I P A L E ****
**********************************************
******************* main.c *******************
**********************************************
**** Thibaud LEMAIRE et Pierre CORDIEE-ROY ***
**** Mini-projet ISI 2009-2010 ***************
*********************************************/
#include "main.h"

void main()
{
	// INIT VARs
	led=0;
	
	// CONFIG PORTS 
	ANSELA	= 0b00010001; // Set analog ports
	ANSELB	= 0b00000010; // ...
	TRISA	= 0b10110011; // Set Read/Write ports
	TRISB	= 0b11001111; // ...
	LATB	= 0b00110000; // Set value to the latch
	LATA	= 0b01000000; // ...

	// CONFIG TMRS
	T2CON	= 0b01001101;
	TMR2IE 	= 1;
	PR2 	= 250;

	// OTHER CONFIG
	GIE		= 1; // Global interrupt enable
	PEIE 	= 1; // Peripherical interupt enable
	IRCF0	= 0b1; // Set internal oscillator 4MHz
	IRCF1	= 0b0;
	IRCF2	= 0b1;
	IRCF3	= 0b1;

	// CONFIG Analog->Digit
	ADCON1 	= 0b00000000;
	ADCON0 	= 0b00010001;
	ADIF	= 0; // Flag à 0
	ADIE	= 1; // Inerrupt enable
	GO_nDONE= 1; // Start conversion

		//// CCP3 ////
	// -- Mode PWM CONFIG
	CCPR3L=24;
	CCPTMRS=0b00100000;
	CCP3CON=0b00001100;
	// -- CCP3 TIMER6
	PR6=0b11111111;
	T6CON=0b01001111;

	// CONFIG Digit->Analog
	DACCON0=0b10100000;	
	DACCON1=0b00000000;
	
	etat.offset_capteur1 = 255;
	etat.offset_capteur2 = 255;

	//	Set tempo bouton 
	tempos.tempo_bouton = 50;

	etat.general.general_bits.attente_init = 1; // Permet le lancement
	
	while(1)
	{
		if(etat.general.general_bits.attente_init)
		{
			stop();
			set_cligno(2);
			while(etat.general.general_bits.attente_init)
			{
				manoeuvres();
			}
		}
		
		if(etat.general.general_bits.initialisation)
		{	
			set_cligno(1);
			set_tempo1(450, 4);
			while(tempos.tempo1.tempo1_con.tempo1_con_bits.tempo1_active && etat.general.general_bits.initialisation)
			{
				manoeuvres();
			}
			if(etat.general.general_bits.initialisation)
				etat.general.general_groupes.stades = etat.general.general_groupes.stades<<1;
			etat.amplitude_capteur1 = etat.vmax_capteur1 - etat.offset_capteur1;
			etat.amplitude_capteur2 = etat.vmax_capteur2 - etat.offset_capteur2;
			stop();
		}
		
		if(etat.general.general_bits.attente_start)
		{
			set_cligno(3);
			while(etat.general.general_bits.attente_start)
			{
				manoeuvres();
			}
		}
		
		if(etat.general.general_bits.cycle_en_cours)
		{
			set_cligno(1);
			etat.cycle.cycle_bits.ph1 = 1;
			etat.cycle.cycle_bits.first_tour = 1;
			tempos.bords.bords_con.bords_con_groupes.bords = 1;

			while(etat.general.general_bits.cycle_en_cours)
			{
				if(etat.cycle.cycle_bits.ph1 && etat.general.general_bits.cycle_en_cours)
				{
					avancer();
					set_capteur_consigne();
					if(etat.cycle.cycle_bits.first_tour)
					{
						tempos.bords.bords_con.bords_con_bits.enregistrement = 1;
						
						while(etat.cycle.cycle_bits.ph1 && etat.general.general_bits.cycle_en_cours)
						{
							manoeuvres();
						}

						tempos.bords.bords_con.bords_con_bits.enregistrement = 0;

						if(!tempos.bords.bords_con.bords_con_bits.bord4)
							tempos.bords.bords_con.bords_con_groupes.bords = tempos.bords.bords_con.bords_con_groupes.bords<<1;
						else
						{	
							etat.cycle.cycle_bits.first_tour = 0;
							etat.bords.bords = 1;
						}
					}
					else
					{
						short int temp;
						if(etat.bords.bords_bits.bord1)
						{	
							temp = tempos.bords.bord1;
							tempos.bords.bord1 = tempos.bords.bord1 - 20;
						}
						if(etat.bords.bords_bits.bord2)
						{	
							temp = tempos.bords.bord2;
							tempos.bords.bord2 = tempos.bords.bord2 - 20;
						}
						if(etat.bords.bords_bits.bord3)
						{	
							temp = tempos.bords.bord3;
							tempos.bords.bord3 = tempos.bords.bord3 - 20;
						}
						if(etat.bords.bords_bits.bord4)
						{	
							temp = tempos.bords.bord4;
							tempos.bords.bord4 = tempos.bords.bord4 - 20;
						}
						
						if(temp >= 40) // Si il reste assez de temps
							set_tempo1(temp - 20,4); // On fait le bord
						else // Sinon, retour à la berge
							etat.general.general_groupes.stades = etat.general.general_groupes.stades<<1;

						while(tempos.tempo1.tempo1_con.tempo1_con_bits.tempo1_active 
								&& etat.general.general_bits.cycle_en_cours
								&& etat.cycle.cycle_bits.ph1)
						{
							manoeuvres();
						}
						if(!etat.bords.bords_bits.bord4)
							etat.bords.bords = etat.bords.bords<<1;
						else
							etat.bords.bords = 1;
						etat.cycle.cycle_groupes.phases = etat.cycle.cycle_groupes.phases<<2;
					}
					set_azimut_consigne();
				}
				if(etat.cycle.cycle_bits.ph2 && etat.general.general_bits.cycle_en_cours)
				{	
						reculer(); // Recule 
						set_tempo1(60,4); // Pendant 3 secondes
						while(tempos.tempo1.tempo1_con.tempo1_con_bits.tempo1_active 
								&& etat.general.general_bits.cycle_en_cours
								&& etat.cycle.cycle_bits.ph2)
						{
							manoeuvres();
						}
						if(etat.cycle.cycle_bits.ph2 && etat.general.general_bits.cycle_en_cours) 
							// Si la fin de la boucle vient de la tempo
							etat.cycle.cycle_groupes.phases = etat.cycle.cycle_groupes.phases<<1;
				}
				if(etat.cycle.cycle_bits.ph3 && etat.general.general_bits.cycle_en_cours)
				{
					tourner(); // Tourne à droite
					while(etat.cycle.cycle_bits.ph3 && etat.general.general_bits.cycle_en_cours)
					{
						manoeuvres();
					}
					etat.cycle.cycle_groupes.phases = 1;
				}
			}
		}

		if(etat.general.general_bits.fin_de_cycle)
		{
			set_cligno(2);
			while(etat.general.general_bits.fin_de_cycle)
			{
			
			}
		}
	}
}