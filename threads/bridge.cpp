#include "car.h"
#include "car_proto.h"
#include <list>
#include "car.cpp"
#include "car_proto.cpp"
Lock *Car_Lock;
Condition *CV_DIR0;
Condition *CV_DIR1;
Lock *CV_DIR0_LOCK;
Lock *CV_DIR1_LOCK;
int DIR0_COUNT;
int DIR1_COUNT;
int CUR_DIR;
extern void (*THREAD_RUN)(int);
 
#define CAR_COUNT 10
/* specify a number of cars on the command line
   if none are specified 10 is assumed per side */
void ThreadTest()
{
	Car_Lock = new Lock( "CAR LOCK" );
	CV_DIR0 = new Condition( "CV DIR0" );
	CV_DIR1 = new Condition( "CV DIR1" );
	DIR0_COUNT = DIR1_COUNT = 0;
	CUR_DIR = 0; // arbitrarily choose one dir to give priority to

	std::list<Car> cars_0;
	std::list<Car> cars_1;

	char car0_name[32];
	char car1_name[32];
	// NOTE cars are numbered per dir 			
	/* make some threads */
	for( int i = 0; i < CAR_COUNT; ++i )
	{
		sprintf( car0_name, "Car %d in dir %d", i, 0 );
		sprintf( car1_name, "Car %d in dir %d", i, 1 );
		cars_0.push_front( Car( 0, car0_name ) );
		cars_1.push_front( Car( 1, car1_name ) );
	}

	/* execute them, THREAD_RUN is defined in car_proto.cpp */
	for( std::list<Car>::iterator iter=cars_0.begin(); iter!=cars_0.end(); ++iter )
	{
		(*iter).Fork( OneVehicle, 0 );
	}

		
	for( std::list<Car>::iterator iter=cars_1.begin(); iter!=cars_1.end(); ++iter )
	{
		(*iter).Fork( OneVehicle, 1 );

	}
	/* thread->join()??? */
}
