#ifndef NACHOS_CAR_H
#define NACHOS_CAR_H

/**
 * FILE: car.h
 * Prototype for the car thread.
 */

#include "../threads/thread.h"
#include "../threads/synch.h"

class Car : public Thread {
	public:
		Car();
		Car( int, char* ); // we will just use the default priority
		int dir;
		void Fork( VoidFunctionPtr func, int arg );
};

#endif
