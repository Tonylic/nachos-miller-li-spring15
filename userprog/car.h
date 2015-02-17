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
};