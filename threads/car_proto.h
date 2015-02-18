#ifndef NACHOS_CAR_PROTO_H
#define NACHOS_CAR_PROTO_H

#include "synch.h"
extern Lock *Car_Lock;
extern Condition *CV_DIR0; // blocks one dir
extern Condition *CV_DIR1; // blocks the other dir
extern Lock *CV_DIR1_LOCK; // lcck for the cv
extern Lock *CV_DIR0_LOCK; // lock for other cv
extern int DIR0_COUNT; // counts cars on the bridge
extern int DIR1_COUNT; // counts cars on the bridge (the other way)
extern int CUR_DIR; // indicates which direction traffic is going
extern void (*THREAD_RUN)(int); // func pointer for cars to use when the run

void OneVehicle( int );
int ArriveBridge( int );
int CrossBridge( int );
int ExitBridge( int );

#endif
