#include "../threads/synch.h"
extern Lock *Car_Lock;
extern Condition *CV_DIR0;
extern Condition *CV_DIR1;
extern int DIR0_COUNT;
extern int DIR1_COUNT;
extern int CUR_DIR;

int OneVehicle( int );
int ArriveBridge( int );
int CrossBridge( int );
int ExitBridge( int );