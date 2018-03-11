#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

int capsLock = 0, shift = 0;
// Hubert
unsigned char keyboardLowerCase[88] =
{
  ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '-', '=', ' ', ' ', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '[', ']', ' ', ' ', 'a',
  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
  '`', '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  ',', '.', '/', '\0', '*', ' ', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

unsigned char keyboardUpperCase[88] =
{
  ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '-', '=', ' ', ' ', 'Q', 'W', 'E', 'R', 'T',
  'Y', 'U', 'I', 'O', 'P', '[', ']', ' ', ' ', 'A',
  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'',
  '`', '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  ',', '.', '/', '\0', '*', ' ', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

unsigned char keyboardShiftLowerCase[88] =
{
  ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(',
  ')', '_', '+', ' ', ' ', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '{', '}', ' ', ' ', 'a',
  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"',
  '`', '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  '<', '>', '?', '\0', '*', ' ', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

unsigned char keyboardShiftUpperCase[88] =
{
  ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(',
  ')', '_', '+', ' ', ' ', 'Q', 'W', 'E', 'R', 'T',
  'Y', 'U', 'I', 'O', 'P', '{', '}', ' ', ' ', 'A',
  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
  '`', '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  '<', '>', '?', '\0', '*', ' ', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};
/*
unsigned char getScanCode(){               //interrupt driven approach
  unsigned char c = 0;
  do{
    if(inb(0x60) != c){
      c = inb(0x60);
      if(c > 0){
        return c;
      }
    }
  }while(1);
}*/
/*
unsigned char getScanCode(){ //polling keyboard
    while (!(inb(0x64) & 1));
    return inb(0x60);
}
*/
unsigned char getChar(unsigned char character){
  //if left or right shift key is pressed
  if(character == 0x2A || character == 0x36){
    shift = 1;
  }
  //if left or right shift key is released
  if(character == 0xAA || character == 0xB6){
    shift = 0;
  }
  //if capslock is pressed and previous value of capslock is 0
  if(character == 0x3A && capsLock == 0){
    capsLock = 1;
  }
  //if capslock is pressed and previous value of capslock is 1
  if(character == 0x3A && capsLock == 1){
    capsLock = 0;
  }
  //if button is pressed, not released
  if(character < 88){
    //keyboard for shift key on and no capslock
    if(shift == 1 && capsLock == 0){
      return keyboardShiftUpperCase[character - 1];
    }
    //keyboard for shift key off and capslock off
    else if(shift == 0 && capsLock == 0){
      return keyboardLowerCase[character - 1];
    }
    //keyboard for shift key on and capslock on
    else if(shift == 1 && capsLock == 1){
      return keyboardShiftLowerCase[character - 1];
    }
    //keyboard for shit off and capslock on
    else if(shift == 0 && capsLock == 1){
      return keyboardUpperCase[character - 1];
    }
  }
  return '\0';
}

void handle_keyboard_interrupt(){
  unsigned char character = inb(0x60);
  if(getChar(character) != '\0'){
	  printf("%d", capsLock);
	  printf("%c", getChar(character));
  }
  send_eoi(1);
  //printf("\nwrite done\n");
}
