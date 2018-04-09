#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "file_desc.h"

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define ATTRIB      0x7
#define VIDEO       0xB8000
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define CAPS 0x3A
#define CTRL_ON 0x1D
#define CTRL_OFF 0x9D
#define SHIFT_LEFT_OFF 0xAA
#define SHIFT_LEFT_ON 0x2A
#define SHIFT_RIGHT_ON 0x36
#define SHIFT_RIGHT_OFF 0xB6
#define L_PRESSED 0x26

static char* video_mem = (char *)VIDEO;
int capsLock = 0, shift = 0, ctrl = 0;
int currentrow = 0;
int terminalrow = 0;
int currentcolumn = 0;
int bufferPos = 0;
volatile int readflag = 0;
char mode[20];
char keyboardbuffer[128];
unsigned char keyboardLowerCase[88] =
{
  '\0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '0', '-', '=', '\0', '\0', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a',
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
  'Y', 'U', 'I', 'O', 'P', '[', ']', '\0', '\0', 'A',
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
  'y', 'u', 'i', 'o', 'p', '{', '}', '\0', '\0', 'a',
  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"',
  '`', '\0', '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
  '<', '>', '?', '\0', '*', '\0', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '0', '0', '0', '1', '2'
};

unsigned char keyboardShiftUpperCase[88] =
{
  '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(',
  ')', '_', '+', '\0', '\0', 'Q', 'W', 'E', 'R', 'T',
  'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A',
  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"',
  '`', '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
  '<', '>', '?', '\0', '*', '\0', ' ', '\0', '1', '2',
  '3', '4', '5', '6', '7', '8', '9', '0', 'N', 'S',
  '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
  '3', '0', '.', '\0', '\0', '\0', '1', '2'
};
/*
 * getChar
 *   DESCRIPTION: reads in the keycode and deciphers it, returning the decoded keyboard value back to user
 *   INPUTS: keyboard opcode
 *   OUTPUTS: none
 *   RETURN VALUE: ascii character
 */

unsigned char getChar(unsigned char character){
  //if left or right shift key is pressed
  if(character == SHIFT_LEFT_ON || character == SHIFT_RIGHT_ON){
    shift = 1;
  }
  //if left or right shift key is released
  if(character == SHIFT_LEFT_OFF || character == SHIFT_RIGHT_OFF){
    shift = 0;
  }
  if(character == CTRL_ON){
    ctrl = 1;
  }
  if(character == CTRL_OFF){
    ctrl = 0;
  }
  //if capslock is pressed and previous value of capslock is 0
  if(character == CAPS && capsLock == 0){
    capsLock = 1;
	//printf("%d", capsLock);
  }
  //if capslock is pressed and previous value of capslock is 1
  else if(character == CAPS && capsLock == 1){
    capsLock = 0;
	//printf("%d", capsLock);
  }
  if(character == ENTER){
    //saves last location of cursor
    //in order to read line feed character
    readflag = 1;
    //printf("%d", bufferPos);
    //update_boundaries();
  }
  //if backspace is pressed
  if(character == BACKSPACE){
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
	//clears the location on screen
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
	//if control l is pressed, clears the screen
    if(character == L_PRESSED){
      currentcolumn = 0;
      currentrow = 0;
      terminalrow = 0;
      bufferPos = 0;
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
    keyboardbuffer[bufferPos] = decoded;
    bufferPos++;
    if(bufferPos == 127){
      readflag = 1;
      terminalrow = currentrow;
    }
    else{
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = decoded;
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
      currentcolumn++;// move the cursor forward
    }
  }
  //TODO : take care of where printf print stuff
  update_boundaries();
  update_cursor(currentrow, currentcolumn);
  send_eoi(1);
}

/*
 * terminal_open
 *   DESCRIPTION: opens the keyboard and initializes the values
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */

int32_t terminal_open(fd_t* a, const uint8_t* filename){
  clear();
  return 0;
}
int32_t terminal_close(fd_t* fd){
  return 0;
}

/*
 * terminal_write
 *   DESCRIPTION: takes in a string and number of bytes and writes number of bytes in string to terminal
 *   INPUTS: fd, string, number of bytes
 *   OUTPUTS: string outputted to screen
 *   RETURN VALUE: number of bytes outputted to screen
 */

int32_t terminal_write(fd_t *fd, uint8_t *string, int32_t length){
  //printf("%d %d", valid, length);
  //goes to next line
  if(string == NULL){
    return -1;
  }
  currentrow++;
  currentcolumn = 0;
  //makes sure line is not out of bounds
  update_boundaries();
  int i;
  //prints characters in string to screen
  for(i = 0; i < length; i++){
    unsigned char character = (char*)string[i];
    //printf("%d", i);
    if(i > strlen(string)){
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = ' ';
      *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
      currentcolumn++;// move the cursor forward
    }
    else{
      if(character == '\n'){
        currentcolumn = 0;
        currentrow++;
      }
      else{
        *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1)) = character;
        *(uint8_t *)(video_mem + ((VGA_WIDTH * currentrow + currentcolumn) << 1) + 1) = ATTRIB;
        currentcolumn++;// move the cursor forward
      }
    }
	  //makes sure currentcolumn does not go out of bounds
    update_boundaries();
    update_cursor(currentrow, currentcolumn);
  }
  return length;
}

