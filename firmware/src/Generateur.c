/******************************************************************************/
// Project		: TP3 Menu Gen
// Author 		: Ricardo Crespo & Giuseppe Stabile
// Date 		: 07.02.2022
// Descrition   : Fichier du g�n�rateur
/******************************************************************************/

// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du g�n�2rateur

// Pr�vu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber

#include "bsp.h"
#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "math.h"
#include "Mc32gestI2cSeeprom.h"


#define MAX_ECH           100           // Echantillon max
#define MIN_ECH           0             // Echantillon min

#define UC_FREQUENCY      80000000      // 80 MHz

#define TENSION_MIN_POS   0             // Tension min de sortie positive AOP
#define TENSION_MAX_POS   10000         // Tension max de sortie positive AOP

#define TENSION_MIN_NEG   -10000        // Tension min de sortie n�gative AOP
#define TENSION_MAX_NEG   0             // Tension max de sortie n�gative AOP

#define DAC_MAX           65535         // Valeur max du DAC 2^16 - 1
#define DAC_MIDDLE        32768         // Valeur moyenne DAC (2^16)/2
#define DAC_MIN           0             // Valeur min DAC

int32_t TbEch[100] = {0};               // Tableau 100 cases pour g�n�rer les signaux


// Initialisation du  g�n�rateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    //Lecture memoire
    I2C_ReadSEEPROM((void*)pParam, ADRESSE_STRUCT_EEPROM, sizeof(*pParam));    
    //Si on retrouve la variable magic
    // - Celle-ci indique que des valeurs sont presente dans la flash
    
    //Si pas la valeur magic retrouvee, on met des valeurs par d�fault
    //Sinon on garde les valeur 
    if (pParam->Magic != MAGIC){
        //Si on retrouve magic alors on garde les valeurs de la flash
        //Sinon "!=" alors je met les valeurs par d�faut
        pParam->Amplitude = 0;
        pParam->Forme = SignalSinus;
        pParam->Frequence = 20;
        pParam->Offset = 0;
        pParam->Magic = 0;   
    }
    
    
    GENSIG_UpdatePeriode(pParam); //Mise � jour des caract�ristique du signal
}
  

// Mise � jour de la periode d'�chantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    uint16_t ValTMR3 = 0;
    
    // Stop du timer 3
    PLIB_TMR_Stop(TMR_ID_3);
   
    // Salcul de la valeur de comptage du timer 3
    ValTMR3 = (UC_FREQUENCY / ((uint32_t)pParam->Frequence * MAX_ECH)) - 1;
    
    // �tablissement de la p�riode du timer 3
    PLIB_TMR_Period16BitSet(TMR_ID_3, ValTMR3);
    
    // Start du timer 3
    PLIB_TMR_Start(TMR_ID_3);
    
}

// Mise � jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{
    int i = 0;
    int32_t ValNew = 0;
    int16_t step = 0;
    float radian = 0;
    
    

    
    // Calcul du param�tre step (pour triangle et dent de scie)
    step = pParam->Amplitude / (MAX_ECH - 1);
    
    // Calcul valeurs du tableau en fonction de la forme choisi
    switch(pParam->Forme)
    {
        // Signal Sinus
        case SignalSinus :
            for(i = 0; i < MAX_ECH; i++)
            {
                // Conversion d'un �chantillon en radian (0 -> 0[RAD], 99 -> 2PI[RAD])
                radian = sin(map_float(i, MIN_ECH, MAX_ECH, 0, 2*3.14));

                ValNew = ((radian * (pParam->Amplitude) + pParam->Offset));

                TbEch[i] = ConvAOP(ValNew);
            }
            break;
        // Signal Triangle    
        case SignalTriangle :

            for(i = 0; i < MAX_ECH; i++)
            {
                // Calcul de la moiti� de �chantillons
                if(i <= 50)
                {
                  ValNew = (pParam->Offset - pParam->Amplitude) + step * i * 4;
                }
                // Calcul pour les reste des �chantillons 51->99
                else
                {
                  ValNew = (pParam->Offset + pParam->Amplitude) - step * (i - 50) * 4;
                }

                TbEch[i] = ConvAOP(ValNew);
            }
            break;
        // Signal dent de scie  
        case SignalDentDeScie :

            for(i = 0; i < MAX_ECH; i++)
            {
                ValNew = (pParam->Offset - pParam->Amplitude) + step * i * 2;
                TbEch[i] = ConvAOP(ValNew);
            }
            break;
        // Signal carre
        case SignalCarre :

            for(i = 0; i < MAX_ECH; i++)
            {
                // Calcul de la moiti� de �chantillons
                if(i < (MAX_ECH - 1)/2)
                {
                    ValNew = pParam->Amplitude + pParam->Offset;
                }
                // Calcul de la deuxi�me moiti� des �chantillons 50->99
                if(i > (MAX_ECH - 1)/2)
                {
                    ValNew = pParam->Offset - pParam->Amplitude;
                }

                TbEch[i] = ConvAOP(ValNew);
            }
            break;
        // Default state
        default:
            break;
    }
}

// Conversion de la valeur de tension calcul� pour convertir en valeur ADC
int32_t ConvAOP (int16_t Tension)
{
    int32_t Res = 0;
    // Si param?tre d'entr�e positif fixer la sortie du DAC entre 32768 et 0 
    if(Tension > 0)
    {
        Res = map(Tension, TENSION_MIN_POS, TENSION_MAX_POS,
                           DAC_MIDDLE     , DAC_MIN);
    }
    // Sinon fixer la sortie du DAC entre 65535 et 32768
    else
    {
        Res = map(Tension, TENSION_MIN_NEG, TENSION_MAX_NEG, 
                           DAC_MAX        , DAC_MIDDLE);
    }
    // Retour de la variable en fixant la plage minimale et maximale
    return TestMinMax(Res);   
}

// Test de la variable x en fonction des param�tre d'entr�e et sortie min et max en int32_t
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{	
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;				
}

// Test de la variable x en fonction des param�tre d'entr�e et sortie min et max en float
float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{	
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;				
}

// Test de la valeur minimale et maximale de notre ADC
// valeurs fix�e entre 0 et 65535 car l'ADC est un 16 bits 2^16 -1 = 65535
int32_t TestMinMax(int32_t InputVal)
{
    // Si valeur inf�rieure � 0 fixer � 0
    if(InputVal < 0)
    {
        InputVal = 0;
    }
    // Si valeur sup�rieur � 65535 fixer � 65535
    if(InputVal > 65535)
    {
        InputVal = 65535;
    }

    return InputVal;
}

// Execution du g�n�rateur
void  GENSIG_Execute(void)
{   
   static int i = 0;
   // Envoi d'un �chantillon sur le canal 0
   SPI_WriteToDac(0, TbEch[i]);
   i++;
   i = i % MAX_ECH;
}
