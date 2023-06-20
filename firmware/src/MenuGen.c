/******************************************************************************/
// Project		: TP3 Menu Gen
// Author 		: Ricardo Crespo & Giuseppe Stabile
// Date 		: 07.02.2022
// Descrition   : Fichier du menu du générateur
/******************************************************************************/

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms
//Développeurs: Giuseppe Stabile et Rodrigo Lopes



#include <stdint.h>                   
#include <stdbool.h>
#include "MenuGen.h"
#include "Mc32DriverLcd.h"
#include "Mc32NVMUtil.h"
#include "GesPec12.h"
#include <string.h>
#include "appgen.h"
#include "Mc32gestI2cSeeprom.h"
#include "app.h"
#include "Generateur.h"

bool save;
S_SwitchDescriptor DescrOK;
APP_DATA appData; 

const char *selecting = "*";
const char *sharp = "#";

const char *space = " ";
const char *selected = "?";

const char *forme = " Shape ";
const char *frequence = " Frequ [Hz] ";
const char *amplitude = " Ampli [mV] ";
const char *offset = " Offset [mV] ";

extern uint8_t tbIP[4];
bool TCPCon_Menu = false;

// Initialisation du menu et des paramètres
void MENU_Initialize(S_ParamGen *pParam)
{
    lcd_gotoxy(1,1); //Pointer sur la ligne 1 colone 1 du LCD
    printf_lcd("TP5 TCP-IP Gen 22-23");
    lcd_gotoxy(1,2); //Pointer sur la ligne 2 colone 1 du LCD
    printf_lcd("Alexandre Steffen   ");
    lcd_gotoxy(1,3); //Pointer sur la ligne 3 colone 1 du LCD
    printf_lcd("Jonathan Chafla     "); //Affichage LCD
    
    DebounceInit(&DescrOK);
}

//Affiche du menu save
void MENU_Save(E_save state){
    
    lcd_gotoxy(1,2);
    printf_lcd(" Save Parameters ? ");
    lcd_gotoxy(1,3);
    switch(state){
        case e_Selection:
            printf_lcd("    (long press)    ");                
            break;
        case e_OK:
            lcd_ClearLine(2);
            lcd_ClearLine(3);
            printf_lcd("  Parameters Saved  ");               
            break;
        case e_NOK:
            lcd_ClearLine(2);
            lcd_ClearLine(3);
            printf_lcd(" Operation Canceled ");            
            break;
    }
}
 // Affiche du menu
void MENU_Selection(void){
    lcd_gotoxy(1,1);
    printf_lcd(forme);
    printf_lcd("%c", SEPARATOR);
    lcd_gotoxy(1,2);
    printf_lcd(frequence);
    printf_lcd("%c", SEPARATOR);
    lcd_gotoxy(1,3);
    printf_lcd(amplitude);
    printf_lcd("%c", SEPARATOR);
    lcd_gotoxy(1,4);
    printf_lcd(offset);
    printf_lcd("%c", SEPARATOR);
}