/*
 * terminal_read
 *   DESCRIPTION: takes in number of bytes and stores the number of bytes on terminal into the buffer
 *   INPUTS: fd, buffer, number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes copied
 */

int32_t terminal_read(fd_t *fd, uint8_t *string, int32_t length){
  if(string == NULL){
    return -1;
  }
  memset(string, '\0', strlen(string));
  readflag = 0;
  while(!readflag);
  (char*)string[length] = '\n';
  int i;
  for(i = 0; i < bufferPos; i++){
    (char*)string[i] = keyboardbuffer[i];
  }
  terminal_write(0, string, bufferPos);
  readflag = 0;
  currentrow++;
  currentcolumn = 0;
  terminalrow = currentrow;
  bufferPos = 0;
  return length;
}

/*
 * update_boundaries
 *   DESCRIPTION: implements scrolling and makes sure the currentcolumn and currentrow never go out of bounds
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */

void update_boundaries(){
  if(currentcolumn > VGA_WIDTH - 1){ // if currencolumn exceeds width of screen
    currentcolumn = 0;
    currentrow++;
  }
  int i, j;
  //if currentrow exceeds size of screen, copies the currentrow + 1 to currentrow and leaves last row blank
  if(currentrow >= VGA_HEIGHT){
    for(i = 0; i < VGA_WIDTH; i++){
      for(j = 0; j < VGA_HEIGHT - 1; j++){
        *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1)) = *(uint8_t *)(video_mem + ((VGA_WIDTH * (j + 1) + i) << 1));
        *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1) + 1) = ATTRIB;
      }
    }
	//clears last row
    for(i = 0; i < VGA_WIDTH; i++){
      *(uint8_t *)(video_mem + ((VGA_WIDTH * (VGA_HEIGHT - 1) + i) << 1)) = ' ';
      *(uint8_t *)(video_mem + ((VGA_WIDTH * (VGA_HEIGHT - 1) + i) << 1) + 1) = ATTRIB;
    }
	//reset currentrow to last row, terminalrow, and currentcolumn
    currentrow = VGA_HEIGHT - 1;
    if(bufferPos != 0){
      terminalrow = currentrow - 1;
    }
    else{
      terminalrow = currentrow;
    }
    currentcolumn = 0;
  }
}
/*
 * update_cursor
 *   DESCRIPTION: takes the row and column and updates cursor to that location
 *   INPUTS: row, col
 *   OUTPUTS: cursor to row, col location on screen
 *   RETURN VALUE: none
 */

void update_cursor(int row, int col){
	uint16_t pos = row * VGA_WIDTH + col;
	outb(0x0F,0x3D4);
	outb((uint8_t) (pos & 0xFF),0x3D5);
	outb(0x0E,0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF),0x3D5);
}


int32_t invalid_func() {
  return -1;
}
