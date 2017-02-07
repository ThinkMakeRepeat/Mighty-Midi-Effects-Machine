//Teensy Based effects board with buttons, menu, MIDI and OLED.
#include <Arduino.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // DIYMALL OLED .96"2
//#include <ButtonEventCallback.h>
#include <PushButton.h>
#include <Bounce2.h>    // https://github.com/thomasfredericks/Bounce-Arduino-Wiring
PushButton button0 = PushButton(0, ENABLE_INTERNAL_PULLUP);
PushButton button1 = PushButton(1, ENABLE_INTERNAL_PULLUP);
//BitCrusher
int current_CrushBits = 16; //this defaults to passthrough.
int current_SampleRate = 44100; // this defaults to passthrough.

//Compressor
boolean compressorOn = false; //default:off

//Chorus
boolean chorusOn = false;

//Reverb
boolean reverbOn = false;
bool oldState = HIGH;
int showType = 0;
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=56.142845153808594,137
AudioEffectFade          fade1;          //xy=249,28
AudioEffectDelay         delay1;         //xy=248,324
AudioFilterFIR           fir1;           //xy=248.33331298828125,432.888916015625
AudioFilterStateVariable filter1;        //xy=248.4444580078125,472.111083984375
AudioEffectFlange        flange1;        //xy=250,89
AudioEffectReverb        reverb2;        //xy=250,120
AudioEffectChorus        chorus2;        //xy=252,59
AudioFilterBiquad        biquad1;        //xy=251.5555419921875,401
AudioEffectMultiply      multiply1;      //xy=254,152
AudioEffectMidSide       midside1;       //xy=254,183
AudioEffectEnvelope      envelope1;      //xy=259,247
AudioEffectBitcrusher    bitcrusher1;    //xy=261,214.71429443359375
AudioEffectBitcrusher   left_BitCrusher;
AudioEffectBitcrusher   right_BitCrusher;
AudioMixer4              mixer1;         //xy=472,235.71429443359375
AudioOutputI2S           i2s1;           //xy=852.7142333984375,139.2856903076172
AudioOutputUSB           usb1;           //xy=1066,686.1428527832031
AudioConnection          patchCord1(i2s2, 0, bitcrusher1, 0);
AudioConnection          patchCord2(bitcrusher1, 0, mixer1, 0);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=64.5,106.00000762939453
// GUItool: end automatically generated code


void setup() {
  Serial.begin(9600);
  button0.configureButton(configurePushButton);
  button1.configureButton(configurePushButton);

  // When the button is first pressed, call the function onButtonPressed (further down the page)
  button0.onPress(onButtonPressed);
  button1.onPress(onButtonPressed);
  button0.onHoldRepeat(1000, 500, onButtonHeld);
  button1.onHoldRepeat(1000, 500, onButtonHeld);

  u8x8.begin(/*Select=*/ 1, /*Right/Next=*/ 0, /*Left/Prev=*/ 3, /*Up=*/ A9, /*Down=*/ A8, /*Home/Cancel=*/ 1 && 0); // Arduboy 10 (Production)
  u8x8.setFont(u8x8_font_pxplustandynewtv_f);

  StartUpScreen(); //Start up Logo and messaging, then defaults to Effects

  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);

  // usbMIDI.setHandleProgramChange(OnProgramChange);
  // usbMIDI.setHandleNoteOn(OnNoteOn);


  mixer1.gain(0, 0.75);
  mixer1.gain(1, 0.0);
  mixer1.gain(2, 0.0);
  mixer1.gain(3, 0.0);
  // Bitcrusher
  left_BitCrusher.bits(current_CrushBits); //set the crusher to defaults. This will passthrough clean at 16,44100
  left_BitCrusher.sampleRate(current_SampleRate); //set the crusher to defaults. This will passthrough clean at 16,44100
  right_BitCrusher.bits(current_CrushBits); //set the crusher to defaults. This will passthrough clean at 16,44100
  right_BitCrusher.sampleRate(current_SampleRate); //set the crusher to defaults. This will passthrough clean at 16,44100
  //Bitcrusher
}

int mixer1_setting = 0;
elapsedMillis timeout = 0;
bool mixer2_envelope = false;

void loop() {
  button0.update();
  button1.update();
  //usbMIDI.read();
  // use the knobs to adjust parameters
  float knob1 = (float)analogRead(A8) / 1023.0;
  float knob2 = (float)analogRead(A9) / 1023.0;
}


