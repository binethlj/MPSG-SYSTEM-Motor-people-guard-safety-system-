
// PIC16F877 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#pragma config WRT = OFF        // FLASH Program Memory Write Enable (Unprotected program memory may not be written to by EECON control)

//==========================================================
// PIC16F877A - INTELLIGENT INDUSTRIAL MOTOR SAFETY SYSTEM
//==========================================================

#include <xc.h>

//==========================================================
// CONFIGURATION BITS
//==========================================================

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000UL


//==========================================================
// LCD CONNECTIONS
//==========================================================

#define LCD_RS RD4
#define LCD_EN RD5

#define LCD_D4 RD0
#define LCD_D5 RD1
#define LCD_D6 RD2
#define LCD_D7 RD3


//==========================================================
// MOTOR / OUTPUT CONNECTIONS
//==========================================================

#define MOTOR_IN1       RC0
#define MOTOR_IN2       RC1

#define COOLING_FAN     RC3
#define BUZZER          RC4
#define WARNING_MOTOR   RC5


//==========================================================
// LED CONNECTIONS
//==========================================================

#define GREEN_LED       RE0
#define YELLOW_LED      RE1
#define RED_LED         RE2


//==========================================================
// INPUT CONNECTIONS
//==========================================================

#define VIBRATION       RB0
#define FLAME           RB1
#define EMERGENCY_STOP  RB2
#define START_BUTTON    RB3
#define RESET_BUTTON    RB4


//==========================================================
// SYSTEM STATES
//==========================================================

#define STATE_NORMAL       0
#define STATE_WARNING      1
#define STATE_CRITICAL     2
#define STATE_FIRE         3
#define STATE_EMERGENCY    4


//==========================================================
// FAULT REASONS
//==========================================================

#define FAULT_NONE          0
#define FAULT_CURRENT       1
#define FAULT_VIBRATION     2
#define FAULT_TEMPERATURE   3
#define FAULT_EMERGENCY     4
#define FAULT_FIRE          5


//==========================================================
// FUNCTION PROTOTYPES
//==========================================================

void LCD_Init(void);
void LCD_Command(unsigned char command);
void LCD_Data(unsigned char data);
void LCD_String(const char *text);
void LCD_Clear(void);
void LCD_Number(unsigned int number);

void ADC_Init(void);
unsigned int ADC_Read(unsigned char channel);

void Motor_ON(void);
void Motor_OFF(void);

void Cooling_ON(void);
void Cooling_OFF(void);

void Buzzer_ON(void);
void Buzzer_OFF(void);

void WarningMotor_ON(void);
void WarningMotor_OFF(void);

void Siren_Reset(void);
void Siren_Update(void);


//==========================================================
// SIREN VARIABLES
//==========================================================

unsigned char siren_counter = 0;
unsigned char siren_step = 0;
unsigned char siren_output = 0;


//==========================================================
// LCD COMMAND
//==========================================================

void LCD_Command(unsigned char command)
{
    LCD_RS = 0;

    // High nibble
    LCD_D4 = (command >> 4) & 1;
    LCD_D5 = (command >> 5) & 1;
    LCD_D6 = (command >> 6) & 1;
    LCD_D7 = (command >> 7) & 1;

    LCD_EN = 1;
    __delay_us(5);
    LCD_EN = 0;

    // Low nibble
    LCD_D4 = command & 1;
    LCD_D5 = (command >> 1) & 1;
    LCD_D6 = (command >> 2) & 1;
    LCD_D7 = (command >> 3) & 1;

    LCD_EN = 1;
    __delay_us(5);
    LCD_EN = 0;

    __delay_us(50);
}


//==========================================================
// LCD DATA
//==========================================================

