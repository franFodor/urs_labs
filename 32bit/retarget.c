#include <stm32f407xx.h>  
#include <stdio.h>   // FILE, __stdin, __stdout 
#include <rt_misc.h> // razne funkcije za redeficiciju 
#include <rt_sys.h>  // _sys_open, _sys_write itd. 
#include <string.h>  // strncmp i sl. 

extern void sendchar_USART2(int); 
extern void send_char_morse(char);

// warn all calls which are not redefined
__asm(".global __use_no_semihosting");

// handle definitions
#define FH_STDIN    0x8001
#define FH_STDOUT   0x8002
#define FH_STDERR   0x8003
#define FH_MORSE    0x1234

// otvaranje jedinice fopen¸, deklaracija iz manuala
// argumenti, ime datoteke, nacin otvaranja
// izlaz handle (pokazivac na jedinicu)
FILEHANDLE _sys_open(const char* name, int openmode) {
  (void)openmode; // nije nam vazno koji je open, samo serijsko sucelje ocekujemo samo write
  
  if (!strncmp(name, ":STDIN", 6)) {
    return(FH_STDIN);
  } else if (!strncmp(name, ":STDOUT", 7)) {
    return(FH_STDOUT);
  } else if (!strncmp(name, ":STDERR", 7)) {
    return(FH_STDERR);
  } else if (!strncmp(name, "Morse", 5)) {
    return(FH_MORSE);
  } else {
    return -1;
  }
}

// je li jedinice interakvitna  medjukorak
int _sys_istty(FILEHANDLE fh) {
  if (fh == FH_STDIN) {
    return 1;
  } else if (fh == FH_STDOUT) {
    return 1;
  } else if (fh == FH_STDERR) {
    return 1;
  } else if (fh == FH_MORSE) {
    return 1;
  } else {
    return -1;
  }
}

// pisanje u jedinicu fwrite
int _sys_write(FILEHANDLE fh, const unsigned char* buf, unsigned len, int mode) {
  (void)mode;
  
  if (fh == FH_STDOUT) {
    while (len != 0) {
      sendchar_USART2(*buf);
      buf++;
      len--;
    }
    return 0;
  } else if (fh == FH_MORSE){
     while (len != 0) {
      send_char_morse(*buf);
      buf++;
      len--;
    }
    return 0;
  } else {
    return -1;
  }
}

// funkcija za dojava greske
void _ttywrch(int ch) { 
  sendchar_USART2(ch); 
} 

__attribute__((noreturn)) void _sys_exit(int return_code) { 
  (void)return_code; // rezultat maina - ne zanima nas
  while(1); 
} 

void sendchar_USART2(int32_t c) { 
  while (!(USART2->SR & 0x0080));   // cekamo da prethodni char ode
  USART2->DR = (c & 0xFF);          // posalji character
} 

// zatvorit jedinicu fclose
int _sys_close (FILEHANDLE fh) {
  if (fh == FH_STDIN) {
      return 0;
   } else if (fh == FH_STDOUT) {
      return 0;
   } else if (fh == FH_STDERR) {
      return 0;
   } else if (fh == FH_MORSE) {
      return 0;
   } else {
      return -1;
   }
}

