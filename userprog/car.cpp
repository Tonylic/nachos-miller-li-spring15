#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../threads/thread.h"
#include "car.h"

Car::Car() : Thread( "uninit car" )
{
	dir = -1; // set dir to indicate that this is not a real car
};

/**
 * Car::Car()
 * 
 * Construct a new car with direction i, named debugname
 * cond should point to a shared condition variable, that is shared
 * 	among all cars with the same direction.
 * 	
 * lock, similar to cond, should point to a mutex that is shared
 * 	among ALL the cars.
 * 	
 * In the event a car is attempted to be made without a binary direction
 *	 errno is set and dir is set to -1 to indicate this error.
 */
Car::Car( int i, char *debugname ) : Thread( debugname )
{
	if ( i < 0 || i > 1 )
	{
		perror( "invalid direction!\n" );
		errno = EINVAL;
		dir = -1;
	}
	dir = i;
};