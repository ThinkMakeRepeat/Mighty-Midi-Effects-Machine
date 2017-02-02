//Teensy Based effects board with buttons, menu, MIDI and OLED.
#include <Arduino.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // DIYMALL OLED .96"
#include <Bounce.h>
Bounce button0 = Bounce(0, 15); // 15 = 15 ms debounce time
Bounce button1 = Bounce(1, 15);  
//BitCrusher
int current_CrushBits = 16; //this defaults to passthrough.
int current_SampleRate = 44100; // this defaults to passthrough.

//Compressor
boolean compressorOn = false; //default:off

//Chorus
boolean chorusOn = false;

//Reverb
boolean reverbOn = false;
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
const char *string_list = 
  "Reverb\n"
  "Bitcrusher\n"
  "Chorus\n"
  "Flange\n"
  "Mid-side\n"
  "Cumulonimbus";

uint8_t current_selection = 0;


// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=57,456.71428298950195
AudioSynthNoisePink      pink1;          //xy=60,514.714282989502
AudioSynthWaveform       waveform1;      //xy=64,368.71428298950195
AudioSynthWaveform       waveform2;      //xy=69,315.71428298950195
AudioInputI2S            i2s2;           //xy=107.1428451538086,230.00000524520874
AudioSynthWaveformSineModulated sine_fm1;       //xy=207,422.71428298950195
AudioMixer4              mixer3;         //xy=327.14283370971685,245.71429634094238
AudioMixer4              mixer1;         //xy=378,383.71428298950195
AudioEffectBitcrusher    bitcrusher1;    //xy=512,342.71428298950195
AudioEffectReverb        reverb1;        //xy=560.0000228881836,185.71426582336426
AudioEffectChorus        chorus1;        //xy=560,227.1428565979004
AudioMixer4              mixer2;         //xy=707,395.71428298950195
AudioMixer4              mixer4;         //xy=779.9999999999999,201.42857142857142
AudioMixer4              mixer5;         //xy=934.2857246398926,285.71428871154785
AudioOutputI2S           i2s1;           //xy=1091.7142791748047,164.28569412231445
AudioOutputUSB           usb1;           //xy=1120.0000457763672,337.142858505249
AudioConnection          patchCord1(sine1, 0, mixer1, 2);
AudioConnection          patchCord2(pink1, 0, mixer1, 3);
AudioConnection          patchCord3(waveform1, sine_fm1);
AudioConnection          patchCord4(waveform1, 0, mixer1, 0);
AudioConnection          patchCord5(i2s2, 0, mixer3, 0);
AudioConnection          patchCord6(i2s2, 1, mixer3, 1);
AudioConnection          patchCord7(sine_fm1, 0, mixer1, 1);
AudioConnection          patchCord8(mixer3, reverb1);
AudioConnection          patchCord9(mixer3, chorus1);
AudioConnection          patchCord10(mixer1, bitcrusher1);
AudioConnection          patchCord11(bitcrusher1, 0, mixer2, 0);
AudioConnection          patchCord12(reverb1, 0, mixer4, 0);
AudioConnection          patchCord13(chorus1, 0, mixer4, 1);
AudioConnection          patchCord14(mixer2, 0, mixer5, 1);
AudioConnection          patchCord15(mixer4, 0, mixer5, 0);
AudioConnection          patchCord16(mixer5, 0, i2s1, 0);
AudioConnection          patchCord17(mixer5, 0, i2s1, 1);
AudioConnection          patchCord18(mixer5, 0, usb1, 0);
AudioConnection          patchCord19(mixer5, 0, usb1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=64.5,106.00000762939453
// GUItool: end automatically generated code

void setup() {
  Serial.begin(9600);
  u8x8.begin(/*Select=*/ 7, /*Right/Next=*/ A9, /*Left/Prev=*/ A8, /*Up=*/ A0, /*Down=*/ A3, /*Home/Cancel=*/ A5); // Arduboy 10 (Production)
  u8x8.setFont(u8x8_font_pxplustandynewtv_f);

  StartUpScreen(); //Start up Logo and messaging, then defaults to Effects

  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);

  // usbMIDI.setHandleProgramChange(OnProgramChange);
  // usbMIDI.setHandleNoteOn(OnNoteOn);

  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  mixer1.gain(0, 0.75);
  mixer1.gain(1, 0.0);
  mixer1.gain(2, 0.0);
  mixer1.gain(3, 0.0);
  mixer2.gain(0, 0.15);
  mixer2.gain(1, 0.0);
  mixer2.gain(2, 0.0);
  mixer2.gain(3, 0.0);
  mixer3.gain(0, 0.75);
  mixer3.gain(1, 0.0);
  mixer3.gain(2, 0.0);
  mixer3.gain(3, 0.0);
  mixer4.gain(0, 0.15);
  mixer4.gain(1, 0.0);
  mixer4.gain(2, 0.0);
  mixer4.gain(3, 0.0);  
  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.amplitude(0.75);
  waveform1.frequency(50);
  waveform1.pulseWidth(0.15);
  sine_fm1.frequency(440);
  sine_fm1.amplitude(0.75);
  sine1.frequency(200);
  sine1.amplitude(0.75);
  pink1.amplitude(0.75);
  //envelope1.attack(10);
  //envelope1.hold(10);
  //envelope1.decay(25);
  //envelope1.sustain(0.4);
  //envelope1.release(70);

}

