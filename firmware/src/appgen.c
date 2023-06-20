/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
	Microchip Technology Inc.
  
  File Name:
	app_gen.c

  Summary:
	This file contains the source code for the MPLAB Harmony application.

  Description:
	This file contains the source code for the MPLAB Harmony application.  It 
	implements the logic of the application's state machine and it may call 
	API routines of other MPLAB Harmony modules in the system, such as drivers,
	system services, and middleware.  However, it does not call any of the
	system interfaces (such as the "Initialize" and "Tasks" functions) of any of
	the modules in the system or make any assumptions about when those functions
	are called.  That is the responsibility of the configuration-specific system
	files.
 *******************************************************************************/

#include "system_config/pic32mx_eth_sk2/framework/driver/tmr/drv_tmr_static.h"


// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "appgen.h"
#include "app.h"
#include "Mc32DriverLcd.h"
#include "Mc32gestSpiDac.h"
#include "MenuGen.h"
#include "GesPec12.h"
#include "Generateur.h"
#include "Mc32NVMUtil.h"
#include "DefMenuGen.h"
#include "Mc32_I2cUtilCCS.h"
#include "driver/tmr/drv_tmr_static.h"
#include "Mc32gest_SerComm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
	Holds application data

  Description:
	This structure holds the application's data.

  Remarks:
	This structure should be initialized by the APP_Initialize function.
	
	Application strings and buffers are be defined outside this structure.
*/


APPGEN_DATA appgenData;           // Structude du programme principale
APP_DATA appData;                 
S_ParamGen LocalParamGen;
S_ParamGen RemoteParamGen;
bool TCPCon;
bool saveTCP = false;
bool AffichageClear = true;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
	void APP_GEN_Initialize ( void )

  Remarks:
	See prototype in app_gen.h.
 */

void APPGEN_Initialize ( void )
{
	/* Place the App state machine in its initial state. */
	appgenData.state = APPGEN_STATE_INIT;

			
	// Init les timers 
	DRV_TMR0_Initialize();
	DRV_TMR1_Initialize();
}


/******************************************************************************
  Function:
	void APP_GEN_Tasks ( void )

  Remarks:
	See prototype in app_gen.h.
 */

void APPGEN_Tasks ( void )
{
	bool comTCP_ok = false;
	/* Check the application's current state. */
	switch ( appgenData.state )
	{
		/* Application's initial state. */
		case APPGEN_STATE_INIT:
		{
			// Flag Init --> on
			//Sers a autoriser l'affichage du menu 
			//lors de la premiere entree dans task
			appgenData.flag_first_task = true;
			
			//Temps_Init (3 secondes) Eteint sur system_interrupt.c
			LED6_W = LED_ON;  //Temps_Init (3 secondes) Allumage app.c
			
			//Init I2C
			i2c_init(true);
		  
			// Init Lcd
			lcd_init();
			lcd_bl_on(); // allume le backlight

			// Init SPI DAC
			SPI_InitLTC2604();

			// Initialisation PEC12
			Pec12Init();         
			
			// Initialisation du menu
			ClearAffichage(); //Efface le contenue du lcd
			MENU_Initialize(&LocalParamGen); //Affiche l'affichage d'init
			appgenData.flag_display_menu = true;
			
			// Initialisation du generateur
			// Lecture flash s'il ya des valeurs, elles sont recuperee
			// Sinon valeur par défault
			GENSIG_Initialize(&LocalParamGen);          
			
			//Synchroniser les valeurs USB et Local
			RemoteParamGen = LocalParamGen;
			
			// Active les timers 
			DRV_TMR0_Start();
			DRV_TMR1_Start();
			
			//Passage a l'etat "wait"
			APPGEN_UpdateState(APPGEN_STATE_WAIT);
			break;
		}
		case APPGEN_STATE_WAIT:
		{
			//ne rien faire            
			break;
		}
		case APPGEN_STATE_SERVICE_TASKS:
		{
			//toggle led 2 pour la frequence d'appel de task (20ms)
			//10 cycle de l'ISR du timer 10ms
			LED2_W = !LED2_R;
			if(appgenData.flag_display_menu == true)
			{
				//Si premire entree dans task
				if (appgenData.flag_first_task == true){
					ClearAffichage(); //Clear le contenue du LCD  
					MENU_Selection(); //Affiche le menu
					appgenData.flag_first_task = false; //Ne plus autoriser ces tâches
					Pec12ClearInactivity();
				}
				if (TCPCon == true) 
				{
					//affichage de l'adresse IP durant 5 secondes
					/*if(CompteurAffIP < 500)
					{
						//test temps affichage
						BSP_LEDOn(BSP_LED_7);
						CompteurAffIP++;
						//MENU_AffichageIP();
					}*/
					//else
					//{
						//test temps affichage
						BSP_LEDOff(BSP_LED_7);
						comTCP_ok = GetMessage((int8_t*)BufferTCPIP, &RemoteParamGen, &saveTCP);
						//CompteurAffIP = 500;
						MENU_Execute(&RemoteParamGen, false);
						//Synchronisation des valeurs
						LocalParamGen = RemoteParamGen;
						if(AffichageClear == true)
						{
							appgenData.flag_first_task = true;
						}
						AffichageClear = false;
					//}
				}
				else
				{
					if(appgenData.flag_display_menu == true)
					{
						MENU_Execute(&LocalParamGen, true);
					}
					//Synchronisation des valeurs
					RemoteParamGen = LocalParamGen;
				}
			}
			//si la connexion TCP est faite
			
			
			if (comTCP_ok) 
			{
				SendMessage(&RemoteParamGen, saveTCP);
				comTCP_ok = false;
				saveTCP = false;
			}
			
			//Selon les réglagles du menu, mettre à jour le signal
			GENSIG_UpdateSignal(&LocalParamGen);
			
			//Passage a l'etat attente
			APPGEN_UpdateState(APPGEN_STATE_WAIT);
			break;
		}

		/* The default state should never be executed. */
		default:
		{
			/* TODO: Handle error in application's state machine. */
			break;
		}
	}
}

// Permet de changer l'état de la machine d'état
void APPGEN_UpdateState ( APPGEN_STATES NewState )
{
	// Affectation du nouveau etat dans la fonction de la machine d'état
	appgenData.state = NewState;
}

//Clear tout le contenu du lcd
void ClearAffichage(void)
{
	lcd_ClearLine(LIGNE1_LCD);   //Efface la ligne 1
	lcd_ClearLine(LIGNE2_LCD);   //Efface la ligne 2
	lcd_ClearLine(LIGNE3_LCD);   //Efface la ligne 3
	lcd_ClearLine(LIGNE4_LCD);   //Efface la ligne 4
}

bool GetSaveTCP (void){
	return(saveTCP);
}

/*******************************************************************************
 End of File
 */
