/******************************************************************************/
// Project		: TP3 Menu Gen
// Author 		: Ricardo Crespo & Giuseppe Stabile
// Date 		: 07.02.2022
// Descrition   : Fichier du header du menu du générateur
/******************************************************************************/

#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu  du générateur
// Traitement cyclique à 1 ms du Pec12

#include <stdbool.h>
#include <stdint.h>
#include "DefMenuGen.h"

#define LIGNE_FORME 1
#define LIGNE_FREQUENCE 2
#define LIGNE_AMPLITUDE 3
#define LIGNE_OFFSET 4

#define MAXVALEUR_FORME 3
#define MINVALEUR_FORME 0

//#define MAXVALEUR_FREQUENCE_HZ 2000 //Ne fonctionne pas --> Bloque a 1860
#define MAXVALEUR_FREQUENCE_HZ 1820
#define MINVALEUR_FREQUENCE_HZ 20
#define PAS_FREQUENCE_HZ 20

#define MAXVALEUR_AMPLITUDE_mV 10000
#define MINVALEUR_AMPLITUDE_mV 0
#define PAS_AMPLITUDE_mV 100

#define MAXVALEUR_OFFSET_mV 5000
#define MINVALEUR_OFFSET_mV -5000
#define PAS_OFFSET_mV 100

#define SEPARATOR 246

typedef enum  { e_Selection, e_OK, e_NOK} E_save;

void MENU_Initialize(S_ParamGen *pParam);

void MENU_Selection(void);

void MENU_ClearParameters(uint8_t ligneToClear);

void MENU_Execute(S_ParamGen *pParam,  bool local);

void MENU_Save(E_save state);

void MENU_AffichageIP(void);

#endif
