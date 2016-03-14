/******************************************************************
 This is the library for the Adafruit Motor Shield V2 for Arduino.
 It supports DC motors & Stepper motors with microstepping as well
 as stacking-support. It is *not* compatible with the V1 library!

 It will only work with https://www.adafruit.com/products/1483

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
/******************************************************************************/
    
/*******************************************************************************
    TO-DO
    -----
[ ] Make setSpeed functional or remove it. It should set the time interval in
    the TimerOne interruption, but then it would not make sense when the library
    is not used.
[ ] Implement another 'getSpeed' function that returns actual speed instead of 
    time interval?

*******************************************************************************/
/*

    ADD COMMENTS HERE, BIATCH !!

*/

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>
#include "SkyPointer_MotorShield.h"
#include "utility/Adafruit_PWMServoDriver.h" // It's local to this path
#ifdef __AVR__
 #define WIRE Wire
#else // Arduino Due
 #define WIRE Wire1
#endif

// Custom macro for modulus function that allows negative numbers
#define MOD(a,b) ((((a)%(b))+(b))%(b))

// Define the curve of amplitudes for the senoids to apply to the coils for 
// each microstep
#if (MICROSTEPS == 8)       //  8 microsteps per step
uint16_t microstepcurve[] = {0, 799, 1567, 2275, 2896, 3405, 3783, 4016, 4095};
#elif (MICROSTEPS == 16)    // 16 microsteps per step
uint16_t microstepcurve[] = {0, 401, 799, 1189, 1567, 1930, 2275, 2598, 2896, 3165, 3405, 3611, 3783, 3919, 4016, 4075, 4095};
#endif

/******************************************************************************/
SkyPointer_MotorShield::SkyPointer_MotorShield(uint8_t addr) {
    // Constructor for the class
    _addr = addr;
    _pwm = Adafruit_PWMServoDriver(_addr);
}
/******************************************************************************/
void SkyPointer_MotorShield::begin(uint16_t freq) {
    // Initializes the board with a PWM frequency of freq
    WIRE.begin();
    _pwm.begin();
    _freq = freq;
    _pwm.setPWMFreq(_freq);  // This is the maximum PWM frequency
    for (uint8_t i=0; i<16; i++)
    _pwm.setPWM(i, 0, 0);
<<<<<<< HEAD

    // Initialize times
    laser_t_on = 0;
=======
>>>>>>> 6fab89c2bd3988511d810b3c66bb502626479e75
}
/******************************************************************************/
void SkyPointer_MotorShield::setPWM(uint8_t pin, uint16_t value) {
    // Sets PWM values for the coils excitation
    if (value > 4095) {
    _pwm.setPWM(pin, 4096, 0);
    } else
    _pwm.setPWM(pin, 0, value);
}
/******************************************************************************/
void SkyPointer_MotorShield::setPin(uint8_t pin, boolean value) {
    // Sets amplitude values for the coils excitation
    if (value == LOW)
    _pwm.setPWM(pin, 0, 0);
    else
    _pwm.setPWM(pin, 4096, 0);
}
<<<<<<< HEAD


void SkyPointer_MotorShield::setTimeOn(uint32_t t){
    laser_t_on = t;
}


