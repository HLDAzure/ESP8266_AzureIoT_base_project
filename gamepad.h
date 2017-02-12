#include <Button.h>
#include "EasyButton.h"
#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>

Button upButton = Button(0);
Button downButton = Button(2);
Button leftButton = Button(14);
Button rightButton = Button(12);
Button aButton(16);
Button bButton(13);

void initGamepad() {
  //initialize the buttons
  upButton.begin();
  downButton.begin();
  leftButton.begin();
  rightButton.begin();
  aButton.begin();
  bButton.begin();

  //initialize the LEDdisplay

  //initialize the LightSensor

}


void updateButtons() {
	if (upButton.pressed())
		Serial.println("Up button pressed");
	if (downButton.pressed())
		Serial.println("Down button pressed");
	if (leftButton.pressed())
		Serial.println("Left button pressed");
	if (rightButton.pressed())
		Serial.println("Right button pressed");

	if (upButton.released())
		Serial.println("Up button released");
	if (downButton.released())
		Serial.println("Down button released");
	if (leftButton.released())
		Serial.println("Left button released");
	if (rightButton.released())
		Serial.println("Right button released");

	if (aButton.toggled()) {
		if (aButton.read() == Button::PRESSED)
			Serial.println("A button has been pressed");
		else
			Serial.println("A button has been released");
	}
  if (bButton.toggled()) {
		if (bButton.read() == Button::PRESSED)
			Serial.println("B button has been pressed");
		else
			Serial.println("B button has been released");
	}

}