void LCD_Data(unsigned char data)
{
    LCD_RS = 1;

    // High nibble
    LCD_D4 = (data >> 4) & 1;
    LCD_D5 = (data >> 5) & 1;
    LCD_D6 = (data >> 6) & 1;
    LCD_D7 = (data >> 7) & 1;

    LCD_EN = 1;
    __delay_us(5);
    LCD_EN = 0;

    // Low nibble
    LCD_D4 = data & 1;
    LCD_D5 = (data >> 1) & 1;
    LCD_D6 = (data >> 2) & 1;
    LCD_D7 = (data >> 3) & 1;

    LCD_EN = 1;
    __delay_us(5);
    LCD_EN = 0;

    __delay_us(50);
}


//==========================================================
// LCD STRING
//==========================================================

void LCD_String(const char *text)
{
    while (*text)
    {
        LCD_Data(*text);
        text++;
    }
}


//==========================================================
// LCD CLEAR
//==========================================================

void LCD_Clear(void)
{
    LCD_Command(0x01);
    __delay_ms(2);
}


//==========================================================
// LCD INITIALIZATION
//==========================================================

void LCD_Init(void)
{
    __delay_ms(20);

    LCD_RS = 0;
    LCD_EN = 0;

    LCD_Command(0x02);
    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x06);
    LCD_Command(0x01);

    __delay_ms(2);
}


//==========================================================
// LCD NUMBER
//==========================================================

void LCD_Number(unsigned int number)
{
    if (number >= 1000)
    {
        LCD_Data((number / 1000) + '0');
        number = number % 1000;
    }

    if (number >= 100)
    {
        LCD_Data((number / 100) + '0');
        number = number % 100;
    }

    if (number >= 10)
    {
        LCD_Data((number / 10) + '0');
        number = number % 10;
    }

    LCD_Data(number + '0');
}


//==========================================================
// ADC INITIALIZATION
//==========================================================

void ADC_Init(void)
{
    /*
       AN0 = LM35
       AN1 = ACS712

       VREF+ = VDD
       VREF- = VSS

       Right justified

       FOSC = 20 MHz
       ADC clock = FOSC/32
    */

    ADCON1 = 0x84;
    ADCON0 = 0x81;

    TRISA = 0x03;
}


//==========================================================
// ADC READ
//==========================================================

unsigned int ADC_Read(unsigned char channel)
{
    ADCON0 &= 0xC7;

    ADCON0 |= (channel << 3);

    __delay_us(20);

    GO_nDONE = 1;

    while (GO_nDONE)
    {
        ;
    }

    return ((unsigned int)ADRESH << 8) | ADRESL;
}


//==========================================================
// MAIN MOTOR
//==========================================================

void Motor_ON(void)
{
    MOTOR_IN1 = 1;
    MOTOR_IN2 = 0;
}


void Motor_OFF(void)
{
    MOTOR_IN1 = 0;
    MOTOR_IN2 = 0;
}


//==========================================================
// COOLING FAN
//==========================================================

void Cooling_ON(void)
{
    COOLING_FAN = 1;
}


void Cooling_OFF(void)
{
    COOLING_FAN = 0;
}


//==========================================================
// BUZZER
//==========================================================

void Buzzer_ON(void)
{
    BUZZER = 1;
}


void Buzzer_OFF(void)
{
    BUZZER = 0;
}


//==========================================================
// WARNING MOTOR
//==========================================================

void WarningMotor_ON(void)
{
    WARNING_MOTOR = 1;
}


void WarningMotor_OFF(void)
{
    WARNING_MOTOR = 0;
}


//==========================================================
// SIREN RESET
//==========================================================

void Siren_Reset(void)
{
    siren_counter = 0;
    siren_step = 0;
    siren_output = 0;

    Buzzer_OFF();
}


//==========================================================
// SIREN UPDATE
//
// Creates a changing ON/OFF pattern.
// Because your Proteus buzzer is an ACTIVE buzzer,
// this creates a pulsing siren effect.
//==========================================================

