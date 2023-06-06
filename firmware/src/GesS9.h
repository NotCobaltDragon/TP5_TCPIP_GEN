#ifndef GestS9_h
#define GestS9_h

// GesS9.h
// C. HUBER    14/05/2014
// Creation pour migration PIC32MX

// Fonctions pour la gestion du S9
//
//
// Principe : Il est nécessaire d'appeler cycliquement la fonction ScanS9
//            avec un cycle de 1 ms
//
//  Pour la gestion du S9, il y a 9 fonctions à disposition :
//       S9IsPlus       true indique un nouveau incrément
//       S9IsMinus      true indique un nouveau décrément
//       S9IsSHRT         true indique action SHRT
//       S9IsLNG        true indique action LNG
//       S9NoActivity   true indique abscence d'activité sur S9
//  Fonctions pour quittance des indications
//       S9ClearPlus    annule indication d'incrément
//       S9ClearMinus   annule indication de décrément
//       S9ClearSHRT      annule indication action SHRT
//       S9ClearLNG     annule indication action LNG
//
//
//---------------------------------------------------------------------------


// définitions des types qui seront utilisés dans cette application

#include <stdbool.h>
#include <stdint.h>

#define HOLD_TIME 500  //Hold time when pushbutton is holded down
#define TRUE 1
#define FALSE 0

// structure du descripteur du PEC
// Migration : remplacement bool par champ 1 bit
typedef struct {
            uint8_t SHRT  : 1;              // événement action SHRT
            uint8_t LNG : 1;             // événement action LNG
            uint8_t NoActivity : 1;      // Indication de non activité
            uint16_t PressDuration;     // Pour durée pression du P.B.
            uint16_t InactivityDuration;   // Pour durée inactivité
} S_TacSwitch_Descriptor;

void ScanS9 (bool ValPB);

void S9Init (void);
//       S9IsSHRT         true indique action SHRT
bool S9IsSHRT    (void);
//       S9IsLNG        true indique action LNG
bool S9IsLNG    (void);
//       S9NoActivity   true indique abscence d'activité sur S9
bool S9NoActivity    (void);

//  Fonctions pour quittance des indications
//       S9ClearSHRT      annule indication action SHRT
void S9ClearSHRT   (void);
//       S9ClearLNG     annule indication action LNG
void S9ClearLNG   (void);
void S9ClearInactivity   (void);


#endif
