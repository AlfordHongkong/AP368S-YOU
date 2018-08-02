#ifndef	_MY_GPIO_H_
#include "mydefine.h"
#include "stm32f1xx.h"

#define	PA0	0x0000
#define	PA1	0x0001
#define	PA2	0x0002
#define	PA3	0x0003
#define	PA4	0x0004
#define	PA5	0x0005
#define	PA6	0x0006
#define	PA7	0x0007
#define	PA8	0x0008
#define	PA9	0x0009
#define	PA10	0x000A
#define	PA11	0x000B
#define	PA12	0x000C
#define	PA13	0x000D
#define	PA14	0x000E
#define	PA15	0x000F

#define	PB0	0x0010
#define	PB1	0x0011
#define	PB2	0x0012
#define	PB3	0x0013
#define	PB4	0x0014
#define	PB5	0x0015
#define	PB6	0x0016
#define	PB7	0x0017
#define	PB8	0x0018
#define	PB9	0x0019
#define	PB10	0x001A
#define	PB11	0x001B
#define	PB12	0x001C
#define	PB13	0x001D
#define	PB14	0x001E
#define	PB15	0x001F

#define	PC0	0x0020
#define	PC1	0x0021
#define	PC2	0x0022
#define	PC3	0x0023
#define	PC4	0x0024
#define	PC5	0x0025
#define	PC6	0x0026
#define	PC7	0x0027
#define	PC8	0x0028
#define	PC9	0x0029
#define	PC10	0x002A
#define	PC11	0x002B
#define	PC12	0x002C
#define	PC13	0x002D
#define	PC14	0x002E
#define	PC15	0x002F


#define	PD0	0x0030
#define	PD1	0x0031
#define	PD2	0x0032
#define	PD3	0x0033
#define	PD4	0x0034
#define	PD5	0x0035
#define	PD6	0x0036
#define	PD7	0x0037
#define	PD8	0x0038
#define	PD9	0x0039
#define	PD10	0x003A
#define	PD11	0x003B
#define	PD12	0x003C
#define	PD13	0x003D
#define	PD14	0x003E
#define	PD15	0x003F

#define	PE0	0x0040
#define	PE1	0x0041
#define	PE2	0x0042
#define	PE3	0x0043
#define	PE4	0x0044
#define	PE5	0x0045
#define	PE6	0x0046
#define	PE7	0x0047
#define	PE8	0x0048
#define	PE9	0x0049
#define	PE10	0x004A
#define	PE11	0x004B
#define	PE12	0x004C
#define	PE13	0x004D
#define	PE14	0x004E
#define	PE15	0x004F
//a gpio setting depends by the "PIN_CONF_XXX" | "PIN_SPEED_xxx" | "PIN_PULL_XX" 

#define  PIN_CONF_INPUT                		0x0000 //  ((uint32_t)0x00000000)   /*!< Input Floating Mode                   */
#define  PIN_CONF_OUTPUT_PP       		0x0100 //  ((uint32_t)0x00000001)   /*!< Output Push Pull Mode                 */
#define  PIN_CONF_OUTPUT_OD                  	0x0200 //  ((uint32_t)0x00000011)   /*!< Output Open Drain Mode                */
#define  PIN_CONF_AF_PP                      	0x0300 //  ((uint32_t)0x00000002)   /*!< Alternate Function Push Pull Mode     */
#define  PIN_CONF_AF_OD                       	0x0400// ((uint32_t)0x00000012)   /*!< Alternate Function Open Drain Mode    */
#define  PIN_CONF_AF_INPUT                    	0x0500// GPIO_MODE_INPUT          /*!< Alternate Function Input Mode         */
#define  PIN_CONF_ANALOG                       	0x0600 //((uint32_t)0x00000003)   /*!< Analog Mode  */
#define  PIN_CONF_IT_RISING                   	0x0700// ((uint32_t)0x10110000)   /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  PIN_CONF_IT_FALLING                   	0x0800//((uint32_t)0x10210000)   /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  PIN_CONF_IT_RISING_FALLING            	0x0900//((uint32_t)0x10310000)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
#define  PIN_CONF_EVT_RISING                   	0x0A00//((uint32_t)0x10120000)   /*!< External Event Mode with Rising edge trigger detection               */
#define  PIN_CONF_EVT_FALLING                  	0x0B00//((uint32_t)0x10220000)   /*!< External Event Mode with Falling edge trigger detection              */
#define  PIN_CONF_EVT_RISING_FALLING           	0x0C00//((uint32_t)0x10320000)   /*!< External Event Mode with Rising/Falling edge trigger detection       */

#define  PIN_PULL_NONE        			0x0000  /*!< No Pull-up or Pull-down activation  */
#define  PIN_PULL_UP        			0x0010 	/*!< Pull-up activation                  */
#define  PIN_PULL_DOWN     			0x0020	  /*!< Pull-down activation                */

#define  PIN_SPEED_LOW               		0x0000 /*!< Low speed */
#define  PIN_SPEED_MEDIUM           		0x0001 	/*!< Medium speed */
#define  PIN_SPEED_HIGH             		0x0002   /*!< High speed */

//gpio function select. HSB= gpio_mode, LSB high nibble is pull setting
//LSB low nibble is speed setting, @see gpio_config_pin function
#define	GPIO_INPUT_FLOATING	 	(PIN_CONF_INPUT | PIN_SPEED_LOW | PIN_PULL_NONE)
#define	GPIO_INPUT_PULL_UP	    	(PIN_CONF_INPUT | PIN_SPEED_LOW | PIN_PULL_UP)
#define	GPIO_INPUT_PULL_DOWN	    	(PIN_CONF_INPUT | PIN_SPEED_LOW | PIN_PULL_DOWN)

#define	GPIO_OUT			(PIN_CONF_OUTPUT_PP | PIN_SPEED_HIGH | PIN_PULL_NONE)
#define	GPIO_OUT_OPENDRAIN		(PIN_CONF_OUTPUT_OD | PIN_SPEED_HIGH | PIN_PULL_NONE)
#define	GPIO_OUT_OPENDRAIN_UP		(PIN_CONF_OUTPUT_OD | PIN_SPEED_HIGH | PIN_PULL_UP)

#define	GPIO_ANALOG			(PIN_CONF_ANALOG | PIN_SPEED_LOW | PIN_PULL_NONE)

#define	GPIO_AF_IN			(PIN_CONF_AF_INPUT | PIN_SPEED_HIGH)
#define	GPIO_AF_PULL_UP_IN		(PIN_CONF_AF_INPUT | PIN_SPEED_HIGH | PIN_PULL_UP)
#define	GPIO_AF_PULL_DWON_IN		(PIN_CONF_AF_INPUT | PIN_SPEED_HIGH | PIN_PULL_DOWN)
#define	GPIO_AF_OUT			(PIN_CONF_AF_PP    | PIN_SPEED_HIGH | PIN_PULL_NONE)
#define	GPIO_AF_OUT_OPENDRAIN		(PIN_CONF_AF_OD    | PIN_SPEED_HIGH | PIN_PULL_NONE)

BOOL	read_pin(U8 pin_name);
void	set_pin(U8 pin_name);
void	clr_pin(U8 pin_name);
void	write_pin(U8 pin_name,U8 value);
void 	toggle_pin(U8 pin_name);
void	config_pin(U8 pin_name, U16 function);


#endif

