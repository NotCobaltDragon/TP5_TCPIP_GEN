// Mc32Gest_SerComm.C
// fonction d'émission et de réception des message
// transmis en USB CDC
// Canevas TP4 SLO2 2015-2015


#include "app.h"
#include "Mc32gest_SerComm.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


uint8_t charOfValue[NBR_VALUE] = { 'S', 'F', 'A', 'O', 'W' };
uint8_t valueOfForme[NBR_FORME_SIGNAL] = {'S', 'T', 'D', 'C'};
// Fonction de reception  d'un  message
// Met à jour les paramètres du generateur a partir du message recu
// Format du message
//  !S=TF=2000A=10000O=+5000W=0#
//  !S=PF=2000A=10000O=-5000W=1#

bool GetDataIsOK(int8_t* chaine) {
	int i;
    bool start = false;
    bool end = false;
    
	for (i = 0; i < SIZE_MAX_OF_DATA; i++) {
		if (*chaine == (int8_t)END_DATA) {
			end = true;
		}
        if (*chaine == (int8_t)START_DATA){
            start = true;
        }
		chaine++;
	}
    
    if((start==true) && (end==true)){
        return true;
    }
    else{
       return false; 
    }
}


E_FormesSignal getValueForme(uint8_t charactere){
	int i;
	for (i = 0; i < NBR_FORME_SIGNAL; i++) {
		if (charactere == valueOfForme[i]) {
			return i;
		}
	}
	return 0;
}


int16_t testValue(int16_t value, char parameterToTest) {
	switch (parameterToTest)
	{
	case SIGNAL_VALUE:
		if (value > MAXVALEUR_FORME)
			value = MAXVALEUR_FORME;
		else if (value < MINVALEUR_FORME)
			value = MINVALEUR_FORME;
		break;

	case FREQ_VALUE:
		if (value > MAXVALEUR_FREQUENCE_HZ)
			value = MAXVALEUR_FREQUENCE_HZ;
		else if (value < MINVALEUR_FREQUENCE_HZ)
			value = MINVALEUR_FREQUENCE_HZ;
		break;
	case AMPL_VALUE:
		if (value > MAXVALEUR_AMPLITUDE_mV)
			value = MAXVALEUR_AMPLITUDE_mV;
		else if (value < MINVALEUR_AMPLITUDE_mV)
			value = MINVALEUR_AMPLITUDE_mV;
		break;
	case OFFS_VALUE:
		if (value > MAXVALEUR_OFFSET_mV)
			value = MAXVALEUR_OFFSET_mV;
		else if (value < MINVALEUR_OFFSET_mV)
			value = MINVALEUR_OFFSET_mV;
		break;
	case W_VALUE:
		if (value > MAXVALEUR_W)
			value = MAXVALEUR_W;
		else if (value < MINVALEUR_W)
			value = MINVALEUR_W;
		break;
	}
	return value;
}


int16_t getValueFreqAmplOffsW(char* chaine, char valueParameter) {
	int i;
	char value[SIZE_MAX_VALUE] = {0, 0, 0, 0, 0, 0};

	for (i = 0; i < SIZE_MAX_VALUE; i++) {
		if (((*chaine >= START_LETTRE_MAJ_ASCII) && (*chaine <= END_LETTRE_MAJ_ASCII)) || (*chaine == END_DATA)) {
			break;
		}
		else {
			value[i] = *chaine;
		}
		chaine++;
	}
	return testValue(atoi(value), valueParameter);
}

bool GetMessage(int8_t *TCPReadBuffer, S_ParamGen *pParam, bool *SaveTodo)
{
	uint8_t *positionOfStartValue = 0;
	uint8_t i, j;
	uint8_t idxValue;
	bool dataIsOk = false;

	if (GetDataIsOK(TCPReadBuffer)) {
		dataIsOk = true;
		//Faire autant de fois qu'il y a de valeur à recup (NBR_VALUE)
		for (i = 0; i < NBR_VALUE; i++) {
			//Recup la position du caractère selectionner sur charOfValue
			positionOfStartValue = (uint8_t*)strchr((char*)TCPReadBuffer, charOfValue[i]);
			//Recuperer le caractère lié à la position recup (positionOfStartValue)
			idxValue = charOfValue[i];

			//Si un caractère à été liée é la position
			if (positionOfStartValue != 0) {
				//Décale la position (éviter le "X=") afin de traiter la valeur souhaitée
				positionOfStartValue += (sizeof(CHAR_AFTER_VALUE) - 1); //-1 pour enlever "\0"
				switch (idxValue)
				{
				case SIGNAL_VALUE:
					pParam->Forme = getValueForme((uint8_t)*(positionOfStartValue));
					break;

				case FREQ_VALUE:
				
					pParam->Frequence = getValueFreqAmplOffsW((char*)positionOfStartValue, (char)idxValue);
					break;

				case AMPL_VALUE:
					pParam->Amplitude = getValueFreqAmplOffsW((char*)positionOfStartValue, (char)idxValue);
					break;

				case OFFS_VALUE:
					pParam->Offset = getValueFreqAmplOffsW((char*)positionOfStartValue, (char)idxValue);
					break;
				case W_VALUE:
					if (getValueFreqAmplOffsW((char*)positionOfStartValue, (char)idxValue))
						*SaveTodo = true;
					else
						*SaveTodo = false;
					break;
				}
			}
		}        
        //vide le buffer
        for(j = 0; j < MAX_LONGEUR_SEND; j++)
        {
            TCPReadBuffer[j] = 0;
        }
	}
	return dataIsOk;
} // GetMessage


// Fonction d'envoi d'un  message
// Rempli le tampon d'émission pour USB en fonction des paramètres du générateur
// Format du message
// !S=TF=2000A=10000O=+5000D=25WP=0#
// !S=TF=2000A=10000O=+5000W=1#    // ack sauvegarde

void SendMessage(S_ParamGen *pParam, bool Saved)
{
    int8_t endString = 0;
    int8_t TCPSendBuffer[32];
    char Forme = 'E';
    
    switch (pParam->Forme)
    {
        //Cas triangle
        case SignalTriangle:
        {
            //Assignation de "Triangle" au champs forme de la structure
            Forme = 'T';
            break;
        }
        case SignalSinus:
        {
            //Assignation de "Sinus" au champs forme de la structure
            Forme = 'S';
            break;
        }
        case SignalCarre:
        {
            //Assignation de "Carre" au champs forme de la structure
            Forme = 'C';
            break;
        }
        case SignalDentDeScie:
        {
           //Assignation de "DentDeScie" au champs forme de la structure
            Forme = 'D';
            break; 
        }
        default : 
            break;
    }
    if (Saved) 
    {
        endString = 1;
    }
    else
    {
        endString = 0;
    }
    //mise en forme du message à envoyer
    if(pParam->Offset>=0)
    	sprintf((char*)TCPSendBuffer, "!S=%cF=%dA=%dO=+%dWP=%d#", Forme, pParam->Frequence, pParam->Amplitude, pParam->Offset,endString);
    else
    sprintf((char*)TCPSendBuffer, "!S=%cF=%dA=%dO=%dWP=%d#", Forme, pParam->Frequence, pParam->Amplitude, pParam->Offset,endString);
    //envoyer la réponse via TCP-IP
    TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)TCPSendBuffer, 28);
    //TCPIP_TCP_ArrayPut(appData.socket, (uint8_t*)TCPSendBuffer, (sizeof(TCPSendBuffer)-3)); //tentative enlever NULL -> pas ok
} // SendMessage