uint32_t SkyPointer_MotorShield::getTimeOn() {
    return laser_t_on;
}
=======
>>>>>>> 6fab89c2bd3988511d810b3c66bb502626479e75
/*******************************************************************************
    ###  Class for the MicroStepper motor  ###
*******************************************************************************/
SkyPointer_MicroStepper::SkyPointer_MicroStepper (void) {
    // Constructor for the MicroStepper class
    // Initializes to zero
    target = 0;           // Value for target position
    microstepsPerRev = 0; // Steps per rev times MICROSTEPS
    microsteppernum = 0;  // Identifier for the motor, {1, 2}
    currMicrostep = 0;    // Current microstep in the range [0, MICROSTEPS]
    currPos = 0;          // Current position in microsteps in the revolution
}
/******************************************************************************/
SkyPointer_MicroStepper *SkyPointer_MotorShield::getMicroStepper (uint16_t steps, uint8_t num) {
    /*
    Defines a stepper motor in microstep rotation mode.
        steps --> Number of steps per revolution
        num   --> Number assigned to the motor = {1, 2}
    */
    if (num > 2) return NULL; // Motor number cannot be greater than 2 !!
    num--;  // Changes the number of the motor to fit the range {0, 1}

    if (microsteppers[num].microsteppernum == 0) {
        microsteppers[num].microsteppernum = num; // Number of the motor
        microsteppers[num].microstepsPerRev = steps*MICROSTEPS; // Number of
                   // microsteps per rev; this could be changed to customize the
                   // number of microsteps per step by using a variable         
        microsteppers[num].MC = this;
        // Variables for the PWM control
        uint8_t pwma, pwmb, ain1, ain2, bin1, bin2;
        
        // Configuration of the motor's pins
        if (num == 0) {
            pwma = 8;   ain2 = 9;   ain1 = 10;
            pwmb = 13;  bin2 = 12;  bin1 = 11;
        } else if (num == 1) {
            pwma = 2;   ain2 = 3;   ain1 = 4;
            pwmb = 7;   bin2 = 6;   bin1 = 5;
        }
        microsteppers[num].PWMApin = pwma;
        microsteppers[num].PWMBpin = pwmb;
        microsteppers[num].AIN1pin = ain1;
        microsteppers[num].AIN2pin = ain2;
        microsteppers[num].BIN1pin = bin1;
        microsteppers[num].BIN2pin = bin2;
    }
    return &microsteppers[num];
}
/******************************************************************************/
void SkyPointer_MicroStepper::setSpeed (float rpm) {
    // This function calculates the value that must be loaded in
    // Timer1 register for interrupts to occur at needed intervals for producing
    // the desired speed
    
    // Currently it does nothing but calculate the time interval in microseconds
    // between two microstep rotations in order to get the desired speed.
    // Time interval still needs to be entered by hand in the Timer1 interruption
    // definition.
    usecPerMicrostep = (uint32_t)(60000000L / (microstepsPerRev * rpm));
}
/******************************************************************************/
uint32_t SkyPointer_MicroStepper::getSpeed() {
    // Returns time interval in microsecons between two microstep rotations.
    return usecPerMicrostep;
}
/******************************************************************************/
void SkyPointer_MicroStepper::release(void) {
    // Releases the motor, removing voltage from the coils. Free rotation.
    MC->setPin(AIN1pin, LOW);
    MC->setPin(AIN2pin, LOW);
    MC->setPin(BIN1pin, LOW);
    MC->setPin(BIN2pin, LOW);
    MC->setPWM(PWMApin, 0);
    MC->setPWM(PWMBpin, 0);
}
/******************************************************************************/
void SkyPointer_MicroStepper::setTarget (uint16_t _target) {
    // Sets the value for the target, in ABSOLUTE microsteps
    target = _target;
}
/******************************************************************************/
boolean SkyPointer_MicroStepper::isTarget() {
    // Checks if currPos == target and returns 1 if True or 0 otherwise
    return (currPos == target);
}
/******************************************************************************/
uint16_t SkyPointer_MicroStepper::microstep (uint16_t usteps, uint8_t dir) {
    /* Produces a rotation of 'usteps' microsteps in the given direction.
            usteps --> Number of microsteps to rotate
            dir    --> Direction of the rotation = {FORWARD, BACKWARDS}
    */
    // Variables for the values to be loaded to the PWM registers
    uint16_t ocra, ocrb; // ocra, ocrb = [0, 4095] for the IC in the shield
    
    // Update current position of the rotor
    if (dir == FORWARD) {   // Forward rotation
        currMicrostep++;
        currPos++;
        //  TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // Make sure currPos is in range
        currPos += microstepsPerRev;
        currPos %= microstepsPerRev;
    }
    else {
    
    // Can this be improved ????????????????????????????????????????????????????
        
        if (currMicrostep == 0) {
            currMicrostep = 4*MICROSTEPS - 1; // Deals with negative values
        }
        else {
            currMicrostep--;    // Backwards rotation
        }
        if (currPos == 0) {
            currPos = microstepsPerRev - 1; // Deals with negative values
        }
        else {
            currPos--;          // Update position
        }
        //  TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
        // Make sure currPos is in range
        currPos += microstepsPerRev;        
        currPos %= microstepsPerRev;
    }
    // Make sure that currMicrostep is in the right range: [0, 4*MICROSTEPS]
    currMicrostep += 4*MICROSTEPS;
    currMicrostep %= 4*MICROSTEPS;
    
    // Initialize values of PWM registers
    ocra = ocrb = 0;
    // Check in which microstep the rotor is so it applies the right torque
    if ((currMicrostep >= 0) && (currMicrostep < MICROSTEPS)) {
        // Assign values to PWM
        ocra = microstepcurve[MICROSTEPS - currMicrostep];
        ocrb = microstepcurve[currMicrostep];
    }
    else if ((currMicrostep >= MICROSTEPS) && (currMicrostep < 2*MICROSTEPS)) {
        // Assign values to PWM
        ocra = microstepcurve[currMicrostep - MICROSTEPS];
        ocrb = microstepcurve[2*MICROSTEPS - currMicrostep];    
    }
    else if ((currMicrostep >= 2*MICROSTEPS) && (currMicrostep < 3*MICROSTEPS)) {
        // Assign values to PWM
        ocra = microstepcurve[3*MICROSTEPS - currMicrostep];
        ocrb = microstepcurve[currMicrostep - 2*MICROSTEPS];     
    }
    else if ((currMicrostep >= 3*MICROSTEPS) && (currMicrostep < 4*MICROSTEPS)) {
        // Assign values to PWM
        ocra = microstepcurve[currMicrostep - 3*MICROSTEPS];
        ocrb = microstepcurve[4*MICROSTEPS - currMicrostep];
    }
    
    // Load values into PWM registers
    MC -> setPWM(PWMApin, ocra);
    MC -> setPWM(PWMBpin, ocrb);
    
    // Latch state for deciding which coils to excite
    uint8_t latch_state = 0;    // State variable
    
    if ((currMicrostep >= 0) && (currMicrostep < MICROSTEPS)) {
        latch_state |= 0x03;    // b'0011'
    }
    else if ((currMicrostep >= MICROSTEPS) && (currMicrostep < 2*MICROSTEPS)) {
        latch_state |= 0x06;    // b'0110'
    }
    else if ((currMicrostep >= 2*MICROSTEPS) && (currMicrostep < 3*MICROSTEPS)) {
        latch_state |= 0x0C;    // b'1100'
    }
    else if ((currMicrostep >= 3*MICROSTEPS) && (currMicrostep < 4*MICROSTEPS)) {
        latch_state |= 0x09;    // b'1001'
    }
    
    // Excitation of the coils
    // Check if pin BIN1 must be HIGH or LOW
    if (latch_state & 0x01) {
        MC -> setPin(AIN2pin, HIGH);
    }
    else {
        MC -> setPin(AIN2pin, LOW);
    }
    // Check if pin BIN1 must be HIGH or LOW
    if (latch_state & 0x02) {
        MC -> setPin(BIN1pin, HIGH);
    }
    else {
        MC -> setPin(BIN1pin, LOW);
    }
    // Check if pin AIN1 must be HIGH or LOW
    if (latch_state & 0x04) {
        MC -> setPin(AIN1pin, HIGH);
    }
    else {
        MC -> setPin(AIN1pin, LOW);
    }
    // Check if pin BIN2 must be HIGH or LOW
    if (latch_state & 0x08) {
        MC -> setPin(BIN2pin, HIGH);
    }
    else {
        MC -> setPin(BIN2pin, LOW);
    }
    
    return currPos;
}
/******************************************************************************/
uint16_t SkyPointer_MicroStepper::getPosition() {
    // Returns the current ABSOLUTE position of the motor.
    return currPos;
}
/******************************************************************************/



