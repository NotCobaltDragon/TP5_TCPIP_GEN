//--------------------------------------------------------
// Mc32gestI2cEEprom.C
//--------------------------------------------------------
// Gestion I2C de la SEEPROM du MCP79411 (Solution exercice)
//	Description :	Fonctions pour EEPROM MCP79411
//
//	Auteur 		: 	C. HUBER
//      Date            :       26.05.2014
//	Version		:	V1.0
//	Compilateur	:	XC32 V1.31
// Modifications :
//
/*--------------------------------------------------------*/



#include "Mc32gestI2cSeeprom.h"
#include "Mc32_I2cUtilCCS.h"
#include "app.h"


// Définition pour MCP79411
#define MCP79411_EEPROM_R    0xAF         // MCP79411 address for read
#define MCP79411_EEPROM_W    0xAE         // MCP79411 address for write
// La EEPROM du 79411 est de 1 Kbits donc 128 octets
#define MCP79411_EEPROM_BEG   0x00         // addr. début EEPROM
#define MCP79411_EEPROM_END   0x7F         // addr. fin EEPROM

// Definitions du bus (pour mesures)
// #define I2C-SCK  SCL2/RA2      PORTAbits.RA2   pin 58
// #define I2C-SDA  SDa2/RA3      PORTAbits.RA3   pin 59




// Initialisation de la communication I2C et du MCP79411
// ------------------------------------------------

void I2C_InitMCP79411(void)
{
   bool Fast = true;
   i2c_init( Fast );
} //end I2C_InitMCP79411

// Ecriture d'un bloc dans l'EEPROM du MCP79411 
void I2C_WriteSEEPROM(void *SrcData, uint32_t EEpromAddr, uint16_t NbBytes)
{
    uint8_t i, j;
    uint8_t* scrByteData = SrcData; //Evite de caster *SrcData
    bool ack;
    uint16_t saveNbBytes;// = NbBytes; //Save de NbBytes pour manipulations
     
    uint8_t pageStart, pageEnd, nbPages;
    //_____Determine le nombre de page à écrire_____
    pageStart = EEpromAddr / 8;
    pageEnd = (EEpromAddr + NbBytes) / 8;
    nbPages = pageEnd - pageStart;
    //_______________________________________________
    
    for (j= 0 ; j < nbPages ; j++)
    {
        if ( NbBytes > 8)
            saveNbBytes = 8;
        else
            saveNbBytes = NbBytes;    
        
        do{
            i2c_start();
            ack = i2c_write(MCP79411_EEPROM_W);        
        }while(!ack);

        i2c_write(EEpromAddr);
        
        for ( i = 0 ; i < saveNbBytes; i++ ) {
            i2c_write(*scrByteData);
            scrByteData++;      
        }
        i2c_stop();
        
        EEpromAddr += saveNbBytes;
        NbBytes -= saveNbBytes;
    }
} // end I2C_WriteSEEPROM

// Lecture d'un bloc dans l'EEPROM du MCP79411
void I2C_ReadSEEPROM(void *DstData, uint32_t EEpromAddr, uint16_t NbBytes)
{
    int i;
    bool ack;
    //Realisation du start
    do{
        i2c_start();
        ack = i2c_write(MCP79411_EEPROM_W);        
    }while(!ack);
    //Fin du start
    i2c_write(EEpromAddr); //Adress
    i2c_reStart(); //Restart
    i2c_write(MCP79411_EEPROM_R);
     for ( i = 0 ; i < NbBytes; i++ ) {
        if (i == (NbBytes - 1)){
           *((uint8_t*)DstData) = i2c_read(0); ///NO ACK  
        }
        else{
            *((uint8_t*)DstData)  = i2c_read(1); // ACK
        }
        //((uint8_t*)DstData) ++;
        DstData = ((uint8_t*)DstData) + 1;
    }
    i2c_stop();
} // end I2C_ReadSEEPROM
   





 



