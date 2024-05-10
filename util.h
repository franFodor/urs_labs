#ifndef util_header
#define util_header 

// -------------------- BASIC LED INTERACTIONS --------------------

// turns off the given led 
void turn_off_led(uint8_t led_num);

// turns on the given led 
void turn_on_led(uint8_t led_num);

// --------------------------------------------------------------

// -------------------- USART3 COMMUNICATION --------------------

// sends the char to stream
int usart_putchar(char c, FILE *stream);

// inits usart3 with the given baud rate 
void init_usart3(void);

// -------------------------------------------------------

// -------------------- AD CONVERSION --------------------

// ad converter init
void init_adc(void);

// -----------------------------------------------------------

// -------------------- 7 SEGMENT DISPLAY --------------------

// function to display a number on the display
// uses time-mux to display number on both displays
// displays for 1 second
void display_num(uint8_t num1, uint8_t num2);

// -------------------- BUZZER BUZZ --------------------

// buzzes for 100 ms
void buzz(void);

// -------------------------------------------------------

// -------------------- PWM GENERATOR --------------------

// pwm generator init for controlling the on-board heater 
void init_pwm(void);

// -----------------------------------------------------------

// -------------------- I2C COMMUNICATION --------------------

// i2c init 
void I2C_init(void);

// function for starting the i2c communication
uint8_t I2C_start(uint8_t address, uint8_t rw);

// stopping the i2c communication
void I2C_stop();

// writing data to i2c
uint8_t I2C_write(uint8_t data);

// reading from i2c 
uint8_t I2C_read(uint8_t last_byte);

// ------------------------------------------------------------


#endif