void StartUpScreen() {
  u8x8.draw2x2String(1, 0, "Mighty");
  delay(350);
  u8x8.draw2x2String(1, 3, "MIDI");
  delay(350);
  u8x8.clear();
  u8x8.setInverseFont(1);
  u8x8.draw2x2String(1, 3, "Noise");
  delay(350);
  u8x8.clear();
  u8x8.setInverseFont(1);
  u8x8.draw2x2String(1, 3, "Maker");
  delay(350);
  u8x8.clear();
  u8x8.draw2x2String(1, 1, "THINK");
  delay(150);
  u8x8.draw2x2String(1, 3, "MAKE");
  delay(150);
  u8x8.draw2x2String(1, 5, "REPEAT");
  delay(150);
  u8x8.clear();
  u8x8.setInverseFont(0);
  u8x8.draw2x2String(0, 0, "!!!!!!!!");
  u8x8.draw2x2String(0, 2, "01010101");
  u8x8.draw2x2String(0, 4, "!!!!!!!!");
  u8x8.draw2x2String(0, 6, "01010101");
  delay(100);
  u8x8.clear();
  Menu();

  //Bitcrusher(showType);
}
void Menu() {
  u8x8.clear();
  int option;
  option = map(analogRead(A8), 0, 1023, 0, 5);
  u8x8.draw2x2String(0, 0, "MODE");
  u8x8.drawString(0, 2, "Bitcrusher");
  u8x8.drawString(0, 3, "Chorus");
  u8x8.drawString(0, 4, "Reverb");
  u8x8.drawString(0, 5, "ADSR");
  u8x8.drawString(0, 6, "Echo");
  u8x8.drawString(0, 7, "Delay");
  if (option = 5) {
    u8x8.setInverseFont(1);
    u8x8.drawString(0, 2, "Bitcrusher");
  }
  if (option =! 5){
    u8x8.setInverseFont(0);    
    }
  else {
    u8x8.setInverseFont(0);
  }
  //waveform1.frequency(360 * knob1 + 0.25);
  //waveform2.frequency(360 * knob2 + 0.25);

}
// Use this function to configure the internal Bounce object to suit you. See the documentation at: https://github.com/thomasfredericks/Bounce2/wiki
// This function can be left out if the defaults are acceptable - just don't call configureButton
void configurePushButton(Bounce& bouncedButton) {

  // Set the debounce interval to 15ms - default is 10ms
  bouncedButton.interval(15);
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void onButtonPressed(Button& btn) {
  if (btn.is(button0)) {
    u8x8.clear();
    u8x8.drawString(0, 0, "Bitcrusher");
    u8x8.drawString(0, 2, "Sample rate:");


    if (current_CrushBits >= 2) { //eachtime you press it, deduct 1 bit from the settings.
      current_CrushBits--;
      char b[5];
      String str;
      str = String(current_CrushBits);
      str.toCharArray(b, 5);
      u8x8.draw2x2String(0, 5, b);
      u8x8.drawString(4, 5, "bits");
    } else {
      current_CrushBits = 16; // if you get down to 1 go back to the top.
    }

    bitcrusher1.bits(current_CrushBits);
    bitcrusher1.sampleRate(current_SampleRate);
    Serial.print("Bitcrusher set to ");
    Serial.print(current_CrushBits);
    Serial.print(" Bit, Samplerate at ");
    Serial.print(current_SampleRate);
    Serial.println("Hz");
    Serial.println("button pressed");
    Serial.print("Button 1");
  } else if (btn.is(button1)) {
    u8x8.clear();
    u8x8.drawString(0, 0, "Bitcrusher");
    u8x8.drawString(0, 2, "Sample rate:");
    //Bitcrusher SampleRate // the lowest sensible setting is 345. There is a 128 sample buffer, and this will copy sample 1, to each of the other 127 samples.
    if (current_SampleRate >= 690) { // 345 * 2, so we can do one more divide
      current_SampleRate = current_SampleRate / 2; // half the sample rate each time
      char c[5];
      String str;
      str = String(current_SampleRate);
      str.toCharArray(c, 5);
      u8x8.draw2x2String(0, 5, c);
      u8x8.drawString(8, 5, "Hz");
    } else {
      current_SampleRate = 44100; // if you get down to the minimum then go back to the top and start over.
    }

    left_BitCrusher.bits(current_CrushBits);
    left_BitCrusher.sampleRate(current_SampleRate);
    right_BitCrusher.bits(current_CrushBits);
    right_BitCrusher.sampleRate(current_SampleRate);
    Serial.print("Bitcrusher set to ");
    Serial.print(current_CrushBits);
    Serial.print(" Bit, Samplerate at ");
    Serial.print(current_SampleRate);
    Serial.println("Hz");
    Serial.print("Button 2");
  } else {
    Serial.print("Hmmm, no button was");
  }

}

// duration reports back how long it has been since the button was originally pressed.
// repeatCount tells us how many times this function has been called by this button.
void onButtonHeld(Button& btn, uint16_t duration, uint16_t repeatCount) {
  Menu();
  Serial.print("button has been held for ");
  Serial.print(duration);
  Serial.print(" ms; this event has been fired ");
  Serial.print(repeatCount);
  Serial.println(" times");
}

// duration reports back the total time that the button was held down
void onButtonReleased(Button& btn, uint16_t duration) {

  Serial.print("button released after ");
  Serial.print(duration);
  Serial.println(" ms");
}
