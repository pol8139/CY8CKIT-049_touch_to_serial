#include <project.h>
#include <stdlib.h>
#include <stdio.h>

#define STR_LENGTH 256
#define TOUCH_NUM CapSenseP4_TOTAL_CSD_WIDGETS

#define TRUE 1
#define FALSE 0

#define LEFT -1
#define MIDDLE 0
#define RIGHT 1

void InitUART(void);
void InitCapSense(void);
void UpdateCapSense(void);
void DetectHandPosition(int hand_position[2], int touch_bool[TOUCH_NUM]);
int DetectHandMotion(int hand_position[2], int hand_motion[2]);
void SendHandDataIfMoved(int hand_position[2], int hand_motion[2]);
char GetMotionChar(int motion);
void SendUARTKB(int motion);

char message[STR_LENGTH];

static const int motion_list[3] = {LEFT, MIDDLE, RIGHT};

int main() {
    
    CyGlobalIntEnable;      /* Enable global interrupts */
    
    PWM_2_Start();
    InitUART();
    InitCapSense();
    int hand_position[2];
    int touch_bool[TOUCH_NUM];
    int hand_motion[2] = {MIDDLE, MIDDLE};
    int i;
    while(1) {
        if(!CapSenseP4_IsBusy()) {
            DetectHandPosition(hand_position, touch_bool);
            if(DetectHandMotion(hand_position, hand_motion)) {
                for(i = 0; i < TOUCH_NUM; i++) {
                    itoa(touch_bool[i], message, 10);
                    UART_UartPutString(message);
                }
                for(i = 0; i < 2; i++) {
                    UART_UartPutChar(' ');
                    UART_UartPutChar(GetMotionChar(hand_motion[i]));
                    itoa(hand_position[i], message, 10);
                    sprintf(message, "% 0d", hand_position[i]);
                    UART_UartPutString(message);
                }
                UART_UartPutString("  \r");
                SendUARTKB(hand_motion[0]);
            }
            UpdateCapSense();
        }
    }
}

void InitUART(void) {
    UART_Start();
    UART_Enable();
    UART_KB_Start();
    UART_KB_Enable();
}

void InitCapSense(void) {
    CapSenseP4_Start();
    CapSenseP4_InitializeAllBaselines();
}

void UpdateCapSense(void) {
    CapSenseP4_UpdateAllBaselines();
    CapSenseP4_ProcessAllWidgets();
    CapSenseP4_ScanAllWidgets();
}

void DetectHandPosition(int hand_position[2], int touch_bool[TOUCH_NUM]) {
    int hand_is_touched_left = 0;
    int hand_num = 0;
    int i;
    for(i = 0; i < 2; i++) {
        hand_position[i] = -1;
    }
    for(i = 0; i < TOUCH_NUM; i++) {
        touch_bool[i] = (CapSenseP4_IsWidgetActive(i) >> i);
        if(hand_num >= 2) {
            break;
        }
        if(!hand_is_touched_left) {
            if(touch_bool[i]) {
                hand_position[hand_num] = i;
                hand_is_touched_left = TRUE;
            }
        } else {
            if(!touch_bool[i]) {
                hand_position[hand_num] += i;
                hand_is_touched_left = FALSE;
                hand_num++;
            }
        }
        if(i == (TOUCH_NUM - 1) && touch_bool[i]) {
            hand_position[hand_num] += TOUCH_NUM;
            hand_num++;
        }

    }
    //hand_position[0] /= 2;
    //hand_position[1] /= 2;
}

int DetectHandMotion(int hand_position[2], int hand_motion[2]) {
    static int hand_position_prev[2] = {-1, -1};
    int i, j;
    int hand_moved = 0;
    
    for(i = 0; i < 2; i++) {
        if(hand_position[i] != hand_position_prev[i]){
            hand_moved = 1;
        }
        for(j = 0; j < 3; j++) {
            if(hand_position[i] == hand_position_prev[i] + motion_list[j]) {
                hand_motion[i] = motion_list[j];
                break;
            }
        }
        if(hand_position[i] == -1) {
            hand_motion[i] = MIDDLE;
        }
        hand_position_prev[i] = hand_position[i];
    }
    return hand_moved;
}

char GetMotionChar(int motion) {
    static const char motion_char[3] = {'<', '_', '>'};
    return motion_char[motion + 1];
}

void SendUARTKB(int motion) {
    static const uint8 send_data_raw[3] = {0x08, 0x00, 0x04};
    UART_KB_UartPutChar(send_data_raw[motion + 1]);
}