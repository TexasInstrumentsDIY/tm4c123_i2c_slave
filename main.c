#define PART_TM4C123GH6PM 1

#include <stdint.h>
#include <stdbool.h>
#include "../inc/hw_ints.h"
#include "../inc/hw_i2c.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_types.h"
#include "../inc/hw_gpio.h"
#include "../driverlib/interrupt.h"
#include "../driverlib/i2c.h"
#include "../driverlib/sysctl.h"
#include "../driverlib/gpio.h"
#include "../driverlib/pin_map.h"
#include "../driverlib/uart.h"
#include "../utils/uartstdio.h"
#include "ST7735.h"
#define SLAVE_ADDRESS 0x3C
int32_t pad = 0;
volatile char   result = 'V';
int32_t pad2 = 0;
volatile char recv_buff[256] = {0};
volatile int32_t counter = 0;

void InitConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, 16000000);
}


void I2C1_Slave_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    I2CSlaveEnable(I2C0_BASE);
    I2CSlaveInit(I2C0_BASE, SLAVE_ADDRESS);
}

void I2C1SlaveIntHandler(void)
{
    // Clear the I2C0 interrupt flag.
    I2CSlaveIntClear(I2C1_BASE);
    // Read the data from the slave.
    result = I2CSlaveDataGet(I2C1_BASE);
}

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1| SYSCTL_USE_OSC| SYSCTL_OSC_MAIN| SYSCTL_XTAL_16MHZ);
		Output_Init();
		ST7735_DrawString(0,0, "Spice Rack", ST7735_YELLOW);
    InitConsole();
    I2C1_Slave_Init();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    IntEnable(INT_I2C0);
    I2CSlaveIntEnableEx(I2C0_BASE, I2C_SLAVE_INT_DATA);
    IntMasterEnable();
		
    while(1)
    {
        //I2CSlaveDataPut(I2C1_BASE, result);
        //UARTprintf(" Slave receive: '%c' \n", result);
        SysCtlDelay(SysCtlClockGet()/12);

        //while(!(I2CSlaveStatus(I2C1_BASE) & I2C_SLAVE_ACT_TREQ));
        I2CSlaveDataPut(I2C0_BASE, result);
			  //void ST7735_DrawCharS(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size);
				ST7735_DrawCharS(24, 24, result, ST7735_YELLOW, ST7735_BLACK, 1);
			  
    }
}
