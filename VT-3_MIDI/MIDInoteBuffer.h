
#ifndef MIDI_NOTE_BUFFER_H_
#define MIDI_NOTE_BUFFER_H_

//#include <basic.h>
#include <Arduino.h>


class MIDInoteBuffer {

public:
	void init();
	MIDInoteBuffer(uint8_t *_midiBuffer, uint8_t *_velocityBuffer,uint8_t *_windowBuffer, uint8_t _bufferSize);
	void addNoteToBuffer(uint8_t _note, uint8_t _velocity);
	void addNoteToWindowBuffer(uint8_t _note);
	uint8_t getWindowNote(uint8_t _voice,uint8_t _windowPosition);
	bool removeNoteFromBuffer(uint8_t note);
	bool removeNoteFromWindowBuffer(uint8_t note);
	void handlePriority();
	void setPolyphony(uint8_t _polyphony);
	void setPriority(uint8_t _priority);
	void testOrdering();
	void orderMidiBuffer();
	void orderWindowBuffer();
	void setBuffer(uint8_t _index,uint8_t _value);
	void increaseWindowPosition(){ windowPosition++; };
	uint8_t getWindowPosition(){ return windowPosition; };

	uint8_t getBuffer(uint8_t _index);

	uint8_t getVoiceNote(uint8_t _voice);
	uint8_t getVoiceVelocity(uint8_t _voice);
	uint8_t getNoteForVoice(uint8_t _voice);

	uint8_t getNoteToBePlayedAgain();
	uint8_t getVelocityOfNoteToBePlayedAgain();
	uint8_t getNumberOfNotesInBuffer();
	uint8_t getNoteFromBuffer(uint8_t _note);
	uint8_t getVelocityFromBuffer(uint8_t _note);
	uint8_t getNoteFromWindowBuffer(uint8_t _note);

	bool getVoiceGate(uint8_t _voice);
	void allocateVoice(uint8_t _note);
	void deallocateVoice(uint8_t _note);
	void copyMidiBufferToWindowBuffer();
	bool legato;
	//
	void clearBuffers();
	bool isNoteInBuffer(uint8_t note);
	void setSustain(bool _state);
	void noteOff(uint8_t _note);
	bool isNoteSustained(uint8_t _note);
	void removeSustainedNote(uint8_t _note);


private:
	void addSustainedNote(uint8_t _note);
	void removeAllSustainedNotes();
	uint8_t sustainedNote[16];
	bool _sustain;
	uint8_t windowPosition;
	void decreaseReleaseOrder();
	uint8_t activeVoice[4];
	uint16_t whenReleased[4];
	uint8_t priority;
	uint8_t polyphony;
	uint8_t notesInBuffer;
	uint8_t notesInWindowBuffer;
	uint8_t *midiBuffer;
	uint8_t *velocityBuffer;
	uint8_t *windowBuffer;
	const uint8_t BUFFER_SIZE;

	bool voiceOn[4];
	//uint8_t voiceRotation;
	void shiftBuffer(uint8_t direction,uint8_t from);
	void shiftWindowBuffer(uint8_t direction,uint8_t from);
	uint8_t findNoteInBuffer(uint8_t note);
	uint8_t findNoteInWindowBuffer(uint8_t note);

};





#endif
