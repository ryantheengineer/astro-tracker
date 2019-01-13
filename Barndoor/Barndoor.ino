/*
  Barn Door Driver
  Written by Theron Wierenga
  July 18, 2014
  */

// Include the library code for LiquidCrystal display
#include <LiquidCrystal.h>
// Initialize the LiquidCrystal library with 
// the numbers of the interface pins
// needed for SainSmart LCD Keypad Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// 4 bit stepper motor patterns for full step and half step
byte fullsteps[4] = {0x06, 0x0A, 0x09, 0x05};
byte halfsteps[8] = {0x04, 0x06, 0x02, 0x0A, 0x08, 0x09, 0x01, 0x05};
// Sidereal conversion
float sidereal_conv = 1.0/0.99726958;
// Angle in radians for three hours, or 45 degrees 
float three_hour_angle = 3.1415926 / 4.0;
// PI
float pi = 3.1415926;
// r value, distange from hinge to threaded rod
float r = 11.5;
// Solar time in seconds
float solar_time = 0;
// Delay value used to only change LCD readings about every half second
long lcount = 0L;
// Sidereal time in seconds
float sidereal = 0;
// Angle theta made by two boards
float theta = 0;
// Angle theta in degress for easy reading on LCD
float degangle = 0;
// Angle psi, the base angles of the triangle
float psi = 0;
// The complement of psi, 90 deg - psi or pi/2 radians - psi
float comp_psi = 0;
// The correction distance
float correction = 0;
// The value r after correction is applied
float rc = 0;
// d value, the base of the triangle
float d = 0;
// Number of steps needed to raise d,
// so that boards make an angle of theta
float steps = 0;
// Number of steps as a long integer
long isteps = 0L;
// Last value of isteps
long last_isteps = 0L;
// Up = true, down = false
boolean up = true;
// Slow (sideral rate) = true, fast slew = false
boolean slow = true;
// Value use to keep track of which step value to use next
// Used in fullstep_motor() a halfstep_motor() functions
int step_count = 0;
// Analog value used to read switches
int temp = 0;


void setup() {
  // Set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Starting");
  delay(1500);
  // Declare port C pins as outputs
  DDRC = B11111111;
  // Set port C output pins to all zero
  PORTC = B00000000;
}

void loop() {
  if (slow)
  {
      // Get the solar time in seconds
      solar_time = float(millis())/1000.0;
      // Convert solar time to sidereal time
      sidereal = solar_time * sidereal_conv;
      // Advance lcount, each 400 times through the loop it 
      // will update the LCD display
      lcount = lcount + 1L;
      if (lcount == 400)
      {
        // Set the cursor to column 0, line 1
        // Line 1 is the second row, since counting begins with 0
        lcd.setCursor(0,0);
        // Display sidereal time in seconds
        lcd.print("Sid'eal="); lcd.print(sidereal);
        lcd.setCursor(0, 1);
        // Display number of steps
        lcd.print(isteps);
        lcd.setCursor(10, 1);
        // Display the angle made by the two boards in degrees
        lcd.print(degangle);
        // Reset the counter
        lcount = 0;
      }
      // Calculate the angle theta needed at this time
      theta = (sidereal / 10800.0) * three_hour_angle;
      // Do a safety check if left unattended
      // Stop after angle = three_hour_angle or 45 degrees
      // About as high as it goes with 12 inch threaded rod
      if (theta >= three_hour_angle)
      {
        // Turn off current to all motor windings
         PORTC = 0;
         while(true)
         {
            // do nothing, reset to restart)
         }
      }
      // Convert this angle from radians to degrees 
      // for easy reading on the LCD display
      degangle = theta * 180.0/ pi;
      // Calculate the angle psi, knowing theta
      psi = (pi - theta) / 2.0;
      // Calculate the complement of psi
      comp_psi = (pi / 2.0) - psi;
      // Calculate the correction distance
      correction = 1.25 * tan(comp_psi);
      // Apply the corection value to r
      rc = r - correction;
      // Calculate d, the distance the threaded rod needs to travel
      d = (rc *sin(theta)) / sin(psi);
      // Calculate the number of steps needed to 
      // have the threaded rod travel distance d
      // Change 200.0 to 400.0 if using halfstep_motor()
      steps = d * 28.0 * 200.0;
      // Convert steps to a long integer
      isteps = long(steps + 0.5);
      // We need to add a step, or more, to have isteps correct
      while (last_isteps < isteps)
      {
         // halfstep_motor() can be used here, 
         // steps formula will need changing above
         fullstep_motor(up);
         last_isteps = last_isteps + 1L;
      }
  }
  else
  {
       // If slow = false, then slew fast
       // halfstep_motor() can be used here, 
       // steps formula will need changing above
       fullstep_motor(up);
       // Let stepper motor settle, this may need to be adjusted
       // for other stepper motors to get maximum speed 
       delay(5); 
  }
  // Read switches
  temp = analogRead(0);

  //if (temp > 1000) {}                               // LCD board, no button pushed
  if ((temp > 710) && (temp < 750)) {slow = true;}    // LCD board, Select pushed, run at sideral rate
  if ((temp > 490) && (temp < 510)) {slow = false;}   // LCD board, Left pushed,   run at slew rate
  if ((temp > 320) && (temp < 340)) {up = false;}     // LCD board, Down pushed,   run down
  if ((temp > 140) && (temp < 160)) {up = true;}      // LCD board, Up pushed,     run up
  //if (temp < 30) {}                                 // LCD board, Right pushed


}

void halfstep_motor(boolean forward)
{
  switch (step_count)
  {
     case 0: PORTC = halfsteps[0];
             break; 
     case 1: PORTC = halfsteps[1];
             break; 
     case 2: PORTC = halfsteps[2];
             break; 
     case 3: PORTC = halfsteps[3];
             break; 
     case 4: PORTC = halfsteps[4];
             break; 
     case 5: PORTC = halfsteps[5];
             break; 
     case 6: PORTC = halfsteps[6];
             break; 
     case 7: PORTC = halfsteps[7];
             break; 
  }
  if (forward)
  {
    step_count = step_count + 1;
    if (step_count > 7)
    { 
       step_count = 0;
    }
    return; 
  }
  else
  {
    step_count = step_count - 1; 
    if (step_count < 0)
    {
       step_count = 7; 
    }
    return;
  }
}

void fullstep_motor(boolean forward)
{
  switch (step_count)
  {
     case 0: PORTC = fullsteps[0];
             break; 
     case 1: PORTC = fullsteps[1];
             break; 
     case 2: PORTC = fullsteps[2];
             break; 
     case 3: PORTC = fullsteps[3];
             break; 
  }
  if (forward)
  {
    step_count = step_count + 1;
    if (step_count > 3)
    { 
       step_count = 0;
    }
    return; 
  }
  else
  {
    step_count = step_count - 1; 
    if (step_count < 0)
    {
       step_count = 3; 
    }
    return;
  }  
}