/*******************************************************************************
THIS PART OF THE CODE IS FOR NORMAL (ORIGINAL) MODE OF THE MOTOR, NOT MICROSTEP.
*******************************************************************************/
/*
SkyPointer_StepperMotor *SkyPointer_MotorShield::getStepper(uint16_t steps, uint8_t num) {
  if (num > 2) return NULL;

  num--;

  if (steppers[num].steppernum == 0) {
    // not init'd yet!
    steppers[num].steppernum = num;
    steppers[num].revsteps = steps;
    steppers[num].MC = this;
    uint8_t pwma, pwmb, ain1, ain2, bin1, bin2;
    if (num == 0) {
      pwma = 8;  ain2 = 9;  ain1 = 10;
      pwmb = 13; bin2 = 12; bin1 = 11;
    } else if (num == 1) {
      pwma = 2; ain2 = 3; ain1 = 4;
      pwmb = 7; bin2 = 6; bin1 = 5;
    }
    steppers[num].PWMApin = pwma;
    steppers[num].PWMBpin = pwmb;
    steppers[num].AIN1pin = ain1;
    steppers[num].AIN2pin = ain2;
    steppers[num].BIN1pin = bin1;
    steppers[num].BIN2pin = bin2;
  }
  return &steppers[num];
}
*/
/******************************************************************************/
/*
SkyPointer_StepperMotor::SkyPointer_StepperMotor(void) {
  revsteps = steppernum = currentstep = 0;
}
*/
/******************************************************************************/
/*
void SkyPointer_StepperMotor::setSpeed(uint16_t rpm) {
  //Serial.println("steps per rev: "); Serial.println(revsteps);
  //Serial.println("RPM: "); Serial.println(rpm);

  steppingcounter = 0;
}
*/
/******************************************************************************/
/*
void SkyPointer_StepperMotor::release(void) {
  MC->setPin(AIN1pin, LOW);
  MC->setPin(AIN2pin, LOW);
  MC->setPin(BIN1pin, LOW);
  MC->setPin(BIN2pin, LOW);
  MC->setPWM(PWMApin, 0);
  MC->setPWM(PWMBpin, 0);
}
*/
/******************************************************************************/
/*
void SkyPointer_StepperMotor::microstep(uint16_t usteps, uint8_t dir) {
  uint32_t uspers = usperstep;
  uint8_t ret = 0;

  uspers /= MICROSTEPS;

  while (usteps--) {
    ret = onestep(dir, MICROSTEP);
    delay(uspers);
    steppingcounter += (uspers % 1000);
    if (steppingcounter >= 1000) {
      delay(1);
      steppingcounter -= 1000;
    }
  }
}
*/

