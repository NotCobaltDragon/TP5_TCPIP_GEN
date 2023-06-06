#ifndef Generateur_h
#define Generateur_h

// TP3 MenuGen 2016
// C. HUBER  03.02.2016
// Fichier Generateur.h
// Prototypes des fonctions du générateur  de signal

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "DefMenuGen.h"

// T.P. 2016 100 echantillons
#define MAX_ECH 100

// Valeurs initiaux pour le générateur
// SignalSinus, SignalTriangle, SignalDentDeScie, SignalCarre
#define INIT_FREQ		20
#define INIT_FORM		SignalCarre
#define INIT_AMPLITUDE  2500
#define INIT_OFFSET     0
#define DEFAULT_OFFSET  32767 //NO TOUCH
/*----------------------------------------*/
#define MAX_VAL                  65535
#define MIN_VAL                      0
/*----------------------------------------*/

#define F_SYS                 80000000
#define PRESCALER                    1

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam);


// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam);


// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam);

// A appeler dans int Timer3
void  GENSIG_Execute(void);


#endif