// Execution du menu, appel cyclique depuis l'application
void MENU_Execute(S_ParamGen *pParam, bool local)
{
    //Indique la ligne du menu sur la quelle je pointe
    static uint8_t selectLigne = 1;
    
    //Indique si nous sommes entrain de naviger sur le menue
    //flag_selection = false
    //Inidique si nous sommes entrain de changer une valeurs du menu
    //flag_selection = true
    static bool flag_selection = false;
    
    //Sers a stocker les ancienne valeurs de pParam lorsque nous
    //changons une valeurs du menu
    static S_ParamGen saveParam;
    
    //Sers pour laffichage de la forme de signal selectioner
    //La valeur de forme est une enumeration
    //sinus = 0, triangle = 1, ...
    //Si forme = sinus = 0 et je pointe sur la case 0 du tableau
    //alors la chaine suivante s'affiche : " Sinus"
    const char MenuFormes [4][21] = {" Sine Wave", " Triangle  ", " Saw Tooth", " Square Wave"};
    
    static int16_t old_freq = 0;
    
    static bool flag_save_flash = false;
    static bool flag_save_count = false;
    static bool flag_save_display = false;  
    static int countSave = 0;
    
    
    
    if ((local == false) && (!flag_save_display)) {
        lcd_bl_on(); //Allume le backlight
        
        lcd_gotoxy(1,LIGNE_FORME);
        printf_lcd(sharp);
        lcd_gotoxy(1,LIGNE_FREQUENCE);
        printf_lcd(sharp);
        lcd_gotoxy(1,LIGNE_AMPLITUDE);
        printf_lcd(sharp);
        lcd_gotoxy(1,LIGNE_OFFSET);
        printf_lcd(sharp);          
    }
    else{
        lcd_gotoxy(1,LIGNE_FORME);
        printf_lcd(space);
        lcd_gotoxy(1,LIGNE_FREQUENCE);
        printf_lcd(space);
        lcd_gotoxy(1,LIGNE_AMPLITUDE);
        printf_lcd(space);
        lcd_gotoxy(1,LIGNE_OFFSET);
        printf_lcd(space);        
    }
    
    
    //S'il y a un appui sur le bouton S9
    if ((DebounceIsPressed(&DescrOK))&&(local == true)){
        lcd_bl_on(); //Allume le backlight
        DebounceClearPressed(&DescrOK);
        //Si c'est le premier appuie
        if (flag_save_flash == false){
            flag_save_flash = true; //Inidque que nous somme dans le menu save
            ClearAffichage(); //Clear l'affichage
            MENU_Save(e_Selection); //Affichage du menu save                 
        }
        else{
        //Sinon, c'est la deuxième fois qu'on appuie
        //Sauvegarde autorisé ou non
            flag_save_count = true; // on permet l'incrémentation du temps d'appuie de S9
        }     
    }
    
    //Si nous avons l'aurosiation d'incrémenter l'appui
    if ((DebounceGetInput(&DescrOK) && (flag_save_count == true))||(((Pec12IsOK())||(Pec12IsMinus())||(Pec12IsESC())||(Pec12IsPlus())) && (flag_save_flash == true)) || ((flag_save_display == false)&&(local == false)&&(GetSaveTCP()))){          
        if (local == true){
            if (countSave >= 50){
                MENU_Save(e_OK);
                pParam->Magic = MAGIC;
                I2C_WriteSEEPROM((void*)pParam, ADRESSE_STRUCT_EEPROM, sizeof(*pParam));
            }
            else{
                MENU_Save(e_NOK);                
            }            
        }
        else{
            ClearAffichage(); //Clear l'affichage
            MENU_Save(e_OK);
            pParam->Magic = MAGIC;
            I2C_WriteSEEPROM((void*)pParam, ADRESSE_STRUCT_EEPROM, sizeof(*pParam));            
        }
        
        Pec12ClearOK();
        Pec12ClearESC();
        Pec12ClearMinus();
        Pec12ClearPlus();                      
        countSave = 0;
        flag_save_count = false;
        flag_save_display = true;
    }
    else if ((flag_save_display == false) && (flag_save_count == true)){
        countSave ++; //J'incrimente le temps d'appuie  
    } 
    
    if (flag_save_display == true){
        if (countSave < 200){
            countSave ++;
        }
        else{
            flag_save_display = false;
            flag_save_flash = false;
            countSave = 0;
            ClearAffichage();
            MENU_Selection();
            Pec12ClearInactivity();
        }
    }

    //Test de l'inactivitee
    if ((Pec12NoActivity())&&(flag_save_flash == false)&&(local == true)){
        //Si non actif (5s) alors:
        lcd_bl_off(); //Eteint le backlight
    }
    else if ((flag_save_flash == false)&&(local == true)){      
        //Sinon, donc actif :
        lcd_bl_on(); //Allume le backlight
        
        //_________________Si appuie sur OK________________________
        if (Pec12IsOK()){
            if (flag_selection == false){
                //Si nous etions dans le menu et donc on veut maintenant
                //changer une valeurs --> On save les anciennes valeurs                
                saveParam.Amplitude = pParam->Amplitude;
                saveParam.Forme = pParam->Forme;
                saveParam.Frequence = pParam->Frequence;
                saveParam.Magic = pParam->Magic;
                saveParam.Offset = pParam->Offset;
                flag_selection = true;
            }
            else{
                //Lever du flag --> Indiquant que nous navigons dans le menu
                flag_selection = false;
            }
            //Clear du bouton OK (celui ne se clear pas seul)
            Pec12ClearOK();
        }
        //___________________________________________________________
        //_________________Si appuie sur ESC_________________________
        if (Pec12IsESC()){
            if (flag_selection == true){
                //Si nous etions entrain de changer les valeurs mais que 
                //nous appuyons sur ESC --> On remet les valeurs precedemment save
                pParam->Amplitude = saveParam.Amplitude;
                pParam->Forme = saveParam.Forme;
                pParam->Frequence = saveParam.Frequence;
                pParam->Magic = saveParam.Magic;
                pParam->Offset = saveParam.Offset;
                //Lever le flag --> Indiquant que nous navigons dans le menu
                flag_selection = false;
                MENU_ClearParameters(selectLigne);//Clear valeurs actuel
            }
            //Clear du bouton ESC (celui ne se clear pas seul)
            Pec12ClearESC();
        }
        //___________________________________________________________
        //___________________Si appuie sur +_________________________
        if (Pec12IsPlus()){
            if ((flag_selection == false)){
                //si nous somme en navigation et l'incrementation est possible
                lcd_gotoxy(1,selectLigne); //Pointeur sur la ligne actuel
                printf_lcd(space); //Clear le caractere * avec un espace
                selectLigne ++; //Incrementation de la ligne
                if(selectLigne > 4)
                {
                    selectLigne = 1;
                }
            }
            else if (flag_selection == true){
                //Si nous changons une valeurs
                MENU_ClearParameters(selectLigne);//Clear valeurs actuel
                //selon la ligne
                switch (selectLigne){
                    case LIGNE_FORME: 
                        //Test de la limite
                        if (pParam->Forme != MAXVALEUR_FORME){
                            pParam->Forme ++; //Incrementation de la valeur
                        }
                        break;
                    case LIGNE_FREQUENCE:
                        //Test de la limite
                        if (pParam->Frequence < MAXVALEUR_FREQUENCE_HZ){
                            pParam->Frequence += PAS_FREQUENCE_HZ;
                        }
                        else{
                            //Rebouclement
                            pParam->Frequence = MINVALEUR_FREQUENCE_HZ;
                        }
                        break;
                    case LIGNE_AMPLITUDE:
                        //Test de la limite
                        if (pParam->Amplitude < MAXVALEUR_AMPLITUDE_mV){
                            pParam->Amplitude += PAS_AMPLITUDE_mV;
                        }
                        else{
                            //Rebouclement
                            pParam->Amplitude = MINVALEUR_AMPLITUDE_mV;
                        }
                        break;
                    case LIGNE_OFFSET:
                        //Test de la limite
                        if (pParam->Offset < MAXVALEUR_OFFSET_mV){
                            pParam->Offset += PAS_OFFSET_mV;
                        }
                        //SINON - RIEN --> Butee
                        break;
                }
            }
            //Clear du bouton Plus (celui ne se clear pas seul)
            Pec12ClearPlus();
        }
        //___________________________________________________________
        //___________________Si appuie sur -_________________________        
        if(Pec12IsMinus()){
            if ((flag_selection == false)){
               //si nous somme en navigation et d'incrementation est possible
               lcd_gotoxy(1,selectLigne); //Pointeur sur la ligne actuel
               printf_lcd(space);  //Clear le caractere * avec un espace              
               selectLigne --;    //Decrementation de la ligne   
               if(selectLigne < 1)
               {
                   selectLigne = 4;
               }
            }
            else if (flag_selection == true){
                //Si nous changons une valeurs
                MENU_ClearParameters(selectLigne);//Clear valeurs actuel
                //selon la ligne
                switch (selectLigne){
                    case LIGNE_FORME:
                        //Test de la limite
                        if (pParam->Forme != MINVALEUR_FORME){
                            pParam->Forme --;
                        }
                        break;
                    case LIGNE_FREQUENCE:
                        //Test de la limite
                        if (pParam->Frequence > MINVALEUR_FREQUENCE_HZ){
                            pParam->Frequence -= PAS_FREQUENCE_HZ;
                        }
                        else{
                            //Rebouclement
                            pParam->Frequence = MAXVALEUR_FREQUENCE_HZ;
                        }                        
                        break;
                    case LIGNE_AMPLITUDE:
                        //Test de la limite
                        if (pParam->Amplitude > MINVALEUR_AMPLITUDE_mV){
                            pParam->Amplitude -= PAS_AMPLITUDE_mV;
                        }
                        else{
                            //Rebouclement
                            pParam->Amplitude = MAXVALEUR_AMPLITUDE_mV;
                        }                        
                        break;
                    case LIGNE_OFFSET:
                        //Test de la limite
                        if (pParam->Offset > MINVALEUR_OFFSET_mV){
                            pParam->Offset -= PAS_OFFSET_mV;
                        }
                        //SINON - RIEN --> Butee                        
                        break;
                }                
            }
            //Clear du bouton Moins (celui ne se clear pas seul)
            Pec12ClearMinus();
        }
        //___________________________________________________________
        //___________________Affichage * OU ?________________________ 
        lcd_gotoxy(1,selectLigne); //Pointeur la ligne sur laquelle nous voulons afficher
        if (flag_selection == false){
            //Si nous somme dans le menu
            printf_lcd(selecting); //Afficher --> *    
        }
        else{
            //sinon, donc en changement de valeur
            printf_lcd(selected); //Afficher --> ?    
        }
        //___________________________________________________________     
    }
    
    //__________Affichage des caracteristique signal_____________
    if ((flag_save_flash == false) &&(flag_save_count == false) && (flag_save_display == false)){
        //Pour le pointage sur la ligne en question
        //Pour le pointage de la colone, c'est la taille de la chaine de caractere
        //plus 1 car la premiere valeur de la chaine est 0
        //ex " Forme =" = 8 bytes = 0 ä 7 bytes, la colonne commencons a 0 on ajoute +1
        lcd_gotoxy((strlen(forme) +1),LIGNE_FORME); //+1 apres des test
        printf_lcd(MenuFormes[pParam->Forme]);
        lcd_gotoxy((strlen(frequence) +1),LIGNE_FREQUENCE); //+1 apres des test
        printf_lcd("%4d", pParam->Frequence);
        lcd_gotoxy((strlen(amplitude) +1),LIGNE_AMPLITUDE); //+1 apres des test
        printf_lcd("%5d", pParam->Amplitude);
        lcd_gotoxy((strlen(offset) +1),LIGNE_OFFSET); //+1 apres des test
        printf_lcd("%5d", pParam->Offset);
        if(old_freq != pParam->Frequence){
            // Actualisation de la période
            GENSIG_UpdatePeriode(pParam);               
        }
        old_freq = pParam->Frequence;
    }
    //___________________________________________________________  
    
}


