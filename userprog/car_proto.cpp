#include <errno.h>
#include <stdio.h>
#include "car_proto.h"

#DEFINE MAX_LOAD 3
#DEFINE DEBUG_FLAG t

/**
 * A car arrives at the bridge, attempts to cross it
 * and reports on when it is crossing via DEBUG()
 * 
 * dir indicates which direction the car is going, which
 * must be a binary value (0 or 1). The car object has this
 * property and we check to ensure no problems.
 */
int OneVehicle( int dir )
{
	if( dir < 0 || dir > 1 )
	{
		errno = EINVAL;
		perror("invalid direction!");
		return -1;
	}
	else
	{
		int retval = 0;
		if( ( retval = ArriveBridge( dir ) ) != 0 )
		{
			return retval;
		}
		CrossBridge( dir ); // should not be able to fail assuming dir is 0 or 1
		else if( ( retval = ExitBridge( dir ) ) != 0 )
		{
			return retval;
		}
		else return 0;
	}
};

/**
 * A car arrives at the bridge.
 * 
 * dir: which direction is it travelling (0 or 1)
 * 
 * A car, when it arrives, checks which direction the traffic is travelling,
 * if it travelling in the correct direction OR there are exactly 0 cars
 * travelling in either direction it attempts to cross. If the direction
 * does not match and there are cars crossing it blocks.
 * 
 * Once it has confirmed it can try to cross it tests how many cars
 * are on the bridge, if it is less than 3 it can cross, it also
 * notifies a number of cars proportional to the free space
 * (ex if there is now 1 car, the current car crossing, it signals
 * two times) to the condition that matches this direction to allow
 * more cars to cross.
 * 
 * NOTE: This method can lead to starvation of the opposite side!
 * 
 * When a car has been block and then awoken it returns to the initial check,
 * of is the direction matching or are there exactly 0 cars on the bridge and
 * then blocks/continues accordingly.
 */
int ArriveBridge( int dir )
{
	DEBUG( DEBUG_FLAG, "A car has arrived going in direction: %d\n", dir );
	if( dir == 0 ) // which way am i going?
	{
		/* we check if the direction is ours and THEN if we can get on OR
			if there are even any cars on the bridge, if there are no cars
			we may cross immediately */
		DIR0_CHECK:
		if( CUR_DIR == 0 || ( DIR0_COUNT == 0 && DIR1_COUNT == 1 ) )
		{
			CUR_DIR = 0; // in case there were 0 cars crossing
			Lock->Acquire(); // lock it up
			if( DIR0_COUNT < MAX_LOAD ) // space for us on the bridge
			{
				++DIR0_COUNT;
				/* let some cars go, it does not matter if they are waiting or not 
					we hold the lock and continue execution (MESA) if someone is
					waiting they are awoken eventually to go and will recheck the cv */
				for( int i = DIR0_COUNT; i < MAX_LOAD; ++i ) CV_DIR1->Signal( CV_DIR1 );
				Lock->Release( Lock );
				return 0;
			}
			else // no room we have to wait
			{
				Lock->Release( Lock );
				CV_DIR0->Wait( CV_DIR0 );
				/* *gasp!* a GOTO to recheck the condition when someone wakes us */
				GOTO DIR0_CHECK;
			}
		}
		else // not going our way so we wait
		{
			Lock->Release( Lock );
			CV_DIR0->Wait( CV_DIR0 );
			/* *gasp!* a GOTO to recheck the condition when someone wakes us */
			GOTO DIR0_CHECK; 
		}
	}
	else if( dir == 1 ) // this is a mirror of the above code save checking against DIR1
	{
		/* we check if the direction is ours and THEN if we can get on OR
			if there are even any cars on the bridge, if there are no cars
			we may cross immediately */
		DIR1_CHECK:
		if( CUR_DIR == 1 || ( DIR0_COUNT == 0 && DIR1_COUNT == 1 ) )
		{
			CUR_DIR = 1; // in case no one was crossing
			Lock->Acquire(); // lock it up
			if( DIR1_COUNT < MAX_LOAD ) // space for us on the bridge
			{
				++DIR1_COUNT;
				/* let some cars go, it does not matter if they are waiting or not 
					we hold the lock and continue execution (MESA) if someone is
					waiting they are awoken eventually to go and will recheck the cv */
				for( int i = DIR1_COUNT; i < MAX_LOAD; ++i ) CV_DIR1->Signal( CV_DIR1 );
				Lock->Releae( Lock );
				return 0;
			}
			else // no room we have to wait
			{
				Lock->Release( Lock );
				CV_DIR1->Wait( CV_DIR1 );
				/* *gasp!* a GOTO to recheck the condition when someone wakes us */
				GOTO DIR1_CHECK;
			}
		}
		else // not going our way so we wait
		{
			Lock->Release( Lock );
			CV_DIR1->Wait( CV_DIR1 );
			/* *gasp!* a GOTO to recheck the condition when someone wakes us */
			GOTO DIR1_CHECK; 
		}
	}
	else
	{
		perror("failed to arrive at the bridge");
		errno = EINVAL;
		return -1;
	}
};

