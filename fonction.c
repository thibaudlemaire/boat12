/*********************************************
************** F O N C T I O N S *************
**********************************************
**************** fonctions.c *****************
**********************************************
**** Thibaud LEMAIRE et Pierre CORDIEE-ROY ***
**** Mini-projet ISI 2009-2010 ***************
*********************************************/

#include "main.h"

void set_cligno(int type)
{
	// Cligno : 1 --> Continu
	// 2 --> Lent
	// 3 --> Rapide
	
	if(type==1)
	{
		tempos.led.led_con.led_con_bits.active 			= 0;
		LATA6 											= 1;
	}
	if(type==2)
	{
		tempos.led.led_con.led_con_bits.increment		= 75;
		tempos.led.increment							= 75;
		tempos.led.led_con.led_con_bits.active			= 1;
	}
	if(type==3)
	{
		tempos.led.led_con.led_con_bits.increment		= 15;
		tempos.led.increment							= 15;
		tempos.led.led_con.led_con_bits.active			= 1;
	}

}
void set_azimut_consigne()
{
	if(etat.azimut <= 26)
		etat.azimut_consigne = etat.azimut + 9;
	else
		etat.azimut_consigne = etat.azimut - 27;
}	
void set_tempo1(unsigned short int compteur, int prediviseur)
{

	tempos.tempo1.tempo1_con.tempo1_con_bits.prediviseur 		= prediviseur;
	tempos.tempo1.tempo1_compteur								= compteur;
	tempos.tempo1.tempo1_con.tempo1_con_bits.tempo1_active 		= 1;
}
void avancer()
{
	etat.vitesse.avance = 1;
	etat.vitesse.recule = 0;
	etat.vitesse.tourne = 0;
	CCPR3L = 24;
	if(DACCON1 == 0)
	{
		LATB4 = 1;
		LATB5 = 1;
		DACCON1 = etat.vitesse.vitesse;
	}
	else
	{
		DACCON1 = 0;
		LATB4 	= 1;
		LATB5	= 1;
		tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.dac = etat.vitesse.vitesse;
		tempos.file_moteurs.compteur = 51;
	}
}
void tourner()
{
	etat.vitesse.avance = 0;
	etat.vitesse.recule = 0;
	etat.vitesse.tourne = 1;
	CCPR3L = 29;
	if(DACCON1 == 0)
	{
		LATB4 = 1;
		LATB5 = 0;
		DACCON1 = etat.vitesse.vitesse;
	}
	else
	{
		DACCON1 = 0;
		LATB4 	= 1;
		LATB5	= 0;
		tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.dac = etat.vitesse.vitesse;
		tempos.file_moteurs.compteur = 51;
	}
}
void reculer()
{
	etat.vitesse.avance = 0;
	etat.vitesse.recule = 1;
	etat.vitesse.tourne = 0;
	CCPR3L = 24;
	if(DACCON1 == 0)
	{
		LATB4 = 0;
		LATB5 = 0;
		DACCON1 = etat.vitesse.vitesse - 5;
	}
	else
	{
		DACCON1 = 0;
		LATB4 	= 0;
		LATB5	= 0;
		tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.dac = etat.vitesse.vitesse - 5;
		tempos.file_moteurs.compteur = 51;
	}
}
void stop()
{
etat.vitesse.avance = 0;
etat.vitesse.recule = 0;
etat.vitesse.tourne = 0;
CCPR3L = 24;
DACCON1 = 0;
}
void set_capteur_consigne()
{
	etat.consigne_capteur1 = aquisitions.capteur1;
	etat.consigne_capteur2 = aquisitions.capteur2;
}
void set_vitesse()
{
	etat.vitesse.vitesse = (aquisitions.potard * 0.07) + 13;
}
char capteur1()
{
	char capteur1_temp;
	if(aquisitions.capteur1 >= etat.offset_capteur1)
		capteur1_temp = aquisitions.capteur1 - etat.offset_capteur1;
	else
		capteur1_temp = 0;
	return (capteur1_temp * 173) / etat.amplitude_capteur1;
}
char capteur2()
{
	char capteur2_temp;
	if(aquisitions.capteur2 >= etat.offset_capteur2)
		capteur2_temp = aquisitions.capteur2 - etat.offset_capteur2;
	else
		capteur2_temp = 0;
	return (capteur2_temp * 137) / etat.amplitude_capteur2;
}
void manoeuvres()
{
	cap1_sans_offset = aquisitions.capteur1 - etat.offset_capteur1;
	cap2_sans_offset = aquisitions.capteur2 - etat.offset_capteur2;

	if(etat.vitesse.avance || etat.vitesse.tourne)
	{
		DACCON1 = etat.vitesse.vitesse;
	}
	if(etat.vitesse.recule)
	{
		DACCON1 = etat.vitesse.vitesse - 5;
	}
	if(tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.active)
	{
		DACCON1 = tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.dac;
		tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.active		= 0;
		tempos.file_moteurs.file_moteurs_con.file_moteurs_con_bits.dac			= 0;
	}
	if(RB0 && (tempos.tempo_bouton == 0))
	{
		if(etat.general.general_bits.attente_init || etat.general.general_bits.attente_start)
		{
			etat.general.general_groupes.stades = etat.general.general_groupes.stades<<1;
		}
		else if(etat.general.general_bits.initialisation 
					|| etat.general.general_bits.cycle_en_cours 
					|| etat.general.general_bits.fin_de_cycle )
			{
				 etat.general.general_groupes.stades = 0b00001;
			}
		tempos.tempo_bouton = 101;
	}
	if(RB2 && RB3 && etat.general.general_bits.cycle_en_cours && etat.cycle.cycle_bits.ph1 && etat.cycle.cycle_bits.first_tour)
	{
		etat.cycle.cycle_groupes.phases = etat.cycle.cycle_groupes.phases<<1;
	}
	if(etat.cycle.cycle_bits.ph3)
	{
		if(etat.azimut == etat.azimut_consigne)
			etat.cycle.cycle_bits.ph3 = 0;
	}
	if(etat.cycle.cycle_bits.ph1 && etat.general.general_bits.cycle_en_cours)
	{
		unsigned char ecart;
		if(etat.azimut <= 4 || etat.azimut >= 32)
		{
			if(etat.consigne_capteur2 >= aquisitions.capteur2)
			{
				ecart = etat.consigne_capteur2 - aquisitions.capteur2;
				if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 26;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 28;
				else
					CCPR3L = 30;
			}
			else
			{
				ecart = aquisitions.capteur2 - etat.consigne_capteur2;
				if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 22;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 20;
				else
					CCPR3L = 18;
			}
		}
		else if (etat.azimut >= 5 && etat.azimut <= 13)
		{
			if(etat.consigne_capteur1 >= aquisitions.capteur1)
			{
				ecart = etat.consigne_capteur1 - aquisitions.capteur1;
				if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 26;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 28;
				else
					CCPR3L = 30;
			}
			else
			{
				ecart = aquisitions.capteur1 - etat.consigne_capteur1;
				if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 22;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 20;
				else
					CCPR3L = 18;
			}
		}
		else if (etat.azimut >= 14 && etat.azimut <= 22)
		{
			if(etat.consigne_capteur2 >= aquisitions.capteur2)
			{
				ecart = etat.consigne_capteur2 - aquisitions.capteur2;
				if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 22;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 20;
				else
					CCPR3L = 18;
			}
			else
			{
				ecart = aquisitions.capteur2 - etat.consigne_capteur2;
				if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 26;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 28;
				else
					CCPR3L = 30;
			}
		}
		else if (etat.azimut >= 23 && etat.azimut <= 31)
		{
			if(etat.consigne_capteur1 >= aquisitions.capteur1)
			{
				ecart = etat.consigne_capteur1 - aquisitions.capteur1;
					if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 22;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 20;
				else
					CCPR3L = 18;
			}
			else
			{
				ecart = aquisitions.capteur1 - etat.consigne_capteur1;
				if(ecart <= 5)
					CCPR3L = 24;
				else if (ecart > 5 && ecart <= 7)
					CCPR3L = 26;
				else if (ecart > 7 && ecart <= 9)
					CCPR3L = 28;
				else
					CCPR3L = 30;
			}
		}
	}
	if(etat.general.general_bits.initialisation)
	{
		if(aquisitions.capteur1 < etat.offset_capteur1)
			etat.offset_capteur1 = aquisitions.capteur1;
		if(aquisitions.capteur2 < etat.offset_capteur2)
			etat.offset_capteur2 = aquisitions.capteur2;
		if(aquisitions.capteur1 > etat.vmax_capteur1)
			etat.vmax_capteur1 = aquisitions.capteur1;
		if(aquisitions.capteur2 > etat.vmax_capteur2)
			etat.vmax_capteur2 = aquisitions.capteur2;

	}
	if(etat.general.general_bits.aquisition)
	{
		set_vitesse();
		if(capteur1() <= 84)
		{
			if(capteur2() >= 24) // 320° -> 90°
			{
				if(capteur2() <= 121 ) // 320° -> 45°
				{
					if(capteur2() >= 65) // 0° -> 45°
					{
						unsigned char ecart_min = 255;
						unsigned char i;
						unsigned char azimut_temp;
						for(i=0; i<=4; i++)
						{
							char ecart_temp;
							if(capteur2() >= t[i])
								ecart_temp = capteur2() - t[i];
							else
								ecart_temp = t[i] - capteur2();
							if(ecart_temp <= ecart_min)
							{
								ecart_min = ecart_temp;
								azimut_temp = i;
							}
							else
							{
								azimut_temp = i-1;
								i = 4;
							}							
						}
						etat.azimut = azimut_temp;
					}
					else // 320° -> 0°
					{
						unsigned char ecart_min = 255;
						unsigned char i;
						unsigned char azimut_temp;
						for(i=32; i<=35; i++)
						{
							char ecart_temp;
							if(capteur2() >= t[i])
								ecart_temp = capteur2() - t[i];
							else
								ecart_temp = t[i] - capteur2();
							if(ecart_temp <= ecart_min)
							{
								ecart_min = ecart_temp;
								azimut_temp = i;
							}
							else
							{
								azimut_temp = i-1;
								i = 35;
							}							
						}
						etat.azimut = azimut_temp;
					}
				}
				else // 45° -> 90°
				{
					unsigned char ecart_min = 255;
					unsigned char i;
					unsigned char azimut_temp;
					for(i=5; i<=8; i++)
					{
						char ecart_temp;
						if(capteur1() >= t[i])
							ecart_temp = capteur1() - t[i];
						else
							ecart_temp = t[i] - capteur1();
						if(ecart_temp <= ecart_min)
						{
							ecart_min = ecart_temp;
							azimut_temp = i;
						}
						else
						{
							azimut_temp = i-1;
							i = 8;
						}							
					}
					etat.azimut = azimut_temp;
				}
			}
			else // 270° -> 320°
			{
				unsigned char ecart_min = 255;
				unsigned char i;
				unsigned char azimut_temp;
				for(i=27; i<=31; i++)
				{
					char ecart_temp;
					if(capteur1() >= t[i])
						ecart_temp = capteur1() - t[i];
					else
						ecart_temp = t[i] - capteur1();
					if(ecart_temp <= ecart_min)
					{
						ecart_min = ecart_temp;
						azimut_temp = i;
					}
					else
					{
						azimut_temp = i-1;
						i = 31;
					}							
				}
				etat.azimut = azimut_temp;
			}
		}
		else
		{
			if(capteur2() <= 116) // 140° -> 270°
			{
				if(capteur2() >= 21) // 140° -> 230°
				{
					if(capteur2() <= 77) // 180° -> 230°
					{
						unsigned char ecart_min = 255;
						unsigned char i;
						unsigned char azimut_temp;
						for(i=18; i<=23; i++)
						{
							char ecart_temp;
							if(capteur2() >= t[i])
								ecart_temp = capteur2() - t[i];
							else
								ecart_temp = t[i] - capteur2();
							if(ecart_temp <= ecart_min)
							{
								ecart_min = ecart_temp;
								azimut_temp = i;
							}
							else
							{
								azimut_temp = i-1;
								i = 23;
							}							
						}
						etat.azimut = azimut_temp;
					}
					else // 140° -> 180°
					{
						unsigned char ecart_min = 255;
						unsigned char i;
						unsigned char azimut_temp;
						for(i=14; i<=17; i++)
						{
							char ecart_temp;
							if(capteur2() >= t[i])
								ecart_temp = capteur2() - t[i];
							else
								ecart_temp = t[i] - capteur2();
							if(ecart_temp <= ecart_min)
							{
								ecart_min = ecart_temp;
								azimut_temp = i;
							}
							else
							{
								azimut_temp = i-1;
								i = 17;
							}							
						}
						etat.azimut = azimut_temp;
					}
				}
				else // 230° -> 270°
				{
					unsigned char ecart_min = 255;
					unsigned char i;
					unsigned char azimut_temp;
					for(i=23; i<=26; i++)
					{
						char ecart_temp;
						if(capteur1() >= t[i])
							ecart_temp = capteur1() - t[i];
						else
							ecart_temp = t[i] - capteur1();
						if(ecart_temp <= ecart_min)
						{
							ecart_min = ecart_temp;
							azimut_temp = i;
						}
						else
						{
							azimut_temp = i-1;
							i = 26;
						}							
					}
					etat.azimut = azimut_temp;
				}
			}
			else // 90° -> 130°
			{
				unsigned char ecart_min = 255;
				unsigned char i;
				unsigned char azimut_temp;
				for(i=9; i<=13; i++)
				{
					char ecart_temp;
					if(capteur1() >= t[i])
						ecart_temp = capteur1() - t[i];
					else
						ecart_temp = t[i] - capteur1();
					if(ecart_temp <= ecart_min)
					{
						ecart_min = ecart_temp;
						azimut_temp = i;
					}
					else
					{
						azimut_temp = i-1;
						i = 13;
					}							
				}
				etat.azimut = azimut_temp;
			}
		}
		etat.general.general_bits.aquisition = 0;
	}	
}