/******************************************************************************/
// Project		: TP3 Menu Gen
// Author 		: Ricardo Crespo & Giuseppe Stabile
// Date 		: 07.02.2022
// Descrition   : Fichier du header du g�n�rateur
/******************************************************************************/

#ifndef Generateur_h
#define Generateur_h

// TP3 MenuGen 2016
// C. HUBER  03.02.2016
// Fichier Generateur.h
// Prototypes des fonctions du g�n�rateur  de signal

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "DefMenuGen.h"


// Initialisation du  g�n�rateur
void  GENSIG_Initialize(S_ParamGen *pParam);


// Mise � jour de la periode d'�chantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam);


// Mise � jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam);

// A appeler dans int Timer3
void  GENSIG_Execute(void);

//Conversion de la valeur de tension calcul� pour convertir en valeur ADC
int32_t ConvAOP (int16_t Tension);
//test de la variable x en fonction des param�tre d'entr�e et sortie min et max en int32_t
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
//test de la variable x en fonction des param�tre d'entr�e et sortie min et max en float
float map_float(float x, float in_min, float in_max, float out_min, float out_max);
//test de la valeur minimale et maximale de notre ADC
int32_t TestMinMax(int32_t InputVal);


#endif