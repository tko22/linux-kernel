#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "file_desc.h"

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define ATTRIB      0x7
#define VIDEO       0xB8000

static char* video_mem = (char *)VIDEO;
int capsLock = 0, shift = 0, ctrl = 0;
int currentrow = 0;
int terminalrow = 0;
int currentcolumn = 0;
int bufferPos = 0;
char buffer[128];
char mode[20];
unsigned char keyboardLowerCase[88] =
{
  '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '-', '=', '\0', '\0', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a',
  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
  '`', '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  ',', '.', '/', '\0', '*', '\0', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '\0', '\0', '\0', '1', '2'
};

unsigned char keyboardUpperCase[88] =
{
  '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '-', '=', '\0', '\0', 'Q', 'W', 'E', 'R', 'T',
  'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', '\0', 'A',
  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'',
  '`', '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  ',', '.', '/', '\0', '*', '\0', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '\0', '\0', '\0', '1', '2'
};

unsigned char keyboardShiftLowerCase[88] =
{
  '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(',
  ')', '_', '+', '\0', '\0', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '{', '}', '\n', '\0', 'a',
  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"',
  '`', '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  '<', '>', '?', '\0', '*', '\0', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

unsigned char keyboardShiftUpperCase[88] =
{
  '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(',
  ')', '_', '+', '\0', '\0', 'Q', 'W', 'E', 'R', 'T',
  'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', '\0', 'A',
  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
  '`', '\0', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  '<', '>', '?', '\0', '*', '\0', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '\0', '\0', '\0', '1', '2'
};
unsigned char getChar(unsigned char character){
  //if left or right shift key is pressed
  if(character == 0x2A || character == 0x36){
    shift = 1;
  }
  //if left or right shift key is released
  if(character == 0xAA || character == 0xB6){
    shift = 0;
  }
  if(character == 0x1D){
    ctrl = 1;
  }
  //if left or right shift key is released
  if(character == 0x9D){
    ctrl = 0;
  }
  //if capslock is pressed and previous value of capslock is 0
  if(character == 0x3A && capsLock == 0){
    capsLock = 1;
	//printf("%d", capsLock);
  }
  //if capslock is pressed and previous value of capslock is 1
  else if(character == 0x3A && capsLock == 1){
    capsLock = 0;
	//printf("%d", capsLock);
  }
  //if backspace is pressed
  if(character == 0x0E){
    //printf("%d %d", terminalrow, currentrow);
    currentcolumn--;
    bufferPos--;
    if(currentcolumn < 0){
      currentrow--;
      //restores last location of cursor
      currentcolumn = VGA_WIDTH - 1;
      //if at the top left of the screen
      if(currentrow < terminalrow){
        currentrow = terminalrow;
        currentcolumn = 0;
        bufferPos = 0;
      }
    }
    *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = ' ';
    *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
    return '\0';
  }
  //if button is pressed, not released
  if(character < 88 && ctrl == 0){
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
  else if(character < 88 && ctrl == 1){
    if(character == 0x26){
      currentcolumn = 0;
      currentrow = 0;
      clear();
    }
  }
  return '\0';
}

/*
 * handle_keyboard_interrupt
 *   DESCRIPTION: called to handle keyboard interrupts and calls getChar to convert scancode to ascii
 *   INPUTS: none
 *   OUTPUTS: ascii character to screen
 *   RETURN VALUE: none
 */

void handle_keyboard_interrupt(){
  //gets the keycode from keyboard port
  unsigned char character = inb(0x60);
  //if char returned not empty character, print to screen
  if(getChar(character) != '\0'){
    char decoded = getChar(character);
    bufferPos++;
    if(decoded == '\n'){
      //saves last location of cursor
      //in order to read line feed character
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = decoded;
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
      currentcolumn++;
      keyboard_read(0, buffer, bufferPos);
      //printf("%c, %d", buffer[0], bufferPos);
      currentcolumn--;
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = ' ';
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
      keyboard_write(0, buffer, bufferPos);
      terminalrow = currentrow;
    }
    else{
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = decoded;
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
      currentcolumn++;// move the cursor forward
    }
    if(bufferPos == 128){
      keyboard_read(0, buffer, bufferPos);
      keyboard_write(0, buffer, bufferPos);
      bufferPos = 0;
    }
  }
  //TODO : take care of where printf print stuff
  update_boundaries();
  update_cursor(currentrow, currentcolumn);
  send_eoi(1);
}

int32_t keyboard_open(){
  clear();
  capsLock = 0;
  shift = 0;
  ctrl = 0;
  currentrow = 0;
  currentcolumn = 0;
  bufferPos = 0;
  terminalrow = 0;
  return 0;
}

int32_t keyboard_write(int32_t fd, char *string, int32_t length){
  currentrow++;
  currentcolumn = 0;
  int valid = sizeof(string) / sizeof(string[0]);
  if(length > valid){
    return -1;
  }
  update_boundaries();
  int i;
  for(i = 0; i < length; i++){
    unsigned char character = string[i];
    //printf("%d", i);
    *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = character;
    *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
    currentcolumn++;// move the cursor forward
    update_boundaries();
    update_cursor(currentrow, currentcolumn);
  }
  currentrow++;
  currentcolumn = 0;
  bufferPos = 0;
  update_boundaries();
  return length;
}

int32_t keyboard_read(int32_t fd, char *string, int32_t length){
  int i;
  for(i = 0; i < length; i++){
    string[i] = *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn - length + i) << 1));
  }
  return length;
}

void update_boundaries(){
  if(currentcolumn > VGA_WIDTH - 1){ // if it goes beyond the screen
    currentcolumn = 0;
    currentrow++;
  }
  int i, j;
  if(currentrow >= VGA_HEIGHT){
    for(i = 0; i < VGA_WIDTH; i++){
      for(j = 0; j < VGA_HEIGHT - 1; j++){
        *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1)) = *(uint8_t *)(video_mem + ((VGA_WIDTH * (j + 1) + i) << 1));
        *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1) + 1) = ATTRIB;
      }
    }
    for(i = 0; i < VGA_WIDTH; i++){
      *(uint8_t *)(video_mem + ((VGA_WIDTH * (VGA_HEIGHT - 1) + i) << 1)) = ' ';
      *(uint8_t *)(video_mem + ((VGA_WIDTH * (VGA_HEIGHT - 1) + i) << 1) + 1) = ATTRIB;
    }
    currentrow = VGA_HEIGHT - 1;
    terminalrow = currentrow;
    currentcolumn = 0;
  }
}

void update_cursor(int row, int col){
	uint16_t pos = row * VGA_WIDTH + col;
	outb(0x0F,0x3D4);
	outb((uint8_t) (pos & 0xFF),0x3D5);
	outb(0x0E,0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF),0x3D5);
}
