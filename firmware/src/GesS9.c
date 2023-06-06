// GesS9.c  Canevas pour réalisation  
// C. HUBER    09/02/2015

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

#include "GesS9.h"
#include "Mc32Debounce.h"

// Descripteur des sinaux
S_SwitchDescriptor DescrS9;

// Structure pour les traitement du S9
S_TacSwitch_Descriptor S9;

void ScanS9 (bool ValS9)
{
   //static uint8_t Hold_Counter = 0;

   // Traitement antirebond sur PB
   DoDebounce (&DescrS9, ValS9);
   
   // Traitement du PushButton
   if(S9.PressDuration > 0)
   {
      S9.PressDuration++;
   }

   if(DebounceIsPressed(&DescrS9)==TRUE)
   {
      S9.PressDuration = 1;
      DebounceClearPressed(&DescrS9);
   }

   if((DebounceIsReleased(&DescrS9)==TRUE)&&(S9.PressDuration > 0))
   {
      if(S9.PressDuration>=HOLD_TIME)
      {
         S9.LNG = TRUE;
         //S9.PressDuration = 0;
      }
      else// if(S9.PressDuration>=0)
      {
         S9.SHRT = TRUE;
         //S9.PressDuration = 0;
      }
      S9.PressDuration = 0;
      DebounceClearReleased(&DescrS9);
   }
 }

void S9Init (void)
{
   // Initialisation des descripteurs de touches S9
   DebounceInit(&DescrS9);
   
   // Init de la structure S9
    S9.SHRT = 0;              // événement action SHRT
    S9.LNG = 0;             // événement action LNG
    S9.NoActivity = 0;      // Indication d'activité
    S9.PressDuration = 0;   // Pour durée pression du P.B.
    S9.InactivityDuration = 0; // Durée inactivité
 } // S9Init

//       S9IsSHRT         true indique action SHRT
bool S9IsSHRT    (void) {
   return (S9.SHRT);
}

//       S9IsLNG        true indique action LNG
bool S9IsLNG    (void) {
   return (S9.LNG);
}

//       S9NoActivity   true indique abscence d'activité sur S9
bool S9NoActivity    (void) {
   return (S9.NoActivity);
}

//       S9ClearSHRT      annule indication action SHRT
void S9ClearSHRT   (void) {
   S9.SHRT = 0;
}

//       S9ClearLNG     annule indication action LNG
void S9ClearLNG   (void) {
   S9.LNG = 0;
}

void S9ClearInactivity   (void) {
  S9.NoActivity = 0;
  S9.InactivityDuration = 0;
}