/*
void SkyPointer_StepperMotor::step(uint16_t steps, uint8_t dir,  uint8_t style) {
  uint32_t uspers = usperstep;
  uint8_t ret = 0;

  if (style == INTERLEAVE) {
    uspers /= 2;
  }
 else if (style == MICROSTEP) {
    uspers /= MICROSTEPS;
    steps *= MICROSTEPS;
#ifdef MOTORDEBUG
    Serial.print("steps = "); Serial.println(steps, DEC);
#endif
  }

  while (steps--) {
    //Serial.println("step!"); Serial.println(uspers);
    ret = onestep(dir, style);
    delay(uspers/1000); // in ms
    steppingcounter += (uspers % 1000);
    if (steppingcounter >= 1000) {
      delay(1);
      steppingcounter -= 1000;
    }
  }
  if (style == MICROSTEP) {
    while ((ret != 0) && (ret != MICROSTEPS)) {
      ret = onestep(dir, style);
      delay(uspers/1000); // in ms
      steppingcounter += (uspers % 1000);
      if (steppingcounter >= 1000) {
	delay(1);
	steppingcounter -= 1000;
      }
    }
  }
}

uint8_t SkyPointer_StepperMotor::onestep(uint8_t dir, uint8_t style) {
  uint8_t a, b, c, d;
  uint8_t ocrb, ocra;

  ocra = ocrb = 255;


  // next determine what sort of stepping procedure we're up to
  if (style == SINGLE) {
    if ((currentstep/(MICROSTEPS/2)) % 2) { // we're at an odd step, weird
      if (dir == FORWARD) {
	currentstep += MICROSTEPS/2;
      }
      else {
	currentstep -= MICROSTEPS/2;
      }
    } else {           // go to the next even step
      if (dir == FORWARD) {
	currentstep += MICROSTEPS;
      }
      else {
	currentstep -= MICROSTEPS;
      }
    }
  } else if (style == DOUBLE) {
    if (! (currentstep/(MICROSTEPS/2) % 2)) { // we're at an even step, weird
      if (dir == FORWARD) {
	currentstep += MICROSTEPS/2;
      } else {
	currentstep -= MICROSTEPS/2;
      }
    } else {           // go to the next odd step
      if (dir == FORWARD) {
	currentstep += MICROSTEPS;
      } else {
	currentstep -= MICROSTEPS;
      }
    }
  } else if (style == INTERLEAVE) {
    if (dir == FORWARD) {
       currentstep += MICROSTEPS/2;
    } else {
       currentstep -= MICROSTEPS/2;
    }
  }

  if (style == MICROSTEP) {
    if (dir == FORWARD) {
      currentstep++;
    } else {
      // BACKWARDS
      currentstep--;
    }

    currentstep += MICROSTEPS*4;
    currentstep %= MICROSTEPS*4;

    ocra = ocrb = 0;
    if ( (currentstep >= 0) && (currentstep < MICROSTEPS)) {
      ocra = microstepcurve[MICROSTEPS - currentstep];
      ocrb = microstepcurve[currentstep];
    } else if  ( (currentstep >= MICROSTEPS) && (currentstep < MICROSTEPS*2)) {
      ocra = microstepcurve[currentstep - MICROSTEPS];
      ocrb = microstepcurve[MICROSTEPS*2 - currentstep];
    } else if  ( (currentstep >= MICROSTEPS*2) && (currentstep < MICROSTEPS*3)) {
      ocra = microstepcurve[MICROSTEPS*3 - currentstep];
      ocrb = microstepcurve[currentstep - MICROSTEPS*2];
    } else if  ( (currentstep >= MICROSTEPS*3) && (currentstep < MICROSTEPS*4)) {
      ocra = microstepcurve[currentstep - MICROSTEPS*3];
      ocrb = microstepcurve[MICROSTEPS*4 - currentstep];
    }
  }

  currentstep += MICROSTEPS*4;
  currentstep %= MICROSTEPS*4;

#ifdef MOTORDEBUG
  //Serial.print("current step: "); Serial.println(currentstep, DEC);
  //Serial.print(" pwmA = "); Serial.print(ocra, DEC);
  //Serial.print(" pwmB = "); Serial.println(ocrb, DEC);
#endif
  MC->setPWM(PWMApin, ocra*16);
  MC->setPWM(PWMBpin, ocrb*16);

  // release all
  uint8_t latch_state = 0; // all motor pins to 0

  //Serial.println(step, DEC);
  if (style == MICROSTEP) {
    if ((currentstep >= 0) && (currentstep < MICROSTEPS))
      latch_state |= 0x03;
    if ((currentstep >= MICROSTEPS) && (currentstep < MICROSTEPS*2))
      latch_state |= 0x06;
    if ((currentstep >= MICROSTEPS*2) && (currentstep < MICROSTEPS*3))
      latch_state |= 0x0C;
    if ((currentstep >= MICROSTEPS*3) && (currentstep < MICROSTEPS*4))
      latch_state |= 0x09;
  } else {
    switch (currentstep/(MICROSTEPS/2)) {
    case 0:
      latch_state |= 0x1; // energize coil 1 only
      break;
    case 1:
      latch_state |= 0x3; // energize coil 1+2
      break;
    case 2:
      latch_state |= 0x2; // energize coil 2 only
      break;
    case 3:
      latch_state |= 0x6; // energize coil 2+3
      break;
    case 4:
      latch_state |= 0x4; // energize coil 3 only
      break;
    case 5:
      latch_state |= 0xC; // energize coil 3+4
      break;
    case 6:
      latch_state |= 0x8; // energize coil 4 only
      break;
    case 7:
      latch_state |= 0x9; // energize coil 1+4
      break;
    }
  }
#ifdef MOTORDEBUG
  //Serial.print("Latch: 0x"); Serial.println(latch_state, HEX);
#endif

  if (latch_state & 0x1) {
   // Serial.println(AIN2pin);
    MC->setPin(AIN2pin, HIGH);
  } else {
    MC->setPin(AIN2pin, LOW);
  }
  if (latch_state & 0x2) {
    MC->setPin(BIN1pin, HIGH);
   // Serial.println(BIN1pin);
  } else {
    MC->setPin(BIN1pin, LOW);
  }
  if (latch_state & 0x4) {
    MC->setPin(AIN1pin, HIGH);
   // Serial.println(AIN1pin);
  } else {
    MC->setPin(AIN1pin, LOW);
  }
  if (latch_state & 0x8) {
    MC->setPin(BIN2pin, HIGH);
   // Serial.println(BIN2pin);
  } else {
    MC->setPin(BIN2pin, LOW);
  }

  return currentstep;
}

*/