int mixer1_setting = 0;
int mixer2_setting = 0;
elapsedMillis timeout = 0;
bool mixer2_envelope = false;

void loop() {
  button0.update();
  button1.update();
  //usbMIDI.read();
  uint8_t r;


  // Left changes the type of control waveform
  if (button0.fallingEdge()) {
    u8x8.clear();
  }

  // middle button switch which source we hear from mixer1
  if (button1.fallingEdge()) {
    if (mixer1_setting == 0) {
      mixer1.gain(0, 0.75);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Control oscillator");
      u8x8.clear();
      u8x8.drawString(0, 0, "Mixer 1:");
      u8x8.draw2x2String(0, 3, "Control");
      u8x8.drawString(0, 5, "OSC");



      mixer1_setting = 1;
    } else if (mixer1_setting == 1) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.75);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Frequency Modulated Oscillator");
      u8x8.clear();
      u8x8.drawString(0, 0, "Mixer 1:");
      u8x8.draw2x2String(0, 3, "FM");
      u8x8.drawString(0, 5, "OSC");
      mixer1_setting = 2;
    } else if (mixer1_setting == 2) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.75);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: ~ OSC");
      u8x8.clear();
      u8x8.drawString(0, 0, "Mixer 1:");
      u8x8.draw2x2String(0, 3, "Sine");
      u8x8.drawString(0, 5, "OSC");
      mixer1_setting = 3;
    } else if (mixer1_setting   == 3) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.75);
      Serial.println("Mixer1: @#$%^");
      u8x8.clear();
      u8x8.drawString(0, 0, "Mixer 1:");
      u8x8.draw2x2String(0, 3, "@#$%^");
      u8x8.drawString(0, 5, "OSC");
      mixer1_setting = 0;
    }
  }

  /*if (button2.fallingEdge()) {
      //Bitcrusher BitDepth
      if (current_CrushBits >= 2) { //eachtime you press it, deduct 1 bit from the settings.
          current_CrushBits--;
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
    }
  */
  /*if (button2.fallingEdge()) {
    //Bitcrusher SampleRate // the lowest sensible setting is 345. There is a 128 sample buffer, and this will copy sample 1, to each of the other 127 samples.
    if (current_SampleRate >= 690) { // 345 * 2, so we can do one more divide
      current_SampleRate = current_SampleRate / 2; // half the sample rate each time
    } else {
      current_SampleRate = 44100; // if you get down to the minimum then go back to the top and start over.
    }

    bitcrusher1.bits(current_CrushBits);
    bitcrusher1.sampleRate(current_SampleRate);

    Serial.print("Bitcrusher set to ");
    Serial.print(current_CrushBits);
    Serial.print(" Bit, Samplerate at ");
    Serial.print(current_SampleRate);
    Serial.println("Hz");
    u8x8.clear();    
    u8x8.drawString(0, 7, "Bitcrusher On");
    //u8x8.draw2x2String(0, 3, current_CrushBits);
    //u8x8.drawString(0, 5, "Hz");
  }
  */
  /*
  if (button3.fallingEdge()) {
    if (compressorOn) {
      //turn off compressor
      compressorOn = false;
    } else {
      //turn on compressor
      compressorOn = true;
    }
    Serial.print("Compressor: ");
    Serial.println(compressorOn);
    u8x8.clear();
    u8x8.drawString(0, 7, "Compressor On");

  }
  */
  // use the knobs to adjust parameters
  float knob1 = (float)analogRead(A8) / 1023.0;
  float knob2 = (float)analogRead(A9) / 1023.0;
  float knob3 = (float)analogRead(A8) / 1023.0;
  waveform1.frequency(360 * knob1 + 0.25);
  waveform2.frequency(360 * knob2 + 0.25);
  sine_fm1.frequency(knob3 * 1500 + 50);
  sine1.frequency(knob3 * 1500 + 50);
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
  u8x8.draw2x2String(1, 0, "Mode:");
  u8x8.draw2x2String(1, 3, "Effects");
  
  current_selection = u8x8.userInterfaceSelectionList(
    "0101|Effects|1010",
    current_selection, 
    string_list);

  u8x8.userInterfaceMessage(
      "Selection:", 
      u8x8_GetStringLineStart(current_selection, string_list ),
      "",
      " ok \n cancel ");
}
