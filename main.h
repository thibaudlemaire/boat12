/*********************************************
***************  H E A D E R  ****************
**********************************************
****************** main.h ********************
**********************************************
**** Thibaud LEMAIRE et Pierre CORDIEE-ROY ***
**** Mini-projet ISI 2009-2010 ***************
*********************************************/
	
#ifndef M1e // Directive de programation

#include <pic.h> // Déclaration de PIC 16F1827

// DEFINES
#define M1e RB4
#define M1i1 RA7
#define M1i2 RA1
#define M2e RB5
#define M2i1 RA0
#define M2i2 RA6
#define Cg RB2
#define Cd RB3
#define Start RB0 

// DECLARATIONS
char patte_7;
char led;
struct 
{
	union
	{
		unsigned volatile char general;
		struct
		{
			unsigned volatile attente_init				: 1;
			unsigned volatile initialisation			: 1;
			unsigned volatile attente_start				: 1;
			unsigned volatile cycle_en_cours			: 1;
			unsigned volatile fin_de_cycle				: 1;                                                                                                                                                        
 			unsigned volatile aquisition				: 1;
		} general_bits;
		struct
		{
			unsigned volatile stades					: 5;
			unsigned volatile autres					: 1;
		} general_groupes;
		unsigned volatile char vitesse;
	} general;
	union
	{
		unsigned volatile char cycle;
		struct
		{
			unsigned volatile ph1						: 1;
			unsigned volatile ph2						: 1;
			unsigned volatile ph3						: 1;
			unsigned volatile ph4						: 1;
			unsigned volatile ph5						: 1;
			unsigned volatile ph6						: 1;
			unsigned volatile first_bord				: 1;
			unsigned volatile first_tour				: 1;
		} cycle_bits;
		struct
		{
			unsigned volatile phases					: 6;
			unsigned volatile firsts					: 2;
		} cycle_groupes;
	} cycle;
	union	
	{
		unsigned volatile char bords;
		struct
		{
			unsigned volatile bord1							:1;
			unsigned volatile bord2							:1;
			unsigned volatile bord3							:1;
			unsigned volatile bord4							:1;
		} bords_bits;
	} bords;
	struct
	{
		unsigned volatile vitesse							:5;
		unsigned volatile avance							:1;
		unsigned volatile recule 							:1;
		unsigned volatile tourne							:1;
	} vitesse;
	unsigned volatile char azimut;
	unsigned volatile char azimut_consigne;
	unsigned volatile char consigne_capteur1;
	unsigned volatile char consigne_capteur2;
	unsigned volatile char offset_capteur1;
	unsigned volatile char offset_capteur2;
	unsigned volatile char vmax_capteur1;
	unsigned volatile char vmax_capteur2;
	unsigned volatile char amplitude_capteur1;
	unsigned volatile char amplitude_capteur2;
	unsigned volatile char init_tourne;
	unsigned volatile char init_capt1_val;
} etat;
struct 
{
	struct
	{
		union
		{
			unsigned volatile char led_con;
			struct
			{
				unsigned volatile increment					: 7;
				unsigned volatile active					: 1;
			} led_con_bits;
		} led_con;
		unsigned volatile char increment;
	} led;
	struct
	{	
		union
		{
			unsigned volatile char tempo1_con;
			struct
			{
				unsigned volatile tempo1_active 		: 1;
				unsigned volatile tempo1_application	: 1;
				unsigned volatile prediviseur			: 3;
				unsigned volatile niveau_prediviseur	: 3;
			} tempo1_con_bits;
		} tempo1_con;
		unsigned volatile short int tempo1_compteur;
	} tempo1;
	struct
	{	
		unsigned volatile short int bord1;
		unsigned volatile short int bord2;
		unsigned volatile short int bord3;
		unsigned volatile short int bord4;
		union
		{
			unsigned volatile char bords_con;
			struct
			{
				unsigned volatile enregistrement				:1;
				unsigned volatile bord1							:1;
				unsigned volatile bord2							:1;
				unsigned volatile bord3							:1;
				unsigned volatile bord4							:1;
				unsigned volatile niveau_prediviseur			:3;
			} bords_con_bits;
			struct
			{
				unsigned volatile enregistrement				:1;
				unsigned volatile bords							:4;
				unsigned volatile niveau_prediviseur			:3;
			} bords_con_groupes;
		} bords_con;
	} bords;
	struct
	{
		union
		{	
			unsigned volatile char file_con_moteurs;
			struct 
			{
				unsigned volatile active						:1;
				unsigned volatile 								:2;
				unsigned volatile dac							:5;
			} file_moteurs_con_bits;
		} file_moteurs_con;
		unsigned volatile char compteur;
	} file_moteurs;
	unsigned volatile char tempo_bouton;
	unsigned volatile char tempo_aquisition;
} tempos;
struct
{
	unsigned volatile char potard;
	unsigned volatile char capteur1;
	unsigned volatile char capteur2;
} aquisitions;

const char t[36] = {65,76,89,100,112,29,42,56,72,83,95,110,122,132,116,108,98,88,77,65,54,43,32,143,131,117,101,85,72,56,42,33,27,38,46,56};
unsigned char cap1_sans_offset;
unsigned char cap2_sans_offset;
// Prototypes
void set_tempo1(unsigned short int, int);
void set_cligno(int);
void avancer();
void tourner();
void reculer();
void stop();
void set_azimut_consigne();
void set_capteur_consigne();
void set_vitesse();
void manoeuvres();
char capteur1();
char capteur2();

#endif