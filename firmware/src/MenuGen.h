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
#include "Mc32DriverLcd.h"

#include "GesPec12.h"
#include "GesS9.h"

//Definitions for frequency option
#define MIN_FREQUENCY 20
#define MAX_FREQUENCY 2000
#define FREQUENCY_INC 20

//Definitions for amplitude option
#define MIN_AMPLITUDE 0
#define MAX_AMPLITUDE 10000
#define AMPLITUDE_INC 100

//Definitions for offset option
#define MIN_OFFSET -5000
#define MAX_OFFSET 5000
#define OFFSET_INC 100


/*---- LCD Symbol customisation ----*/
// Separation caracter between option and value "Frequ [Hz] X 1000"
#define SEPARATOR 246	// =>
//#define SEPARATOR 61	// = (Specification sheet)


// Cursor for navigation
//#define NAVIGATION 199	// ->
//#define NAVIGATION 42	// * (Specification sheet)

// Cursor for selected option
//#define SELECTED 251	// ->| 
//#define SELECTED 63	// ? (Specification sheet)

#define BLANK 32	//Blank caracter DO NOT CHANGE

#define SAVED 45
#define CANCELED 46

#define CONFIRM_TIME 200 //Time out in [10ms] when there's no activity

typedef enum{Wave_Menu = 0, Frequency_Menu, Amplitude_Menu, Offset_Menu, Main_Menu, Save_Menu}E_Menu_State;

S_TacSwitch_Descriptor S9;

S_Pec12_Descriptor Pec12;

void MENU_Initialize(S_ParamGen *pParam);


void MENU_Execute(S_ParamGen *pParam,bool USB);



typedef enum {MenuPrincipal, MenuForme, MenuFreq, MenuAmpl, MenuOffset, MenuSave} E_Menu;

//void MENU_Initialize(S_ParamGen *pParam);
//void MENU_Execute(S_ParamGen *pParam);
void GestMenu(int16_t ValueMax,int16_t ValueMin,uint8_t Step,int16_t* PtrValue,bool* Save);
void diplay(int16_t ValFreq,int16_t ValAmp,int16_t ValOffs,int16_t Choix);



#endif




  
   







