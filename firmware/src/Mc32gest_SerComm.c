// Mc32Gest_SerComm.C
// fonction d'émission et de réception des message
// transmis en USB CDC
// Canevas TP4 SLO2 2015-2015

#include "app.h"
#include "app_gen.h"
#include "Mc32gest_SerComm.h"
#include "Mc32gestI2cSeeprom.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


APP_DATA appData;
S_ParamGen RemoteParamGen;

/***********************************************************************
*                                                                      *
* Fonction de reception  d'un  message                                 *
* Met à jour les paramètres du generateur a partir du message recu     *
* Format du message                                                    *
*  !S=TF=2000A=10000O=+5000D=100W=0#                                   *
*  !S=PF=2000A=10000O=-5000D=100W=1#                                   *
*                                                                      *
***********************************************************************/
void GetMessage()
{
    static uint8_t etatCar,i;
    static uint32_t ValRecup,compteur;
    static bool Signe,NewCharRecived;
    static char car;
    car = appData.readBuffer[i];
    i++;
    // 33 = !
    if(car == 33) NewCharRecived = true;
 
    if(NewCharRecived == true)
    {
        switch (etatCar)
        {
            case 0://detection info
                if(car == 83)etatCar = 1;//83 = S
            break;
            case 1://forme signal
                if(car == 70)// 70 = F
                {
                    RemoteParamGen.Forme = ValRecup;
                    etatCar = 2;
                }else
                {
                    if(car == 84)ValRecup = 1;// 84 = T
                    if(car == 83)ValRecup = 0;// 83 = S
                    if(car == 67)ValRecup = 3;// 67 = C
                    if(car == 68)ValRecup = 2;// 68 = D
                }
            break;
            case 2://Frequence
                if(car == 61)etatCar = 6;// 61 = '='
            break;
            case 3://Amplitude
                if(car == 61)etatCar = 6;// 61 = '='
            break;
            case 4://Offset
                if(car == 61)etatCar = 6;// 61 = '='
            break;
            case 5://sauvgarde              
                if(car == 49)
                {//save si un 1 et envoié
                    RemoteParamGen.Magic = MAGIC;
                    S_ParamGen* MyRemoteParamGen = &RemoteParamGen ;
                    I2C_WriteSEEPROM(&MyRemoteParamGen,sizeof(S_ParamGen));
                }
                // no save
                if(car == 48)
                {
                
                }
                
                if( car == 35 )
                {
                    //finish
                    etatCar = 0;
                    //i=0;
                    NewCharRecived = false;
                }
            break;
            case 6://si c'est un chifrre en met la valeur dans ValRecup
                if(car >= 48 && car <= 57 )
                {
                    compteur++;
                    if(compteur == 1)ValRecup = car-48;
                    if(compteur >= 2)ValRecup = ((ValRecup*10) + car)-48;
                }
                else
                {   //detection du signe
                    if(car==43)Signe = true;//+
                    if(car==45)Signe = false;//-
                    compteur = 0;
                    if(car == 65)// 65 = A
                    {
                        RemoteParamGen.Frequence = ValRecup;
                        etatCar = 3;
                    }
                    if(car == 79)// 79 = O
                    {
                        RemoteParamGen.Amplitude = ValRecup;
                        etatCar = 4;
                    }
                    if(car == 87 && Signe == true)// 87 = W
                    {
                        RemoteParamGen.Offset = ValRecup;
                        etatCar = 5;
                    }
                    if(car == 87 && Signe == false)// 87 = W
                    {
                        RemoteParamGen.Offset = -ValRecup;
                        etatCar = 5;
                    }    
                }
                break;
            default:
            break;
        }
    }
}
/*********************************************************************************
*                                                                                *
*  Fonction d'envoi d'un  message                                                *
*  Rempli le tampon d'émission pour USB en fonction des paramètres du générateur *
*  Format du message                                                             *
*  !S=TF=2000A=10000O=+5000D=25WP=0#                                             *
*  !S=TF=2000A=10000O=+5000D=25WP=1#    // ack sauvegarde                        *
*                                                                                *
*********************************************************************************/
void SendMessage()
{
    static char Envoie;
    static uint8_t i= 0;
    uint8_t Control = 0;
    if(appData.readBuffer[0] == 33 )//recherche "!"
    {
        Envoie = appData.readBuffer[i];
        i++;
        if(Envoie == 87)//recherche le W
        {
            Envoie = appData.readBuffer[i+1];//recup 0/1
            //control quand a recup la valeur save
            if(Envoie != 48)Control++;
            if(Envoie != 49)Control++;
            //Si en trouve pas un 1 ou un 0 en cherche a nouveaux
            if(Control != 1)Envoie = appData.readBuffer[i+2];
            appData.readBuffer[i] = 80;// P
            appData.readBuffer[i+1] = 61;// =
            appData.readBuffer[i+2] = Envoie;// 0/1
            appData.readBuffer[i+3] = 35;//#
            i=0;
        }
        
    }else
    {
        appData.readBuffer[i]=63;//si faux envoi "?"
    }
}

