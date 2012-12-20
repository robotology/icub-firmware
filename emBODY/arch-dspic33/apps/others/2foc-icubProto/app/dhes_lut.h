//   
//  dhes related tables:
//  
#ifndef __DHES_H
#define __DHES_H


// the HES activation sequence is 1,3,2,6,4,5

// note: zero is init val for "old" dhes value
// this LUT gives 0 if no turn has been done
// 1 if one positive turn has to be counted
// -1 if one negative turn has to be counted
// -2 if an invalid transition happened

                                 //old new
signed char DHES_TRANSITION_LUT[7][7] = {
 
 //to 0  1  2  3  4  5  6 
	{ 0, 0, 0, 0, 0, 0, 0}, // from 0 all ok (init val)
	{-2, 0,-2, 0,-2,-1,-2}, // from 1 (ok 1 3 5)
	{-2,-2, 0, 0,-2,-2, 0}, // from 2 (ok 2 3 6)
	{-2, 0, 0, 0,-2,-2,-2}, // from 3 (ok 3 1 2)
	{-2,-2,-2, 0, 0, 0, 0}, // from 4 (ok 4 6 5)
	{-2, 1,-2,-2, 0, 0,-2}, // from 5 (ok 5 1 4)
	{-2,-2, 0,-2, 0,-2, 0}, // from 6 (ok 6 2 4)
};

unsigned int DHES_UNALIGNEDMECANGLE_LUT[7] = {
  0,
  0x0000U/NPOLEPAIRS, // 0°
  0x5554U/NPOLEPAIRS, // 120°
  0x2AAAU/NPOLEPAIRS, // 60°
  0xAAA8U/NPOLEPAIRS, // 240°
  0xD552U/NPOLEPAIRS, // 300°
  0x7FFFU/NPOLEPAIRS, // 180°
};


/*
                            //old new
signed char DHES_TURN_LUT[7][7] = {
 //to 0  1  2  3  4  5  6 
	{ 0, 0, 0, 0, 0, 0, 0}, // from 0 
	{ 0, 0, 0, 0, 0 -1, 0}, // from 1 
	{ 0, 0, 0, 0, 0, 0, 0}, // from 2 
	{ 0, 0, 0, 0, 0, 0, 0}, // from 3 
	{ 0, 0, 0, 0, 0, 0, 0}, // from 4 
	{ 0, 1, 0, 0, 0, 0, 0}, // from 5 
	{ 0, 0, 0, 0, 0, 0, 0}, // from 6 
};
*/

/*
// fault on 0b000 and 0b111
unsigned char DHES_VALUE_LUT[8] = { 1, 0, 0, 0, 0, 0, 0, 1};	
*/
#endif

