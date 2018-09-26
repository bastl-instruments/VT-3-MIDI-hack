/*
 * I always wanted to sing but i could not because i could hear how much out of tune my voice was. 
 * I finally learned how to sing. Well I am cyborg now... and I dont care...
 * 
 * Václav Peloušek 2018
 * cc-by-sa
 * bastl-instruments.com
 * https://soundcloud.com/toyota-vangelis
 * 
 * MIDI hack for Roland VT-3 voice changer
 * adds MIDI control for PITCH fader by MIDI notes on channel 1
 * and for FORMANT fader by MIDI CC no 1 - the mod wheel
 * 
 * bypasses faders by microcontroller that outputs voltages
 * that way it keeps the fader functionaly if MIDI not used
 * 
 * all documentation is on github
 * https://github.com/bastl-instruments/VT-3-MIDI-hack 
 * 
 * upload firmware to Attiny 85 using this tutorial
 * http://highlowtech.org/?p=1695
 * do not forget to “burn bootloader” to make it run at 8MHz internal clock
 * 
 * I do NOT take any responsibility of YOU breaking your instruments 
 * and I do NOT provide any warranty
 * and I do NOT provide any technical support. 
 * use it well to make the world a better place
 * Peace
 */
#include <SoftwareSerial.h>
#include <MIDI.h>
#include "MIDInoteBuffer.h"
#define MIDI_IN 2
#define MIDI_OUT 9

//create instance of serial port
SoftwareSerial softSerial(MIDI_IN, MIDI_OUT);

//external pointers for midi buffer library
uint8_t smallMidiBuffer[1][16];
uint8_t smallVelocityBuffer[1][16];
uint8_t smallWindowBuffer[1][16];

//create  instance of note buffer
MIDInoteBuffer buffer[1] = {
  {&smallMidiBuffer[0][0], &smallVelocityBuffer[0][0], &smallWindowBuffer[0][0], 24}
  // ,{&smallMidiBuffer[1][0],&smallVelocityBuffer[1][0],&smallWindowBuffer[1][0],24}
};


//create custom instance of MIDI library that does not have big sysEx buffer to save some RAM
struct MySettings : public midi::DefaultSettings
{
  static const unsigned SysExMaxSize = 1; // Accept SysEx messages up to 1024 bytes long.
};

MIDI_CREATE_CUSTOM_INSTANCE(SoftwareSerial, softSerial, midiA, MySettings);


#define INPUT_CHANNEL 1 // input midi channel
#define CC_NUMBER 1 //mod wheel

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  if (channel == INPUT_CHANNEL) {
    buffer[0].addNoteToBuffer(pitch, velocity);
    uint8_t _note = buffer[0].getVoiceNote(0);

    if (_note > 60) { //tere are 25 semitones available. from the centre up of the keyboard prefer the high C
      if (_note % 24 == 0) _note = 25;
      else _note %= 24;
    }
    else {
      _note %= 24;
    }
    setSemitone(_note);
  }

}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{

  if (channel == INPUT_CHANNEL) {
    buffer[0].noteOff(pitch);
    uint8_t _note = buffer[0].getVoiceNote(0);

    if (_note > 60) {
      / tere are 25 semitones available. from the centre up of the keyboard prefer the high C
      if (_note % 24 == 0) _note = 25;
      else _note %= 24;
    }
    else {
      _note %= 24;
    }
    setSemitone(_note);
  }


}

void handleControlChange(byte channel, byte number, byte value)
{
  if (channel == INPUT_CHANNEL) {
    if (number == CC_NUMBER) OCR0B = value << 1;
  }


}

// the setup function runs once when you press reset or power the board
void setup() {


  buffer[0].setPolyphony(1);
  buffer[0].setPriority(2);

  midiA.begin(MIDI_CHANNEL_OMNI);
  midiA.turnThruOff();
  midiA.setHandleNoteOn(handleNoteOn);
  midiA.setHandleNoteOff(handleNoteOff);
  midiA.setHandleControlChange(handleControlChange);

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  //set fast PWM interrupt to render CV outputs
  cli();                               // Interrupts OFF (disable interrupts globally)
  TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
  TCCR0B = 0 << WGM02 | 1 << CS00;
  sei();
  TIMSK |= _BV(TOIE0);

}

uint16_t faderPitchValue;
uint16_t faderFormantValue;
uint8_t semitoneTable[24];

void loop() {

  midiA.read(); //read midi

  //if fader move use the fader value
  uint16_t newValue = map(analogRead(2), 0, 1024, 0, 25);
  if (newValue != faderPitchValue) setSemitone(newValue);
  faderPitchValue = newValue;

  newValue = map(analogRead(3), 0, 1024, 0, 64);
  if (newValue != faderFormantValue) OCR0B = faderFormantValue << 2;
  faderFormantValue = newValue;

}

void setSemitone(uint8_t _tone) {
  //the response is not linear so the curve is spread into 4 linear sections to map the semitones correctly
  if (_tone < 12) OCR0A = map(_tone, 0, 25, 0, 255);
  else if (_tone < 20) OCR0A = map(_tone, 12, 25, 140, 255);
  else if (_tone < 24) OCR0A = map(_tone, 20, 25, 218, 255);
  else OCR0A = map(_tone, 24, 25, 247, 255);

}
