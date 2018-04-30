#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "file_desc.h"
#include "scheduling.h"

#define VGA_HEIGHT 25
#define VGA_WIDTH 80
#define ATTRIB      0x7
#define VIDEO       0xB8000
#define TERMINAL 0x4000000
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
#define ALT_ON 0x38
#define ALT_OFF 0xB8
#define F1_ON 0x3B
#define F1_OFF 0xBB
#define _4KB 4096

static char* video_mem = (char *)VIDEO;
static char* term_mem = (char*)TERMINAL;
int capsLock = 0, shift = 0, ctrl = 0, alt = 0, function = -1;
volatile int readflag = 0;
char mode[20];
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
  if(character >= F1_ON && character < F1_ON + MAX_TERMINALS){
	function = character - F1_ON;
  }
  if(character >= F1_OFF && character < F1_OFF + MAX_TERMINALS){
	function = -1;
  }
  if(character == ALT_ON){
	alt = 1;
  }
  if(character == ALT_OFF){
	alt = 0;
  }
  if(function != -1 && alt == 1 && ctrl == 1){
    send_eoi(1);
    switch_terminal(function);
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
    terminals[currentterminal].readflag = 1;
    //printf("%d", bufferPos);
    //update_boundaries();
  }
  //if backspace is pressed
  if(character == BACKSPACE){
    //printf("%d %d", terminalrow, currentrow);
    if(terminals[currentterminal].currentcolumn > terminals[currentterminal].terminalcol){
      terminals[currentterminal].currentcolumn--;
      terminals[currentterminal].bufferPos--;
      if(terminals[currentterminal].currentcolumn < 0){
        terminals[currentterminal].currentrow--;
        //restores last location of cursor
        terminals[currentterminal].currentcolumn = VGA_WIDTH - 1;
        //if at the top left of the screen
        if(terminals[currentterminal].currentrow < terminals[currentterminal].terminalrow){
          terminals[currentterminal].currentrow = terminals[currentterminal].terminalrow;
          terminals[currentterminal].currentcolumn = 0;
          terminals[currentterminal].bufferPos = 0;
        }
      }
    }
	//clears the location on screen
    *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1)) = ' ';
    *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1) + 1) = ATTRIB;
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
      terminals[currentterminal].currentcolumn = 0;
      terminals[currentterminal].currentrow = 0;
      terminals[currentterminal].terminalrow = 0;
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
  //printf("keyboard handled");
  unsigned char character = inb(0x60);
  //if char returned not empty character, print to screen
  if(terminals[currentterminal].bufferPos == 0){
    terminals[currentterminal].terminalrow = terminals[currentterminal].currentrow;
    terminals[currentterminal].terminalcol = terminals[currentterminal].currentcolumn;
  }
  if(getChar(character) != '\0'){
    char decoded = getChar(character);
    terminals[currentterminal].keyboardbuffer[terminals[currentterminal].bufferPos] = decoded;
    terminals[currentterminal].bufferPos++;
    if(terminals[currentterminal].bufferPos == 127){
      terminals[currentterminal].readflag = 1;
      terminals[currentterminal].terminalrow = terminals[currentterminal].currentrow;
    }
    else{
      *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1)) = decoded;
      *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1) + 1) = ATTRIB;
      terminals[currentterminal].currentcolumn++;// move the cursor forward
    }
  }
  //TODO : take care of where printf print stuff
  update_boundaries();
  update_cursor(terminals[currentterminal].currentrow, terminals[currentterminal].currentcolumn);
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