void Siren_Update(void)
{
    unsigned char period;

    /*
       Different periods create the rising/falling
       siren-style effect.

       Main loop = approximately 50 ms.

       Step:
       0 -> 2
       1 -> 3
       2 -> 4
       3 -> 5
       4 -> 6
       5 -> 5
       6 -> 4
       7 -> 3
       8 -> 2
       9 -> 1
    */

    switch (siren_step)
    {
        case 0:
            period = 2;
            break;

        case 1:
            period = 3;
            break;

        case 2:
            period = 4;
            break;

        case 3:
            period = 5;
            break;

        default:
            period = 1;
            break;
    }


    siren_counter++;

    if (siren_counter >= period)
    {
        siren_counter = 0;

        if (siren_output == 0)
        {
            siren_output = 1;
            Buzzer_ON();
        }
        else
        {
            siren_output = 0;
            Buzzer_OFF();
        }

        siren_step++;

        if (siren_step >= 10)
        {
            siren_step = 0;
        }
    }
}


//==========================================================
// MAIN PROGRAM
//==========================================================

void main(void)
{
    unsigned int temp_adc;
    unsigned int current_adc;

    unsigned int temperature;
    unsigned int current_x10;

    unsigned long voltage_mV;
    unsigned long current_difference;

    unsigned char motor_running;

    unsigned char fault_latched;
    unsigned char fault_reason;
    unsigned char latched_state;

    unsigned char state;
    unsigned char previous_state;


    //======================================================
    // PORT DIRECTIONS
    //======================================================

    TRISA = 0x03;

    TRISB = 0x1F;

    TRISC = 0x00;

    TRISD = 0x00;

    TRISE = 0x00;


    //======================================================
    // INITIAL PORT VALUES
    //======================================================

    PORTA = 0x00;

    PORTB = 0x1F;

    PORTC = 0x00;

    PORTD = 0x00;

    PORTE = 0x00;


    //======================================================
    // ENABLE INTERNAL PORTB PULL-UPS
    //======================================================

    OPTION_REGbits.nRBPU = 0;


    //======================================================
    // INITIALIZE ADC
    //======================================================

    ADC_Init();


    //======================================================
    // INITIALIZE LCD
    //======================================================

    LCD_Init();


    //======================================================
    // INITIAL OUTPUT STATE
    //======================================================

    Motor_OFF();

    Cooling_OFF();

    Buzzer_OFF();

    WarningMotor_OFF();

    GREEN_LED = 0;
    YELLOW_LED = 0;
    RED_LED = 0;


    motor_running = 0;

    fault_latched = 0;

    fault_reason = FAULT_NONE;

    latched_state = STATE_NORMAL;

    previous_state = 255;

    Siren_Reset();


    //======================================================
    // STARTUP MESSAGE
    //======================================================

    LCD_Clear();

    LCD_String("MOTOR SAFETY");

    LCD_Command(0xC0);

    LCD_String("SYSTEM READY");

    __delay_ms(1500);


    //======================================================
    // MAIN LOOP
    //======================================================

    while (1)
    {

        //==================================================
        // TEMPERATURE SENSOR
        //==================================================

        temp_adc = ADC_Read(0);

        /*
           LM35 = 10mV / degree C

           Temperature =
           ADC value x 500 / 1023
        */

        temperature =
            ((unsigned long)temp_adc * 500UL) / 1023UL;


        //==================================================
        // CURRENT SENSOR
        //==================================================

        current_adc = ADC_Read(1);

        voltage_mV =
            ((unsigned long)current_adc * 5000UL) / 1023UL;


        /*
           ACS712-05B

           Zero current = approximately 2.5V

           Sensitivity = 185mV/A
        */

        if (voltage_mV >= 2500UL)
        {
            current_difference =
                voltage_mV - 2500UL;
        }
        else
        {
            current_difference =
                2500UL - voltage_mV;
        }


        /*
           Current x10

           1.0A = 10
           3.0A = 30
           4.0A = 40
           5.0A = 50
        */

        current_x10 =
            (unsigned int)
            ((current_difference * 10UL) / 185UL);


        //==================================================
        // DEFAULT STATE
        //==================================================

        state = STATE_NORMAL;


        //==================================================
        // RESET BUTTON
        //==================================================

        if (RESET_BUTTON == 0)
        {
            fault_latched = 0;

            fault_reason = FAULT_NONE;

            latched_state = STATE_NORMAL;

            motor_running = 0;

            Motor_OFF();

            Cooling_OFF();

            Buzzer_OFF();

            WarningMotor_OFF();

            GREEN_LED = 0;
            YELLOW_LED = 0;
            RED_LED = 0;

            Siren_Reset();

            LCD_Clear();

            LCD_String("SYSTEM RESET");

            LCD_Command(0xC0);

            LCD_String("READY");

            __delay_ms(1000);

            previous_state = 255;

            continue;
        }


        //==================================================
        // IF A FAULT IS ALREADY LATCHED
        //
        // IMPORTANT:
        // DO NOT CHANGE THE ORIGINAL FAULT.
        //==================================================

        if (fault_latched)
        {
            state = latched_state;

            motor_running = 0;

            Motor_OFF();

            Cooling_ON();

            WarningMotor_ON();

            GREEN_LED = 0;
            YELLOW_LED = 0;
            RED_LED = 1;

            /*
               Fire keeps warning motor ON.
            */

            if (fault_reason == FAULT_FIRE)
            {
                WarningMotor_ON();
            }

            /*
               Emergency, current, vibration and temperature
               faults use the siren.
            */

            if ((fault_reason == FAULT_CURRENT) ||
                (fault_reason == FAULT_VIBRATION) ||
                (fault_reason == FAULT_TEMPERATURE) ||
                (fault_reason == FAULT_EMERGENCY) ||
                (fault_reason == FAULT_FIRE))
            {
                Siren_Update();
            }
        }


        //==================================================
        // NO FAULT CURRENTLY LATCHED
        // CHECK FOR NEW FAULTS
        //==================================================

        else
        {

            //================================================
            // EMERGENCY STOP BUTTON
            //================================================

            if (EMERGENCY_STOP == 0)
            {
                state = STATE_EMERGENCY;

                fault_latched = 1;

                fault_reason = FAULT_EMERGENCY;

                latched_state = STATE_EMERGENCY;

                motor_running = 0;

                Motor_OFF();

                Cooling_ON();

                WarningMotor_ON();

                GREEN_LED = 0;
                YELLOW_LED = 0;
                RED_LED = 1;

                Siren_Reset();

                Siren_Update();
            }


            //================================================
            // FIRE DETECTION
            //================================================

            else if (FLAME == 0)
            {
                state = STATE_FIRE;

                fault_latched = 1;

                fault_reason = FAULT_FIRE;

                latched_state = STATE_FIRE;

                motor_running = 0;

                Motor_OFF();

                Cooling_ON();

                WarningMotor_ON();

                GREEN_LED = 0;
                YELLOW_LED = 0;
                RED_LED = 1;

                Siren_Reset();

                Siren_Update();
            }


            //================================================
            // HIGH CURRENT
            //================================================

            else if (current_x10 >= 40)
            {
                state = STATE_EMERGENCY;

                fault_latched = 1;

                fault_reason = FAULT_CURRENT;

                latched_state = STATE_EMERGENCY;

                motor_running = 0;

                Motor_OFF();

                Cooling_ON();

                WarningMotor_ON();

                GREEN_LED = 0;
                YELLOW_LED = 0;
                RED_LED = 1;

                Siren_Reset();

                Siren_Update();
            }


            //================================================
            // ABNORMAL VIBRATION
            //================================================

            else if (VIBRATION == 0)
            {
                state = STATE_EMERGENCY;

                fault_latched = 1;

                fault_reason = FAULT_VIBRATION;

                latched_state = STATE_EMERGENCY;

                motor_running = 0;

                Motor_OFF();

                Cooling_ON();

                WarningMotor_ON();

                GREEN_LED = 0;
                YELLOW_LED = 0;
                RED_LED = 1;

                Siren_Reset();

                Siren_Update();
            }


            //================================================
            // HIGH TEMPERATURE
            //================================================

            else if (temperature >= 120)
            {
                state = STATE_EMERGENCY;

                fault_latched = 1;

                fault_reason = FAULT_TEMPERATURE;

                latched_state = STATE_EMERGENCY;

                motor_running = 0;

                Motor_OFF();

                Cooling_ON();

                WarningMotor_ON();

                GREEN_LED = 0;
                YELLOW_LED = 0;
                RED_LED = 1;

                Siren_Reset();

                Siren_Update();
            }


            //================================================
            // WARNING CONDITION
            //================================================

            else if ((temperature >= 70) ||
                     (current_x10 >= 30))
            {
                state = STATE_WARNING;

                Cooling_ON();

                Buzzer_OFF();

                Siren_Reset();

                WarningMotor_OFF();

                GREEN_LED = 0;
                YELLOW_LED = 1;
                RED_LED = 0;


                if (motor_running)
                {
                    Motor_ON();
                }
                else
                {
                    Motor_OFF();
                }
            }


            //================================================
            // NORMAL CONDITION
            //================================================

            else
            {
                state = STATE_NORMAL;

                Cooling_OFF();

                Buzzer_OFF();

                Siren_Reset();

                WarningMotor_OFF();

                GREEN_LED = 1;
                YELLOW_LED = 0;
                RED_LED = 0;


                // START BUTTON ACTIVE LOW

                if (START_BUTTON == 0)
                {
                    motor_running = 1;
                }


                if (motor_running)
                {
                    Motor_ON();
                }
                else
                {
                    Motor_OFF();
                }
            }
        }


        //==================================================
        // LCD DISPLAY
        //
        // Update only when state or fault reason changes.
        //==================================================

        if ((state != previous_state) ||
            (fault_latched && previous_state == 255))
        {
            previous_state = state;

            LCD_Clear();


            //================================================
            // NORMAL
            //================================================

            if (state == STATE_NORMAL)
            {
                LCD_String("SYSTEM NORMAL");

                LCD_Command(0xC0);

                LCD_String("T:");

                LCD_Number(temperature);

                LCD_String("C I:");

                LCD_Number(current_x10 / 10);

                LCD_Data('.');

                LCD_Number(current_x10 % 10);

                LCD_Data('A');
            }


            //================================================
            // WARNING
            //================================================

            else if (state == STATE_WARNING)
            {
                LCD_String("WARNING");

                LCD_Command(0xC0);

                LCD_String("T:");

                LCD_Number(temperature);

                LCD_String("C I:");

                LCD_Number(current_x10 / 10);

                LCD_Data('.');

                LCD_Number(current_x10 % 10);

                LCD_Data('A');
            }


            //================================================
            // CRITICAL
            //
            // Kept for compatibility.
            // Current serious faults now use EMERGENCY.
            //================================================

            else if (state == STATE_CRITICAL)
            {
                LCD_String("CRITICAL FAULT");

                LCD_Command(0xC0);

                LCD_String("MOTOR STOPPED");
            }


            //================================================
            // FIRE
            //================================================

            else if (state == STATE_FIRE)
            {
                LCD_String("!!! FIRE !!!");

                LCD_Command(0xC0);

                LCD_String("MOTOR STOPPED");
            }


            //================================================
            // EMERGENCY
            //================================================

            else if (state == STATE_EMERGENCY)
            {
                LCD_String("EMERGENCY STOP");

                LCD_Command(0xC0);


                // HIGH CURRENT

                if (fault_reason == FAULT_CURRENT)
                {
                    LCD_String("HIGH CURRENT");
                }


                // ABNORMAL VIBRATION

                else if (fault_reason == FAULT_VIBRATION)
                {
                    LCD_String("ABNORMAL VIBRATN");
                }


                // HIGH TEMPERATURE

                else if (fault_reason == FAULT_TEMPERATURE)
                {
                    LCD_String("HIGH TEMPERATURE");
                }


                // EMERGENCY BUTTON

                else
                {
                    LCD_String("MOTOR STOPPED");
                }
            }
        }


        //==================================================
        // LOOP DELAY
        //
        // 100 ms also controls the siren timing.
        //==================================================

        __delay_ms(100);
    }
}