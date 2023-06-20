#ifndef Mc32Gest_SERCOMM_H
#define Mc32Gest_SERCOMM_H
/*--------------------------------------------------------*/
// Mc32Gest_SerComm.h
/*--------------------------------------------------------*/
//	Description :	emission et reception specialisee
//			pour TP4 2015-2016
//
//	Auteur 		: 	C. HUBER
//
//	Version		:	V1.2
//	Compilateur	:	XC32 V1.40 + Harmony 1.06
//
/*--------------------------------------------------------*/

#include <stdint.h>
#include "DefMenuGen.h"
#include <stdbool.h>

/*--------------------------------------------------------*/
// Prototypes des fonctions 
/*--------------------------------------------------------*/


#define MAXVALEUR_FORME 3
#define MINVALEUR_FORME 0

#define MAXVALEUR_FREQUENCE_HZ 1820
#define MINVALEUR_FREQUENCE_HZ 20

#define MAXVALEUR_AMPLITUDE_mV 10000
#define MINVALEUR_AMPLITUDE_mV 0

#define MAXVALEUR_OFFSET_mV 5000
#define MINVALEUR_OFFSET_mV -5000

#define MAXVALEUR_W 1
#define MINVALEUR_W 0


#define START_DATA '!'
#define END_DATA '#'
#define SIZE_MAX_OF_DATA 28

#define CHAR_AFTER_VALUE "x="

#define SIZE_MAX_VALUE 6 //+10'000 = 6 char
#define NBR_VALUE 5
#define NBR_FORME_SIGNAL 4

#define SIGNAL_VALUE 'S'
#define FREQ_VALUE 'F'
#define AMPL_VALUE 'A'
#define OFFS_VALUE 'O'
#define W_VALUE 'W'


#define START_LETTRE_MAJ_ASCII 0x41
#define END_LETTRE_MAJ_ASCII 0x5A

#define ZERO_ASCII 0x30
#define NEUF_ASCII 0x39

/*--------------------------------------------------------*/
// Prototypes des fonctions 
/*--------------------------------------------------------*/
void SendMessage(S_ParamGen *pParam, bool Saved);
bool GetMessage(int8_t *TCPReadBuffer, S_ParamGen *pParam, bool *SaveTodo);
E_FormesSignal getValueForme(uint8_t charactere);
int16_t getValueFreqAmplOffsW(char* chaine, char valueParameter);
int16_t testValue(int16_t value, char parameterToTest);
bool GetDataIsOK(int8_t* chaine);


#endif
