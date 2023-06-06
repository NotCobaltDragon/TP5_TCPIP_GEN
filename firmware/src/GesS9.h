#ifndef GestS9_h
#define GestS9_h

// GesS9.h
// C. HUBER    14/05/2014
// Creation pour migration PIC32MX

// Fonctions pour la gestion du S9
//
//
// Principe : Il est n�cessaire d'appeler cycliquement la fonction ScanS9
//            avec un cycle de 1 ms
//
//  Pour la gestion du S9, il y a 9 fonctions � disposition :
//       S9IsPlus       true indique un nouveau incr�ment
//       S9IsMinus      true indique un nouveau d�cr�ment
//       S9IsSHRT         true indique action SHRT
//       S9IsLNG        true indique action LNG
//       S9NoActivity   true indique abscence d'activit� sur S9
//  Fonctions pour quittance des indications
//       S9ClearPlus    annule indication d'incr�ment
//       S9ClearMinus   annule indication de d�cr�ment
//       S9ClearSHRT      annule indication action SHRT
//       S9ClearLNG     annule indication action LNG
//
//
//---------------------------------------------------------------------------


// d�finitions des types qui seront utilis�s dans cette application

#include <stdbool.h>
#include <stdint.h>

#define HOLD_TIME 500  //Hold time when pushbutton is holded down
#define TRUE 1
#define FALSE 0

// structure du descripteur du PEC
// Migration : remplacement bool par champ 1 bit
typedef struct {
            uint8_t SHRT  : 1;              // �v�nement action SHRT
            uint8_t LNG : 1;             // �v�nement action LNG
            uint8_t NoActivity : 1;      // Indication de non activit�
            uint16_t PressDuration;     // Pour dur�e pression du P.B.
            uint16_t InactivityDuration;   // Pour dur�e inactivit�
} S_TacSwitch_Descriptor;

void ScanS9 (bool ValPB);

void S9Init (void);
//       S9IsSHRT         true indique action SHRT
bool S9IsSHRT    (void);
//       S9IsLNG        true indique action LNG
bool S9IsLNG    (void);
//       S9NoActivity   true indique abscence d'activit� sur S9
bool S9NoActivity    (void);

//  Fonctions pour quittance des indications
//       S9ClearSHRT      annule indication action SHRT
void S9ClearSHRT   (void);
//       S9ClearLNG     annule indication action LNG
void S9ClearLNG   (void);
void S9ClearInactivity   (void);


#endif
