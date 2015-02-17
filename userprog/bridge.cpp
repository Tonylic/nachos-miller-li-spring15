#include "car.h"
#include "car_proto.h"

Lock *Car_Lock;
Condition *CV_DIR0;
Condition *CV_DIR1;
int DIR0_COUNT;
int DIR1_COUNT;
int CUR_DIR;

#define DEFAULT_CAR_COUNT 10
/* specify a number of cars on the command line
	if none are specified 10 is assumed per side */
int main( int argc, char **argv )
{
	int car_count;
	if( argc != 2 )
	{
		car_count = DEFAULT_CAR_COUNT;
	}
	else 
	{
		car_count = atoi( argv[1] ); // strtol() would be better...
	}
	
	Car_Lock = new Lock( "CAR LOCK" );
	CV_DIR0 = new Condition( "CV DIR0" );
	CV_DIR1 = new Condition( "CV DIR1" );
	DIR0_COUNT = DIR1_COUNT = 0;
	CUR_DIR = 0; // arbitrarily choose one dir to give priority to
	
	Car cars_0[ car_count ];
	Car cars_1[ car_count ];
	
	/* make some threads */
	for( int i = 0; i < car_count; ++i )
	{
		char car0_name[32];
		char car1_name[32];
		// NOTE cars are numbered per dir
		sprintf( car0_name, "CAR #%d - DIR: %d", i, 0 );
		sprintf( car1_name, "CAR #%d - DIR: %d", i, 1 );
		cars_0[i] = new Car( car0_name, 0 );
		cars_1[i] = new Car( car1_name, 1 );
	}
	
	/* execute them, THREAD_RUN is defined in car_proto.cpp */
	for( int i = 0; i < car_count; ++i )
	{
		cars_0->Fork( THREAD_RUN, 0 );
		cars_1->Fork( THREAD_RUN, 1 );
	}
	
	/* thread->join()??? */
}