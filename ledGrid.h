#pragma once

// make sure to set these panel values to the sizes of yours
const uint8_t panelWidth = 3;
const uint8_t panelHeight = 3;

const uint16_t pixelCount = panelWidth * panelHeight;
const uint8_t pixelPin = 15;

NeoTopology<RowMajorLayout> topo(panelWidth, panelHeight);
NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang400KbpsMethod> ledGrid(pixelCount, pixelPin);

RgbColor red(128, 0, 0);
RgbColor green(0, 128, 0);
RgbColor blue(0, 0, 128);
RgbColor white(128);
RgbColor black(0);

uint16_t layoutMap(int16_t x, int16_t y) {
  return topo.MapProbe(x, y);
}

void initLEDGrid() {
	ledGrid.Begin();
	ledGrid.Show();
}

void setLED(uint16_t x, uint16_t y, RgbColor color) {
	ledGrid.SetPixelColor(topo.Map(x, y), color);
}

void setLEDGrid(uint8_t* bitmap) {
	//NeoBuffer<NeoBufferMethod<NeoGrbFeature>> image(panelWidth, panelHeight, bitmap);
	//image.Blt(ledGrid, 0, 0, layoutMap);		
}

void clearLEDGrid(RgbColor color = black) {
  ledGrid.ClearTo(color);
}

void updateLEDGrid() {
  //clearLEDGrid(green);
  
  setLED(0,0, red);
  setLED(0,2, green);
  setLED(2,0,blue);
  setLED(2,2,white);
	ledGrid.Show();
}
