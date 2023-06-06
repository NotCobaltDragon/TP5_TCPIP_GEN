
// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms

#include "MenuGen.h"
#include "Mc32gestI2cSeeprom.h"
#include <stdint.h>                   
#include <stdbool.h>



S_ParamGen ParamDisplay; //= {SignalSinus, 100, 500, 0};

E_Menu_State menuState = Main_Menu;

// Initialisation du menu et des paramètres
void MENU_Initialize(S_ParamGen *pParam)
{
    lcd_bl_on();
	printf_lcd("  Signal Generator");           
    lcd_gotoxy(1,3);
    printf_lcd("   TP4 MINF 22-23");
    lcd_gotoxy(1,4);
    if(pParam -> Magic != MAGIC)
    {
        printf_lcd("    MEMORY ERROR    ");
    }
    else
    {
        printf_lcd("    Alex & Joao");
    }
    ParamDisplay.Forme = pParam -> Forme;
    ParamDisplay.Frequence = pParam -> Frequence;
    ParamDisplay.Amplitude = pParam -> Amplitude;
    ParamDisplay.Offset = pParam -> Offset;
}


// Execution du menu, appel cyclique depuis l'application
void MENU_Execute(S_ParamGen *pParam,bool USB)
{
    static uint8_t position = 0;
    static uint8_t selected = FALSE;   //The option is selected or not
    static uint8_t saveMenu = FALSE;
    char textSignal[4][11]={"Sine Wave  ", "Triangle   ", "Saw Tooth  ", "Square Wave"};
    int lcdPosition;
    static uint8_t saveMenuCounter = 0;
    static char cursor[4] = {BLANK, BLANK, BLANK, BLANK};
    uint8_t NAVIGATION;
    uint8_t SELECTED;
    //control si local ou non
   if(USB)
            {
                lcd_gotoxy(1,1);
                printf_lcd("#Shape %c ", SEPARATOR);
                for (lcdPosition = 0; lcdPosition < 11; lcdPosition++)
                {
                    printf_lcd("%c", textSignal[pParam->Forme][lcdPosition]);   
                }
                lcd_gotoxy(1,2);
                printf_lcd("#Frequ [Hz] %c %-4d", SEPARATOR, pParam->Frequence);
                lcd_gotoxy(1,3);
                printf_lcd("#Ampli [mV] %c %-5d", SEPARATOR, pParam->Amplitude);
                lcd_gotoxy(1,4);
                printf_lcd("#Offset [mV] %c %-5d", SEPARATOR, pParam->Offset);
                
                }else{
    if( USB)
    {
        NAVIGATION = 62;
        SELECTED = 188;   
    }
    else
    {
       NAVIGATION = 199;
       SELECTED = 251; 
    }
    //Manage backlight
    if(Pec12.NoActivity == TRUE)
    {
        lcd_bl_off();
    }
    else
    {
        lcd_bl_on();
    }

    switch(menuState)
    {
    case Main_Menu:
        {
            if(Pec12.Inc == TRUE && position < 3)
            {
                cursor[position + 1] = cursor[position];
                cursor[position]=BLANK;
                position++;
                Pec12ClearPlus();
            }
            else
            {
                Pec12ClearPlus();
            }

            if(Pec12.Dec == TRUE && position > 0)
            {
                cursor[position - 1] = cursor[position];
                cursor[position]=BLANK;
                position--;
                Pec12ClearMinus();
            }
            else
            {
                Pec12ClearMinus();
            }

            if(Pec12.OK == TRUE && selected == FALSE)
            {
                selected = TRUE;
                Pec12ClearOK();
            }

            if(S9.LNG == TRUE && selected == FALSE)
            {
                S9ClearLNG();
                saveMenu = TRUE;
            }

            if(saveMenu == TRUE)
            {
                menuState = Save_Menu;
            }

            if(selected == TRUE)
            {
                cursor[position] = SELECTED;
                menuState = position;

                ParamDisplay.Forme = pParam -> Forme;
                ParamDisplay.Frequence = pParam -> Frequence;
                ParamDisplay.Amplitude = pParam -> Amplitude;
                ParamDisplay.Offset = pParam -> Offset;

                //ParamDisplay = ParamGen;
            }
            else
            {
                cursor[position] = NAVIGATION;
            }

            break;
        }
    case Wave_Menu:
        {
            if(Pec12.Inc == TRUE)
            {
                ParamDisplay.Forme++;
                if(ParamDisplay.Forme > 3){ParamDisplay.Forme = 0;}
                Pec12ClearPlus();
            }
            else if(Pec12.Dec == TRUE)
            {
                if(ParamDisplay.Forme == 0){ParamDisplay.Forme = 3;}
                else{ParamDisplay.Forme--;}        
                Pec12ClearMinus();
            }
            if(Pec12.OK == TRUE)
            {
                pParam -> Forme = ParamDisplay.Forme;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearOK();
            }
            else if(Pec12.ESC == TRUE)
            {
                ParamDisplay.Forme = pParam -> Forme;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearESC();
            }
            break;
        }
    case Frequency_Menu:
        {
            if(Pec12.Inc == TRUE)
            {
                ParamDisplay.Frequence += FREQUENCY_INC;
                if(ParamDisplay.Frequence > MAX_FREQUENCY){ParamDisplay.Frequence = MIN_FREQUENCY;}
                Pec12ClearPlus();
            }
            else if(Pec12.Dec == TRUE)
            {
                ParamDisplay.Frequence -= FREQUENCY_INC;
                if(ParamDisplay.Frequence < MIN_FREQUENCY){ParamDisplay.Frequence = MAX_FREQUENCY;}
                Pec12ClearMinus();
            }
            if(Pec12.OK == TRUE)
            {
                pParam -> Frequence = ParamDisplay.Frequence;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearOK();
            }
            else if(Pec12.ESC == TRUE)
            {
                ParamDisplay.Frequence = pParam -> Frequence;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearESC();
            }
            break;
        }
    case Amplitude_Menu:
        {
            if(Pec12.Inc == TRUE)
            {
                ParamDisplay.Amplitude += AMPLITUDE_INC;
                if(ParamDisplay.Amplitude > MAX_AMPLITUDE){ParamDisplay.Amplitude = MIN_AMPLITUDE;}
                Pec12ClearPlus();
            }
            else if(Pec12.Dec == TRUE)
            {
                ParamDisplay.Amplitude -= AMPLITUDE_INC;
                if(ParamDisplay.Amplitude < MIN_AMPLITUDE){ParamDisplay.Amplitude = MAX_AMPLITUDE;}
                Pec12ClearMinus();
            }
            if(Pec12.OK == TRUE)
            {
                pParam -> Amplitude = ParamDisplay.Amplitude;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearOK();
            }
            else if(Pec12.ESC == TRUE)
            {
                ParamDisplay.Amplitude = pParam -> Amplitude;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearESC();
            }
            break;
        }
        
    case Offset_Menu:
        {
            if(Pec12.Inc == TRUE && ParamDisplay.Offset < MAX_OFFSET)
            {
                ParamDisplay.Offset += OFFSET_INC;
                Pec12ClearPlus();
            }
            else if(Pec12.Dec == TRUE && ParamDisplay.Offset > MIN_OFFSET)
            {
                ParamDisplay.Offset -= OFFSET_INC;
                Pec12ClearMinus();
            }
            if(Pec12.OK == TRUE)
            {
                pParam -> Offset = ParamDisplay.Offset;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearOK();
            }
            else if(Pec12.ESC == TRUE)
            {
                ParamDisplay.Offset = pParam -> Offset;
                menuState = Main_Menu;
                selected = FALSE;
                Pec12ClearESC();
            }
            break;  
        }
    case Save_Menu:
        {
            if(S9.LNG == TRUE)
            {
                pParam -> Magic = MAGIC;
                S_ParamGen* myParam = pParam;
                I2C_WriteSEEPROM(&myParam,sizeof(*pParam));
                //NVM_WriteBlock((uint32_t*)pParam,sizeof(*pParam));
                saveMenu = SAVED;
                S9ClearLNG();
            }
            else if((Pec12.Inc||Pec12.Dec||Pec12.OK||Pec12.ESC) == 1)
            {
                saveMenu = CANCELED;
            }

            if((saveMenu == SAVED)||(saveMenu == CANCELED))
            {
                saveMenuCounter++;
                if(saveMenuCounter>=CONFIRM_TIME)
                {
                    saveMenuCounter = 0;
                    saveMenu = FALSE;
                    menuState = Main_Menu;
                    lcd_ClearLine(1);
                    lcd_ClearLine(2);
                    lcd_ClearLine(3);
                    lcd_ClearLine(4);
                    Pec12ClearInactivity();
                }
            }
            break;
        }
    default:
    break;        
    }

    //Display update
    switch(saveMenu)
    {
    case TRUE:
        {
            lcd_bl_on();
            lcd_ClearLine(1);
            lcd_gotoxy(1,2);
            printf_lcd(" Save Parameters  ? ");
            lcd_gotoxy(1,3);
            printf_lcd("    (long press)    ");
            lcd_ClearLine(4);
            break;
        }
    case SAVED:
        {
            lcd_bl_on();
            lcd_ClearLine(1);
            lcd_gotoxy(1,2);
            printf_lcd("  Parameters Saved  ");
            lcd_ClearLine(3);
            lcd_ClearLine(4);
            break;
        }
    case CANCELED:
        {
            lcd_bl_on();
            lcd_ClearLine(1);
            lcd_gotoxy(1,2);
            printf_lcd(" Operation Canceled ");
            lcd_ClearLine(3);
            lcd_ClearLine(4);
            break;
        }
    default:
        {   
            
                lcd_gotoxy(1,1);
                printf_lcd("%cShape %c ", cursor[0], SEPARATOR);
                for (lcdPosition = 0; lcdPosition < 11; lcdPosition++)
                {
                    printf_lcd("%c", textSignal[ParamDisplay.Forme][lcdPosition]);   
                }
                lcd_gotoxy(1,2);
                printf_lcd("%cFrequ [Hz] %c %-4d", cursor[1], SEPARATOR, ParamDisplay.Frequence);
                lcd_gotoxy(1,3);
                printf_lcd("%cAmpli [mV] %c %-5d", cursor[2], SEPARATOR, ParamDisplay.Amplitude);
                lcd_gotoxy(1,4);
                printf_lcd("%cOffset [mV] %c %-5d", cursor[3], SEPARATOR, ParamDisplay.Offset);
            
            break;
        }
    }
}
}
