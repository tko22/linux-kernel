#include "keyboard.h"
#include "lib.h"

int capsLock = 0, shift = 0;
// Hubert
<<<<<<< HEAD
char keyboardLowerCase[88] =
{
  ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '-', '=', ' ', ' ', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '[', ']', ' ', ' ', 'a',
  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
  '`', ' ', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  ',', '.', '/', ' ', '*', ' ', ' ', ' ', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

char keyboardUpperCase[88] =
{
  ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '-', '=', ' ', ' ', 'Q', 'W', 'E', 'R', 'T',
  'Y', 'U', 'I', 'O', 'P', '[', ']', ' ', ' ', 'A',
  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'',
  '`', ' ', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  ',', '.', '/', ' ', '*', ' ', ' ', ' ', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

char keyboardShiftLowerCase[88] =
{
  ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(',
  ')', '_', '+', ' ', ' ', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '{', '}', ' ', ' ', 'a',
  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"',
  '`', ' ', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  '<', '>', '?', ' ', '*', ' ', ' ', ' ', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

char keyboardShiftUpperCase[88] =
{
  ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(',
  ')', '_', '+', ' ', ' ', 'Q', 'W', 'E', 'R', 'T',
  'Y', 'U', 'I', 'O', 'P', '{', '}', ' ', ' ', 'A',
  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
  '`', ' ', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  '<', '>', '?', ' ', '*', ' ', ' ', ' ', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

=======
/*
>>>>>>> 89f33724803bf5394b4a11eaf340a7628bba0662
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
<<<<<<< HEAD
  if(getScanCode() - 1 == 0x2A || getScaneCode() - 1 == 0x36){
    shift = 1;
  }
  if(getScanCode() - 1 == 0xAA || getScaneCode() - 1 == 0xB6){
    shift = 0;
  }
  if(getScanCode() - 1 == 0x3A && capsLock == 0){
    capsLock = 1;
  }
  if(getScanCode() - 1 == 0x3A && capsLock == 1){
    capsLock = 0;
  }
  if(getScanCode() < 88){
    if(shift == 1 && capsLock == 0){
      return keyboardShiftUpperCase[getScanCode() - 1];
    }
    else if(shift == 0 && capsLock == 0){
      return keyboardLowerCase[getScanCode() - 1];
    }
    else if(shift == 1 && capsLock == 1){
      return keyboardShiftLowerCase[getScanCode() - 1];
    }
    else if(shift == 0 && capsLock == 1){
      return keyboardUpperCase[getScanCode() - 1];
    }
  }
  return '';
=======
  return scancode[getScanCode() + 1];
}*/

void init_keyboard(){ // do this hubert

>>>>>>> 89f33724803bf5394b4a11eaf340a7628bba0662
}
