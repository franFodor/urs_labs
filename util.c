#define F_CPU 3333333
#define BAUD_RATE 115200
// for i2c
#define BAUD_TWI 100000

// -------------------- 7 SEGMENT DISPLAY --------------------
#define SEG_A  (1 << 1)
#define SEG_B  (1 << 0)
#define SEG_C  (1 << 4)
#define SEG_D  (1 << 7)
#define SEG_E  (1 << 5)
#define SEG_F  (1 << 2)
#define SEG_G  (1 << 3)
#define SEG_DP (1 << 6)

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// -------------------- BASIC LED INTERACTIONS --------------------

// turns off the given led 
void turn_off_led(uint8_t led_num) {
    PORTD.OUT &= ~(1 << (8 - led_num));
}

// turns on the given led 
void turn_on_led(uint8_t led_num) {
    PORTD.OUT |= (1 << (8 - led_num));
}

// --------------------------------------------------------------

// -------------------- USART3 COMMUNICATION --------------------

// sends the char to stream
int usart_putchar(char c, FILE *stream) {
    while (!(USART3.STATUS & USART_DREIF_bm));
    USART3.TXDATAL = c;
    return 0;
}

// inits usart3 with the given baud rate 
void init_usart3(void) {
    PORTB.DIRSET = PIN0_bm;                                             // output (TX en)
    USART3.BAUD  = ((uint32_t)F_CPU * 64) / (16 * (uint32_t)BAUD_RATE); // set BAUD 
    USART3.CTRLB = USART_TXEN_bm;                                       // TX-only enable 
}

// -------------------------------------------------------

// -------------------- AD CONVERSION --------------------

// ad converter init
void init_adc(void) {
    ADC0.CTRLA  = ADC_ENABLE_bm;                                        // enable ADC
    ADC0.CTRLC  = ADC_REFSEL_VDDREF_gc;                                 // set VDD as ref voltage
    ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc;                                   // select pin using mux
}

// -----------------------------------------------------------

// -------------------- 7 SEGMENT DISPLAY --------------------

// all segment codes for easy access 
const unsigned char segment_codes[10] = {
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,                      // 0
    SEG_B | SEG_C,                                                      // 1
    SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,                              // 2
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,                              // 3
    SEG_F | SEG_G | SEG_B | SEG_C,                                      // 4
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,                              // 5
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E,                      // 6
    SEG_A | SEG_B | SEG_C,                                              // 7
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,              // 8
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G};                     // 9

// function to display a number on the display
// uses time-mux to display number on both displays
// displays for 1 second
void display_num(uint8_t num1, uint8_t num2) {
    for (uint8_t i = 0; i < 100; i++)  { 
        PORTE.OUT = PIN2_bm;
        PORTC.OUT = segment_codes[num1];
        _delay_ms(5);
    
        PORTE.OUT = PIN3_bm;
        PORTC.OUT = segment_codes[num2];
        _delay_ms(5);
    }    
}

// -------------------- BUZZER BUZZ --------------------

// buzzes for 100 ms
void buzz(void) {
    PORTD.OUTSET = 0x08;                                            
    _delay_ms(100);
    PORTD.OUTCLR = 0x08;
}

// -------------------------------------------------------

// -------------------- PWM GENERATOR --------------------

// pwm generator init for controlling the on-board heater 
void init_pwm(void) {
    PORTF.DIR |= 0x20;                                                   // PF5 (heater) output

    TCA0.SPLIT.CTRLD |= 0x01;                                            // enable split mode 
    TCA0.SPLIT.CTRLB |= 0x40;                                            // enable high byte compare 2 
    
    TCA0.SPLIT.CTRLA |= 0x08;                                            // prescaler 16x, as hper is 8 bit!
    uint8_t period = (F_CPU / (16 * 1000)) - 1;                          // period 1 ms (1 kHz), prescaler 16x
    
    TCA0.SPLIT.HPER = period;                                           // set the period
    TCA0.SPLIT.HCMP2 = (uint8_t)((float)period * 0.75f);                // duty cycle 75%
    PORTMUX.TCAROUTEA = PORTMUX_TCA0_PORTF_gc;                          // connect output of PWM to heater
    
    TCA0.SPLIT.CTRLA |= 0x01;                                           // enable PWM
}

// -----------------------------------------------------------

// -------------------- I2C COMMUNICATION --------------------

// i2c init 
void I2C_init(void) {
    TWI0.MBAUD = (uint8_t)((F_CPU / 2 / BAUD_TWI) - 5);                 // baud rate 100 kbps
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;                                // bus in idle state
    TWI0.MCTRLA = TWI_ENABLE_bm;                                        // enable TWI 
}

// function for starting the i2c communication
uint8_t I2C_start(uint8_t address, uint8_t rw) {
    TWI0.MADDR = address << 1 | rw;                                        
    if (!rw) {                                                          // 0 = write, 1 = read
        while (!(TWI0.MSTATUS & (TWI_WIF_bm)));                         // wait for write interrupt flag (if "write")
        TWI0.MSTATUS |= TWI_WIF_bm;
        TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;                            // master send ACK
        return 2;
    } else {
        while (!(TWI0.MSTATUS & (TWI_RIF_bm)));                         // wait for read interrupt flag 
        TWI0.MSTATUS |= TWI_RIF_bm;        
        return !(TWI0.MSTATUS & TWI_RXACK_bm);                          // return true (!0) if slave gave an ACK    
    }            

}

// stopping the i2c communication
void I2C_stop() {                                
    TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_STOP_gc;                     // send ACK / STOP
}

// writing data to i2c
uint8_t I2C_write(uint8_t data) {
    TWI0.MDATA = data;                                                  // send data
    while (!(TWI0.MSTATUS & (TWI_WIF_bm)));                             // wait for write interrupt flag
    TWI0.MSTATUS |= TWI_WIF_bm;
    TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;                                // do nothing on bus (no ACK etc.)
    return !(TWI0.MSTATUS & TWI_RXACK_bm);                              // returns true (!0) if slave gave an ACK
}

// reading from i2c 
uint8_t I2C_read(uint8_t last_byte) {
    while(!(TWI0.MSTATUS & TWI_CLKHOLD_bm));                            // wait for Clk Hold flag to be high
    if (last_byte) {
        TWI0.MCTRLB = TWI_ACKACT_bm | TWI_MCMD_RECVTRANS_gc;            // send NACK
    } else {
        TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;                            // send ACK
    }
    return TWI0.MDATA;
}

// ------------------------------------------------------------