int32_t terminal_write(fd_t *fd, const uint8_t *string, int32_t length){
  //printf("%d %d", valid, length);
  //goes to next line
  if(string == NULL || (int32_t)fd < 0){
    return -1;
  }
  char* s = (char*)string;
  //makes sure line is not out of bounds
  update_boundaries();
  int i;
  pcb_t *curr = get_last_pcb();
  //prints characters in string to screen
  cli();
  for(i = 0; i < length; i++){
    unsigned char character = s[i];
    //printf("%d", i);
    if(i > strlen(s)){
      if(curr->terminal_id == currentterminal){
        *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1)) = ' ';
        *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1) + 1) = ATTRIB;
        terminals[currentterminal].currentcolumn++;// move the cursor forward
      }
      else{
        *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * terminals[curr->terminal_id].currentrow + terminals[curr->terminal_id].currentcolumn) << 1)) = ' ';
        *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * terminals[curr->terminal_id].currentrow + terminals[curr->terminal_id].currentcolumn) << 1) + 1) = ATTRIB;
        terminals[curr->terminal_id].currentcolumn++;// move the cursor forward
      }
    }
    else{
      if(character == '\n'){
        terminals[curr->terminal_id].currentcolumn = 0;
        terminals[curr->terminal_id].currentrow++;
      }
      else{
        if(curr->terminal_id == currentterminal){
          *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1)) = character;
          *(uint8_t *)(video_mem + ((VGA_WIDTH * terminals[currentterminal].currentrow + terminals[currentterminal].currentcolumn) << 1) + 1) = ATTRIB;
          terminals[currentterminal].currentcolumn++;// move the cursor forward
        }
        else{
          *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * terminals[curr->terminal_id].currentrow + terminals[curr->terminal_id].currentcolumn) << 1)) = character;
          *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * terminals[curr->terminal_id].currentrow + terminals[curr->terminal_id].currentcolumn) << 1) + 1) = ATTRIB;
          terminals[curr->terminal_id].currentcolumn++;// move the cursor forward
        }
      }
    }
	  //makes sure currentcolumn does not go out of bounds
    update_boundaries();
    update_cursor(terminals[curr->terminal_id].currentrow, terminals[curr->terminal_id].currentcolumn);
  }
  sti();
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
  if(string == NULL || (int32_t)fd < 0){
    return -1;
  }
  sti();
  pcb_t *curr = get_last_pcb();
  char* s = (char*)string;
  while(!terminals[curr->terminal_id].readflag);
  int i;
  for(i = 0; i < terminals[curr->terminal_id].bufferPos; i++){
    s[i] = terminals[curr->terminal_id].keyboardbuffer[i];
  }
  s[i] = '\0';
  //terminal_write(0, (uint8_t*)s, bufferPos);
  terminals[curr->terminal_id].readflag = 0;
  terminals[curr->terminal_id].currentrow++;
  terminals[curr->terminal_id].currentcolumn = 0;
  terminals[curr->terminal_id].terminalrow = terminals[currentterminal].currentrow;
  terminals[curr->terminal_id].bufferPos = 0;
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
  pcb_t *curr = get_last_pcb();
  if(terminals[curr->terminal_id].currentcolumn > VGA_WIDTH - 1){ // if currencolumn exceeds width of screen
    terminals[curr->terminal_id].currentcolumn = 0;
    terminals[curr->terminal_id].currentrow++;
  }
  int i, j;
  //if currentrow exceeds size of screen, copies the currentrow + 1 to currentrow and leaves last row blank
  if(terminals[curr->terminal_id].currentrow >= VGA_HEIGHT){
    for(i = 0; i < VGA_WIDTH; i++){
      for(j = 0; j < VGA_HEIGHT - 1; j++){
        if(curr->terminal_id == currentterminal){
          *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1)) = *(uint8_t *)(video_mem + ((VGA_WIDTH * (j + 1) + i) << 1));
          *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1) + 1) = ATTRIB;
        }
        else{
          *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * j + i) << 1)) = *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * (j + 1) + i) << 1));
          *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * j + i) << 1) + 1) = ATTRIB;
        }
      }
    }
	//clears last row
    for(i = 0; i < VGA_WIDTH; i++){
      if(curr->terminal_id == currentterminal){
        *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1)) = ' ';
        *(uint8_t *)(video_mem + ((VGA_WIDTH * j + i) << 1) + 1) = ATTRIB;
      }
      else{
        *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * j + i) << 1)) = ' ';
        *(uint8_t *)(term_mem + curr->terminal_id * _4KB + ((VGA_WIDTH * j + i) << 1) + 1) = ATTRIB;
      }
    }
	//reset currentrow to last row, terminalrow, and currentcolumn
    terminals[curr->terminal_id].currentrow = VGA_HEIGHT - 1;
    if(terminals[curr->terminal_id].bufferPos != 0){
      terminals[curr->terminal_id].terminalrow = terminals[curr->terminal_id].currentrow - 1;
    }
    else{
      terminals[curr->terminal_id].terminalrow = terminals[curr->terminal_id].currentrow;
    }
    terminals[curr->terminal_id].currentcolumn = 0;
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
