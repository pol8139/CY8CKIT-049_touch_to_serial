#include <project.h>
#include <stdlib.h>

#define TOUCH_NUM CapSenseP4_TOTAL_CSD_SENSORS

#define BUFFER_SIZE 5

#define TRUE 1
#define FALSE 0

#define LEFT -1
#define MIDDLE 0
#define RIGHT 1

void InitI2C(void);
void InitCapSense(void);
void UpdateCapSense(void);
void DetectHandPosition(int hand_position[2], int touch_bool[TOUCH_NUM]);
int DetectHandMotion(int hand_position[2], int hand_motion[2]);
void SendHandDataIfMoved(int hand_position[2], int hand_motion[2]);
void SetI2CBuffer(int touch_bool[TOUCH_NUM], int hand_motion[2]);

unsigned char ezI2C_buffer[BUFFER_SIZE] = {};

static const int motion_list[3] = {LEFT, MIDDLE, RIGHT};

int main() {
    
    CyGlobalIntEnable;      /* Enable global interrupts */
    
    InitI2C();
    InitCapSense();
    int hand_position[2];
    int touch_bool[TOUCH_NUM] = {};
    int hand_motion[2] = {MIDDLE, MIDDLE};
    while(1) {
        if(!CapSenseP4_IsBusy()) {
            DetectHandPosition(hand_position, touch_bool);
            if(DetectHandMotion(hand_position, hand_motion)) {
                SetI2CBuffer(touch_bool, hand_motion);
            }
            UpdateCapSense();
        }
    }
}

void InitI2C() {
    EZI2C_Start();
    EZI2C_EzI2CSetBuffer1(BUFFER_SIZE, 0, ezI2C_buffer);
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
        touch_bool[i] = (CapSenseP4_IsSensorActive(0, i) >> i);
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
    for(i = 0; i < 2; i++) {
        if(hand_position[i] > 0) {
            hand_position[i] /= 2;
        }
    }
}

int DetectHandMotion(int hand_position[2], int hand_motion[2]) {
    static int hand_position_prev[2] = {-1, -1};
    int i, j;
    int hand_moved = FALSE;
    for(i = 0; i < 2; i++) {
        if(hand_position[i] != hand_position_prev[i]){
            hand_moved = TRUE;
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
    }
    return hand_moved;
}

void SetI2CBuffer(int touch_bool[TOUCH_NUM], int hand_motion[2]) {
    int i;
    for(i = 0; i < BUFFER_SIZE; i++) {
        ezI2C_buffer[i] = 0;
    }
    if(hand_motion[0] == LEFT) {
        ezI2C_buffer[0] |= 0x08;
    } else if(hand_motion[0] == RIGHT) {
        ezI2C_buffer[0] |= 0x04;
    }
    if(hand_motion[1] == LEFT) {
        ezI2C_buffer[0] |= 0x02;
    } else if(hand_motion[1] == RIGHT) {
        ezI2C_buffer[0] |= 0x01;
    }
    for(i = 0; i < TOUCH_NUM; i++) {
        ezI2C_buffer[(i / 8) + 1] |= (touch_bool[i] << (7 - (i % 8)));
    }
}