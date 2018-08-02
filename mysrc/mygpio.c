#include "mygpio.h"
#include "mydefine.h"

static  uint32_t const gpio_speed[]=
{
	GPIO_SPEED_FREQ_LOW,
	GPIO_SPEED_FREQ_MEDIUM,
	GPIO_SPEED_FREQ_HIGH
};

static  uint32_t const gpio_drv_mode[]=
{
	GPIO_NOPULL,
	GPIO_PULLUP,
	GPIO_PULLDOWN
};

static  uint32_t const gpio_mode[]=
{
	GPIO_MODE_INPUT,                    	/*!< 0  Input Floating Mode                 */
	GPIO_MODE_OUTPUT_PP,                    /*!< 1  Output Push Pull Mode                */
	GPIO_MODE_OUTPUT_OD,                   	/*!< 2 Output Open Drain Mode             */
	GPIO_MODE_AF_PP,                        /*!< 3  Alternate Function Push Pull Mode     */
	GPIO_MODE_AF_OD,                       	/*!< 4  Alternate Function Open Drain Mode    */
	GPIO_MODE_AF_INPUT,                    	/*!< 5 Alternate Function Input Mode         */
	GPIO_MODE_ANALOG,                      	/*!< 6 Analog Mode  */
	GPIO_MODE_IT_RISING,              	/*!< 7 External Interrupt Mode with Rising edge trigger detection          */
	GPIO_MODE_IT_FALLING,                 	/*!< 8 External Interrupt Mode with Falling edge trigger detection         */
	GPIO_MODE_IT_RISING_FALLING,      	/*!< 9 External Interrupt Mode with Rising/Falling edge trigger detection  */
	GPIO_MODE_EVT_RISING,                   /*!< 10 External Event Mode with Rising edge trigger detection               */
	GPIO_MODE_EVT_FALLING,                 	/*!< 11 External Event Mode with Falling edge trigger detection              */
	GPIO_MODE_EVT_RISING_FALLING            /*!< 12 External Event Mode with Rising/Falling edge trigger detection       */
};

static  GPIO_TypeDef*  const gpio_addr[]=
{
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE
};

BOOL	read_pin(U8 pin_id)
{
	GPIO_TypeDef* gpio;
	gpio = gpio_addr[pin_id>>4];	//get the port address.
	pin_id  &= 0x0F;	
	return 0!=(gpio->IDR & (1<<pin_id));
}

void	write_pin(U8 pin_id,U8 value)
{
	GPIO_TypeDef* gpio;
	U32 bitmask;
	gpio = gpio_addr[pin_id>>4];	//get the port address.
	if(value){
		//low 16 bit. set pin.
		bitmask = 1<<(pin_id&0x0F);
	}else{
		//high 16bit clear pin.
		bitmask = 1<<((pin_id & 0x0F) | 0x10);
	}
	gpio->BSRR = bitmask;
}

void	set_pin(U8 pin_id)
{
	GPIO_TypeDef* gpio;
	gpio = gpio_addr[pin_id>>4];	//get the port address.
	gpio->BSRR = 1<<(pin_id&0x0F);
}
void	clr_pin(U8 pin_id)
{
	GPIO_TypeDef* gpio;
	gpio = gpio_addr[pin_id>>4];	//get the port address.
	gpio->BSRR =1<<((pin_id & 0x0F) | 0x10);
}

void 	toggle_pin(U8 pin_id)
{
	GPIO_TypeDef* gpio;
	gpio = gpio_addr[pin_id>>4];	//get the port address.
	gpio->ODR ^= (1<<(pin_id&0x0F));
}

void	config_pin(U8 pin_id, U16 func)
{

	GPIO_TypeDef* gpio;
	GPIO_InitTypeDef io_conf;

	gpio = gpio_addr[pin_id>>4];				//get the port address.

	io_conf.Mode = gpio_mode[func>>8];			// mode = HSB of the func
	io_conf.Speed = gpio_speed[func&0x000F];		// speed = low nibble of LSB
	io_conf.Pull = gpio_drv_mode[(func&0x00F0)>>4]; 	// pull = high nibble of LSB
	io_conf.Pin = 1<<(pin_id & 0x0f);
	HAL_GPIO_Init(gpio,&io_conf);
}

