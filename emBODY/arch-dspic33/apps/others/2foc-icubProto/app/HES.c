//
// Hall Effect sensor related routines
//

#include "HES.h"

void ForceHESEmulation()
//
// Permits to switch 'at the fly' between Absolute encoder and and Hall Effect Emmulation:
//
{
  if (UseHES!=0)
  {
    unsigned HESUVW;
    SFRAC16  Theta;

    HESUVW = (PORTB & 0b011100000) >> 5;
    switch(HESUVW)
    {
      case 1://case 1: 
        Theta = Q15( 0.0000000); 
      break;
      case 2://case 2: 
        Theta = Q15( 0.66666666); 
      break;
      case 3://case 3: 
        Theta = Q15( 0.3333333); 
      break;
      case 4://case 4: 
        Theta = Q15(-0.6666666); 
      break;
      case 5://case 5: 
        Theta = Q15(-0.3333333); 
      break;
      case 6://case 6: 
        Theta = Q15(-0.9999999); 
      break;
      default:
        // 000 and 111 are invalid values 
        SysError.HESInvalidValue=1;
        // TODO: position can be estimated using last Theta and current velocity
        Theta = 0;
        // stop the motor
        CtrlReferences.qIqRef = 0;
        TorqueTogglingReference = 0;
        OmegaTogglingReference = 0;
      break;
    }
    ParkParm.qAngle = Theta;
  } 
}

void DrawHallEffectAngle()
// this routine permits to draw the angle resulting from Hall Sensor reading
// in order to be compared to a more resolute encoder 
{
  switch(((PORTB & 0b011100000) >> 5))
  {
    case 5: Gulp.W[3] = Q15( 0.1666666); // 0°
    break;
    case 1: Gulp.W[3] = Q15( 0.4999999); // 60°
    break;
    case 3: Gulp.W[3] = Q15( 0.8333333); // 120°
    break;
    case 2: Gulp.W[3] = Q15(-0.8333333); // 180°
    break;
    case 6: Gulp.W[3] = Q15(-0.4999999); // 240°
    break;
    case 4: Gulp.W[3] = Q15(-0.1666666); // 300°
    break;
  }
}
