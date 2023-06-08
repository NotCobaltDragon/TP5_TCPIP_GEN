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


#include "appgen.h"
#include "app.h"
#include "Mc32gestI2cSeeprom.h"
#include "Mc32_I2cUtilCCS.h"
#include "DefMenuGen.h"
#include "Mc32Delays.h"

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

void I2C_WriteSEEPROM(S_ParamGen **pParam, uint16_t NbBytes)
{
//      //si probleme bloquage avec l'eeprom
//    i2c_start();
//    i2c_write(MCP79411_EEPROM_W);
//    i2c_write(0xff);
//    i2c_write(0x00);
//    i2c_stop();
    
    uint8_t i=0;
    uint8_t i2=0;
    uint8_t *Ptr = (uint8_t*)*pParam;//ptr pointe sur l'adresse de la struct
    for(i2=0;i2<2;i2++)
    {
        if(i2==0)//ecriture sur une deuxieme page
        {
            i2c_start();
            i2c_write(MCP79411_EEPROM_W);//adresse d'ecriture
            i2c_write(MCP79411_EEPROM_BEG);//a quel adresse en commence
            for(i=0;i<8;i++)
            {
                i2c_write(Ptr[i]);//ecriture de la valeur de la stuct ,i pointe par pas de 8bit
            }
            i2c_stop();
        }
        delay_ms(5);//une page remplie donc attente pour pouvoir ecrire une nouvel
        if(i2==1)//ecriture sur une deuxieme page
        {
            i2c_start();
            i2c_write(MCP79411_EEPROM_W);//adresse d'ecriture
            i2c_write(MCP79411_EEPROM_BEG+8);//a quel adresse en recommence
            for(i=8;i<16;i++)
            {
                i2c_write(Ptr[i]);//ecriture de la valeur de la stuct ,[i] corespond ">>8"
            }
            i2c_stop();
        }
    }
} // end I2C_WriteSEEPROM

// Lecture d'un bloc dans l'EEPROM du MCP79411
void I2C_ReadSEEPROM(S_ParamGen **pParam,uint16_t NbBytes)
{
        //  si probleme bloquage avec l'eeprom
//    i2c_start();
//    i2c_write( MCP79411_EEPROM_W );
//    i2c_write(0xff);
//    i2c_reStart();
//    i2c_write( MCP79411_EEPROM_R );
//    i2c_read(false);
    
    uint8_t i2=0;
    uint8_t *ptr = (uint8_t*)*pParam;//ptr pointe sur l'adresse de la struct
    
    i2c_start();
    i2c_write( MCP79411_EEPROM_W );//adresse d'ecriture
    i2c_write(MCP79411_EEPROM_BEG);//adresse pointé
    i2c_reStart();
    i2c_write( MCP79411_EEPROM_R );//adresse de lecture
    
    for(i2=0;i2<NbBytes;i2++)
    {
        ptr[i2]=i2c_read(true);//LEcture et en met la valeur dans la struct a travers le ptr
    }
    ptr[i2]=i2c_read(false);//derniere lecture donc ACK a 0
    i2c_stop();

} // end I2C_ReadSEEPROM
   