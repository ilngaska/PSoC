#include "project.h"

static void (*COLUMN_x_SetDriveMode[3])(uint8 mode) = {
    COLUMN_0_SetDriveMode, COLUMN_1_SetDriveMode, COLUMN_2_SetDriveMode
};
static void (*COLUMN_x_Write[3])(uint8 value) = {
    COLUMN_0_Write, COLUMN_1_Write, COLUMN_2_Write
};
static uint8 (*ROW_x_Read[4])() = {
    ROW_0_Read, ROW_1_Read, ROW_2_Read, ROW_3_Read
};

static uint8_t keys[4][3];
static uint8_t last_keys_state[4][3];
static uint8_t keys_values[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {10, 0, 11}
};

void setLED(uint8_t r, uint8_t g, uint8_t b) {
    LED_Red_Write(r);
    LED_Green_Write(g);
    LED_Blue_Write(b);
}

void printMatrix() {
    SW_Tx_UART_PutCRLF();
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 3; c++) {
            SW_Tx_UART_PutHexInt(keys[r][c]);
            SW_Tx_UART_PutString(" ");
        }
        SW_Tx_UART_PutCRLF();
    }
    SW_Tx_UART_PutCRLF();
}

void applyColorSwitch(uint8_t val) {
    switch(val) {
        case 1: case 7: 
            setLED(0, 1, 1); // Red
            break;
        case 2: case 8: 
            setLED(1, 0, 1); // Green
            break;
        case 3: case 9: 
            setLED(1, 1, 0); // Blue
            break;
        case 4: case 10: 
            setLED(0, 0, 1); // Yellow
            break;
        case 5: case 0: 
            setLED(0, 1, 0); // Purple
            break;
        case 6: case 11: 
            setLED(1, 0, 0); // Cyan
            break;
        default:
            setLED(1, 1, 1); // Black
            break;
    }
}

void initMatrix() {
    for(int i = 0; i < 3; i++) COLUMN_x_SetDriveMode[i](COLUMN_0_DM_DIG_HIZ);
    for(int r = 0; r < 4; r++) {
        for(int c = 0; c < 3; c++) last_keys_state[r][c] = 1;
    }
}

void readMatrix() {
    for(int col = 0; col < 3; col++) {
        COLUMN_x_SetDriveMode[col](COLUMN_0_DM_STRONG);
        COLUMN_x_Write[col](0);
        for(int row = 0; row < 4; row++) {
            keys[row][col] = ROW_x_Read[row]();
        }
        COLUMN_x_SetDriveMode[col](COLUMN_0_DM_DIG_HIZ);
    }
}

void printBtn(int r, int c, char* state) {
    SW_Tx_UART_PutString("Button ");
    uint8_t v = keys_values[r][c];
    if (v <= 9) SW_Tx_UART_PutChar(v + '0');
    else if (v == 10) SW_Tx_UART_PutString("*");
    else if (v == 11) SW_Tx_UART_PutString("#");
    SW_Tx_UART_PutString(state);
    SW_Tx_UART_PutCRLF();
}

int main(void) {
    CyGlobalIntEnable;
    SW_Tx_UART_Start();
    initMatrix();
    
    setLED(0, 0, 0); // White
    SW_Tx_UART_PutString("Software Transmit UART (Switch-case mode)\r\n");

    for(;;) {
        readMatrix();
        for(int r = 0; r < 4; r++) {
            for(int c = 0; c < 3; c++) {
                if (keys[r][c] == 0 && last_keys_state[r][c] == 1) {
                    printBtn(r, c, " pressed");
                    applyColorSwitch(keys_values[r][c]);
                    printMatrix();
                    last_keys_state[r][c] = 0;
                } 
                else if (keys[r][c] == 1 && last_keys_state[r][c] == 0) {
                    printBtn(r, c, " released");
                    setLED(1, 1, 1); // Black
                    last_keys_state[r][c] = 1;
                }
            }
        }
        CyDelay(50);
    }
}