//Sers a effacer les valeurs en fonction de la ligne
//sans effacer le text du menu
void MENU_ClearParameters(uint8_t ligneToClear){
    int i;
    
    switch (ligneToClear){
        case LIGNE_AMPLITUDE:
            for (i = (20 - strlen(amplitude)); i > 1; i--){
               lcd_gotoxy(strlen(amplitude) + i,ligneToClear);
               printf_lcd(space);
            }               
            break;
        case LIGNE_FORME:
            for (i = (20 - strlen(forme)); i > 1; i--){
               lcd_gotoxy(strlen(forme) + i,ligneToClear);
               printf_lcd(space);
            }              
            break;
        case LIGNE_FREQUENCE:
            for (i = (20 - strlen(frequence)); i > 1; i--){
               lcd_gotoxy(strlen(frequence) + i,ligneToClear);
               printf_lcd(space);
            }              
            break;
        case LIGNE_OFFSET:
            for (i = (20 - strlen(offset)); i > 1; i--){
               lcd_gotoxy(strlen(offset) + i,ligneToClear);
               printf_lcd(space);
            }              
            break;
    }   
}
void MENU_AffichageIP(void)
{
    static bool Clear = true;
    //celar de l'affichage 1 seule fois
    if(Clear == true)
    {
        ClearAffichage();
        Clear = false;
    }
    lcd_bl_on();
    //affichage adresse IP
    lcd_gotoxy(1,2);
    printf_lcd("IP Address:");
    lcd_gotoxy(1,3);
    printf_lcd("%d.%d.%d.%d", tbIP[0], tbIP[1], tbIP[2], tbIP[3]);
}