/**
 * Print a simple message of when a car is crossing
 * eg has returned from ArriveBridge()
 */
int CrossBridge( int dir )
{
	DEBUG( DEBUG_FLAG, "A car is crossing the bridge in direction: %d\n", dir );
	return 0;
};

/**
 * A car has finished crossing and runs the exit
 * procedure
 * 
 * dir: the direction of crossing.
 *
 * Once a car has returned from both arrive and cross it
 * reduces the counter corresponding to its direction.
 * If this reduces the counter to 0, eg there are now exactly
 * 0 cars on the bridge it will change the CUR_DIR to the opposite
 * side and then signal the opposite condition variable.
 * 
 * This method tries to retain some measure of fairness and reduce
 * the possibility of starvation. Though it is possible that one
 * direction is still starved.
 */
int ExitBridge( int dir )
{
	DEBUG( DEBUG_FLAG, "A car has exited going in direction: %d\n", dir );
	if( dir == 0 )
	{
		Lock->Acquire( Lock );
		--DIR0_COUNT;
		if( DIR0_COUNT == 0 ) // there are no more cars crossing currently
		{
			CUR_DIR = 1; // change the dir so other people might go
			/* it is important to NOTE: if no cars are waiting the system does not
				deadlock due to this signal being lost due to the way that
				ArriveBridge() is structured. */
			CV_DIR1->Signal( CV_DIR1 );
			/* it also important to NOTE: that we can and do release the lock NOW
			 instead of before the signal, as this is Mesa we know that we keep
			 the CPU over the guy we just woke up */
			Lock->Release( Lock );
			return 0;
		}
		else
		{
			Lock->Release( Lock );
			return 0;
		}
		return -1;
	}
	else if( dir == 1 ) // this a mirror of above
	{
		Lock->Acquire( Lock );
		--DIR1_COUNT;
		if( DIR1_COUNT == 0 ) // there are no more cars crossing currently
		{
			CUR_DIR = 0; // change the dir so other people might go
			/* it is important to NOTE: if no cars are waiting the system does not
				deadlock due to this signal being lost due to the way that
				ArriveBridge() is structured. */
			CV_DIR0->Signal( CV_DIR0 );
			/* it also important to NOTE: that we can and do release the lock NOW
			 instead of before the signal, as this is Mesa we know that we keep
			 the CPU over the guy we just woke up */
			Lock->Release( Lock );
			return 0;
		}
		else
		{
			Lock->Release( Lock );
			return 0;
		}
		return -1;
	}
	else // error
	{
		perror("problem exiting the bridge!");
		errno = EINVAL;
		return -1;
	}	
};
	