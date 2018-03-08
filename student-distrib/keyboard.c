#include "keyboard.h"
#include "lib.h"


// Hubert

char getScanCode(){
  char c = 0;
  do{
    if(inb(0x60) != c){
      c = inb(0x60);
      if(c > 0){
        return c;
      }
    }
  }while(1);
}

char getChar(){
  return scancode[getScanCode() + 1];
}
