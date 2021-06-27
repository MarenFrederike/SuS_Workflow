#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include <math.h>
#include <stdio.h>



// Praeprozessor-Makros
#define BUFFER_SIZE 1000
#define SAMPLERATE 44000


// Funktionen-Deklarationen
void adcIntHandler(void);
void setup(void);

// globale Variablen
// hier die benötigten globalen Variablen, wie den Ringbuffer einfuegen

void main(void){ // nicht veraendern!! Bitte Code in adcIntHandler einfuegen
    setup();
    while(1){}
}

void setup(void){// konfiguriert den MiKrocontroller

    // konfiguriere System-Clock
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    uint32_t period = SysCtlClockGet()/SAMPLERATE;

    // aktiviere Peripherie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // konfiguriere GPIO
    GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    // konfiguriere Timer
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period - 1);
    TimerControlTrigger(TIMER0_BASE,TIMER_A,true);
    TimerEnable(TIMER0_BASE,TIMER_A);

    // konfiguriere ADC
    ADCClockConfigSet(ADC0_BASE,ADC_CLOCK_RATE_FULL,1);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH1|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE,3);
    ADCIntRegister(ADC0_BASE,3,adcIntHandler);
    ADCIntEnable(ADC0_BASE,3);

}

void adcIntHandler (void){
   uint32_t adcInputValue;
   ADCSequenceDataGet(ADC0_BASE,3,&adcInputValue);
   // Bitte Code hier einfuegen
   // initialize the array needed for the ring buffer, we always want to build the average of 1000 elements
   //therefore array with N=1000, the input from the ADC is in form of an int

   int ringBuffer [BUFFER_SIZE];

   //save the squared inputs, read 1ooo Samples and store them in array
   //before start averaging, store 1000 values
   for (int i = 0; i<= 999; i++){
       ringBuffer[i] = adcInputValue*adcInputValue;
       //use adc*adc instead? faster?
   }

   //now start averaging

   //build first sum

   uint32_t sum = 0;
   for (int j = 0; j<=999; j++){
       sum = sum + ringBuffer[j];
   }
   //buffer to store the last element

   int buffer;
   while (1){
       for (int k=0; k<= 999; k++){
           buffer = ringBuffer[k];
           ringBuffer[k] = adcInputValue*adcInputValue;
           sum = sum - buffer + ringBuffer[k];
           //average : divide by 1000
           uint32_t average = sum/1000;

       }
   }


   // am Ende von adcIntHandler, Interrupt-Flag loeschen
   ADCIntClear(ADC0_BASE,3);
}


/*
 * code Manuel

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"
//#include <complex>
//#include <iostream>

// Praepozessor-Makros
#define SAMPLERATE 44000

//Funktionen-Deklarationen
void adcIntHandler(void);
void setup(void);
// hier nach Bedarf noch weitere Funktionsdeklarationen einfuegen

// globale Variablen
const float DoublePi = 6.283185308;
int32_t bufferSample[440];
uint16_t zaehlvariable = 0;
uint32_t k;
uint32_t n =1;
float real;
float imag;
uint32_t amplitude;
uint32_t kwert;
uint32_t kstelle;



// hier nach Bedarf noch weitere globale Variablen einfuegen

void main(void){ // nicht veraendern!! Bitte Code in adcIntHandler einfuegen
    setup();
    while(1){}
}

void setup(void){//konfiguriert den Mikrocontroller

    // konfiguriere SystemClock
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    uint32_t period = SysCtlClockGet()/SAMPLERATE;

    // aktiviere Peripherie
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // aktiviere Gleitkommazahlen-Modul
    FPUEnable();
    FPUStackingEnable();
    FPULazyStackingEnable();
    FPUFlushToZeroModeSet(FPU_FLUSH_TO_ZERO_EN);

    // konfiguriere GPIO
    GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    // konfiguriere Timer
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period - 1);
    TimerControlTrigger(TIMER0_BASE,TIMER_A,true);
    TimerEnable(TIMER0_BASE,TIMER_A);

    // konfiguriere ADC
    ADCClockConfigSet(ADC0_BASE,ADC_CLOCK_RATE_FULL,1);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH1|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE,3);
    ADCIntRegister(ADC0_BASE,3,adcIntHandler);
    ADCIntEnable(ADC0_BASE,3);

}

void adcIntHandler(void){
   // Bitte Code hier einfuegen
   // ...

    if (zaehlvariable < 440){
        uint32_t adcInputValue;
        ADCSequenceDataGet(ADC0_BASE,3,&adcInputValue);
        bufferSample[zaehlvariable]=adcInputValue;
        zaehlvariable++;
    }

    else if (zaehlvariable == 440 && n< 440){
        real = real + bufferSample[n] *cosf(DoublePi*(n/440)*k);
        imag = imag + bufferSample[n] *sinf(DoublePi*(n/440)*k);

        n++;

    }

    else if (zaehlvariable == 440 && n == 440 && k<= 4000){


        amplitude  = sqrt ( pow(real,2) + pow(imag,2 ));

        if (amplitude >= kwert ){

            kwert = amplitude;
            kstelle = k;

        }

       k++;
       n=0;

    }

    else if (k > 4000){


        zaehlvariable = 0;
        k = 0 ;
        n = 0 ;

        if(kstelle <= 500){
                   //1. LED einschalten
                   GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, 0xFF);
               }
               if(kstelle > 500 && kstelle <= 1000){
                       //2. LED einschalten
                       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0xFF);
                   }
               if(kstelle > 1000 && kstelle <= 1500){
                       //3. LED einschalten
                       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xFF);
                   }
               if(kstelle > 1500 && kstelle <= 2000){
                       //4. LED einschalten
                       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xFF);
                   }
               if(kstelle > 2000 && kstelle <= 2500){
                       //5.LED einschalten
                       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0xFF);
                   }
               if(kstelle > 2500 && kstelle <= 3000){
                       //6. LED einschalten
                       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0xFF);
                   }
               if(kstelle > 3000 && kstelle <= 3500){
                       //7. LED einschalten
                       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, 0xFF);
                  }
               if(kstelle > 3500 && kstelle <= 4000){
                       //8. LED einschalten
                       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, 0xFF);
                  }

               kstelle = 0;
               kwert= 0;
    }




   // am Ende von adcIntHandler, Interrupt-Flag loeschen
   ADCIntClear(ADC0_BASE,3);
}


*/
