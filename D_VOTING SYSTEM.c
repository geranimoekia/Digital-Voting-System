#define _XTAL_FREQ 8000000

// PIC16F877 Configuration Bit Settings
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // Code Protection bit (Code protection off)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low Voltage ICSP disabled
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code protection off)
#pragma config WRT = OFF        // FLASH Program Memory Write Enable (Unprotected program memory)

// LCD Pin Mapping
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>
#include "HS.h"
#include <stdio.h>
#include <string.h>

// Vote counts for candidates
int ttlVotesOf_A = 0;  // Candidate A
int ttlVotesOf_B = 0;  // Candidate B
int ttlVotesOf_C = 0;  // Candidate C
int ttlVotesOf_D = 0;  // Candidate D

char votes[16];
char selectedCandidate = '\0';

// Predefined PIN code
const char correctPin[5] = {'8', '0', '0', '8', '5'};
char enteredPin[5];
int pinIndex = 0;

// Keypad matrix mapping
char read_keypad() {
    for (int row = 0; row < 4; row++) {
        // Activate one row at a time
        PORTB = (1 << row);

        if (PORTBbits.RB4 == 1) return (row == 0) ? 'A' : (row == 1) ? '3' : (row == 2) ? '2' : '1';
        if (PORTBbits.RB5 == 1) return (row == 0) ? 'B' : (row == 1) ? '6' : (row == 2) ? '5' : '4';
        if (PORTBbits.RB6 == 1) return (row == 0) ? 'C' : (row == 1) ? '9' : (row == 2) ? '8' : '7';
        if (PORTBbits.RB7 == 1) return (row == 0) ? 'D' : (row == 1) ? '#' : (row == 2) ? '0' : '*';
    }
    return '\0'; // No key pressed
}

void display_candidates() {
    // Display candidate information
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("A: KGOTSO  B: NIMO");
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String("C: EKIA    D: LOU");
    __delay_ms(1500);
    Lcd_Clear();
}

void enter_pin() {
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Enter PIN Code");

    pinIndex = 0;  // Reset PIN input index
    while (pinIndex < 5) {
        char key = read_keypad();
        if (key >= '0' && key <= '9') {
            enteredPin[pinIndex] = key;
            pinIndex++;
            Lcd_Set_Cursor(2, pinIndex);
            Lcd_Write_String("*");  // Show asterisks for entered PIN
        }
        __delay_ms(200);  // Debouncing delay
    }
}

void blink_led() {
    for (int i = 0; i < 3; i++) {
        PORTAbits.RA0 = 1;  // Turn on LED
        __delay_ms(500);
        PORTAbits.RA0 = 0;  // Turn off LED
        __delay_ms(500);
    }
}

void welcome_message() {
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String(" VOTING SYSTEM ");
    __delay_ms(3000);
}

void wait_for_restart() {
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Press 1 to");
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String("Restart Voting");

    while (1) {
        char key = read_keypad();
        if (key == '1') {
            welcome_message();
            break;
        }
        __delay_ms(200);  // Debouncing delay
    }
}

void main(void) {
    // Configure ports
    TRISA = 0x00;    // Set PORTA as output (RA0 for LED)
    TRISB = 0xF0;    // RB0-RB3 as outputs (rows), RB4-RB7 as inputs (columns)
    TRISD = 0x00;    // Port D as output (LCD)
    TRISC = 0x00;    // Port C as output (Confirmation LED)
    PORTA = 0x00;    // Clear Port A
    PORTB = 0x00;    // Clear Port B
    PORTC = 0x00;    // Clear Port C

    // Blink LED 3 times before starting the system
    blink_led();

    // Keep LED on while the system is running
    PORTAbits.RA0 = 1;

    // Initialize LCD
    Lcd_Init();

    // Initial welcome message
    welcome_message();

    while (1) {
        // Loop for PIN entry
        int pinCorrect = 0;
        while (!pinCorrect) {
            enter_pin();  // Prompt for PIN entry

            // Check if entered PIN matches the correct PIN
            if (memcmp(enteredPin, correctPin, 5) == 0) {
                Lcd_Clear();
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("WELCOME");
                __delay_ms(1000);
                pinCorrect = 1;  // Exit loop if PIN is correct
            } else {
                Lcd_Clear();
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("INVALID PIN");
                __delay_ms(1000);  // Show invalid PIN message
            }

            // Clear entered PIN after each attempt
            memset(enteredPin, 0, 5);  // Reset the enteredPin array
            pinIndex = 0;  // Reset the pinIndex
        }

        // Voting process
        while (1) {
            display_candidates();

            // Prompt user to vote
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Vote: ");
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("A-D to select");

            while (1) {
                char key = read_keypad();

                if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
                    selectedCandidate = key;

                    // Confirm vote
                    Lcd_Clear();
                    Lcd_Set_Cursor(1, 1);
                    char message[16];
                    sprintf(message, "Vote: %c", selectedCandidate);
                    Lcd_Write_String(message);
                    Lcd_Set_Cursor(2, 1);
                    Lcd_Write_String("# to confirm");
                } else if (key == '#') {
                    if (selectedCandidate != '\0') {
                        // Record vote
                        if (selectedCandidate == 'A') ttlVotesOf_A++;
                        else if (selectedCandidate == 'B') ttlVotesOf_B++;
                        else if (selectedCandidate == 'C') ttlVotesOf_C++;
                        else if (selectedCandidate == 'D') ttlVotesOf_D++;

                        // Acknowledge vote
                        Lcd_Clear();
                        Lcd_Set_Cursor(1, 1);
                        Lcd_Write_String("Thanks for Vote!");
                        PORTCbits.RC2 = 1;  // Confirmation LED
                        __delay_ms(2000);
                        PORTCbits.RC2 = 0;  // Turn off LED
                        wait_for_restart();  // Wait for restart signal
                        break;  // Exit to restart voting process
                    }else {
                    // No candidate selected
                    Lcd_Clear();
                    Lcd_Set_Cursor(1, 1);
                    Lcd_Write_String("Invalid Response");
                    __delay_ms(1000);
                    break;  // Exit invalid phase to restart process
                }
                }else if (key == '*') {  // Display total votes with PIN entry
                Lcd_Clear();
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("Enter Vote PIN");
                enter_pin();

                if (memcmp(enteredPin, correctPin, 5) == 0) {
                    Lcd_Clear();
                    Lcd_Set_Cursor(1, 1);
                    sprintf(votes, "A:%d B:%d C:%d D:%d", ttlVotesOf_A, ttlVotesOf_B, ttlVotesOf_C, ttlVotesOf_D);
                    Lcd_Write_String(votes);
                    __delay_ms(5000);  // Display for 2 seconds
                    wait_for_restart();
                    break;
                } else {
                    Lcd_Clear();
                    Lcd_Set_Cursor(1, 1);
                    Lcd_Write_String("Invalid PIN");
                    __delay_ms(1000);
                    break;
                }

                memset(enteredPin, 0, 5);  // Reset the enteredPin array
                pinIndex = 0;  // Reset the pinIndex
            } else if (key != '\0') {  // Invalid key
                Lcd_Clear();
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("Invalid Response");
                __delay_ms(1000);
                break;  // Exit invalid phase to restart process
            }
                __delay_ms(200);  // Debouncing delay
            }
        }
    }
}