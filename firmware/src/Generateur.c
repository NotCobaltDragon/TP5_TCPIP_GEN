// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "Mc32gestI2cSeeprom.h"
#include "math.h"
#include "driver/tmr/drv_tmr_static.h"

// Variables globales
uint16_t SignalValues[MAX_ECH] = {0};

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    //Recup val mémoire
    S_ParamGen* myParam = pParam;
    I2C_ReadSEEPROM(&myParam,sizeof(*pParam));
    //NVM_ReadBlock((uint32_t*)pParam, sizeof(*pParam));
    if(pParam->Magic != MAGIC)
    {
        // Fréquence par défaut = 20
        pParam -> Frequence = INIT_FREQ;
        // Forme du signal par défaut = Sinus
        pParam -> Forme = INIT_FORM;
        // Amplitude par défaut = 0;
        pParam -> Amplitude = INIT_AMPLITUDE;
        // Offset par défaut = 0
        pParam -> Offset = INIT_OFFSET;
    }
    // Demarrage du generateur de fonction
    GENSIG_UpdatePeriode(pParam);
    GENSIG_UpdateSignal(pParam);
}

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    // Variable locale
    uint16_t Val_Periode = 0;
    
    Val_Periode = (float)F_SYS/(float)(pParam -> Frequence * MAX_ECH * PRESCALER) - 1;
    PLIB_TMR_Period16BitSet(TMR_ID_3, Val_Periode);
}

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{
    // Variables locales
    uint8_t i = 0;
    
    int16_t DAC_Amplitude = ((float)pParam -> Amplitude * (float)DEFAULT_OFFSET) / 10000.5;
    int16_t DAC_Offset = ((float)pParam -> Offset * (float)DEFAULT_OFFSET) / 10000.5;
    float pointValue = ((float)DEFAULT_OFFSET - (float)DAC_Amplitude);
    int32_t CalculOffset = 0;
    
    for(i=0; i<=MAX_ECH; i++)
    {
        switch(pParam -> Forme)
        {
            case SignalSinus:
                SignalValues[i] = ((float)DAC_Amplitude * sin((float)2 * M_PI * i/(float)MAX_ECH) + 0.5) + DEFAULT_OFFSET;
                break;
            case SignalTriangle:
                SignalValues[i] = pointValue + 0.5;
                if(i < (MAX_ECH / 2))
                {
                    pointValue = pointValue + ((float)DAC_Amplitude * (float)2 + (float)1) / (float)(MAX_ECH/2.0);
                }
                else
                {
                    pointValue = pointValue - ((float)DAC_Amplitude * (float)2 + (float)1) / (float)(MAX_ECH/2.0);
                }
                break;
            case SignalDentDeScie:
                SignalValues[i] = pointValue + 0.5;
                pointValue = pointValue + ((float)DAC_Amplitude * (float)2 + (float)1) / (float)(MAX_ECH - 1);
                break;
            case SignalCarre:
                if(i < (MAX_ECH / 2))
                {
                    SignalValues[i] = DAC_Amplitude + 1 + DEFAULT_OFFSET;;
                }
                else
                {
                    SignalValues[i] = -DAC_Amplitude + DEFAULT_OFFSET;
                }
                break;
            default :
                break;
        }
    }
    for(i=0; i<=MAX_ECH; i++)
    {
        CalculOffset = (int32_t)SignalValues[i] - (int32_t)DAC_Offset;
        if(CalculOffset > MAX_VAL)
        {
            CalculOffset = MAX_VAL;
        }
        else if(CalculOffset < MIN_VAL)
        {
            CalculOffset = MIN_VAL;
        }
        SignalValues[i] = (uint16_t)CalculOffset;
    }
    // Test pour debug
    i = 10;
}


// Execution du générateur
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC à modifier
void  GENSIG_Execute(void)
{
    LED7_W = 1;
    static uint16_t EchNb = 0;
    //const uint16_t Step = 65535 / MAX_ECH;

    //   SPI_WriteToDac(0, Step * EchNb );      // sur canal 0
    SPI_WriteToDac(0, SignalValues[EchNb]);      // sur canal 0
    EchNb++;
    EchNb = EchNb % MAX_ECH;
    LED7_W = 0;
}
