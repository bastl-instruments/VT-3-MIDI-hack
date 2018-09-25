/*
 * MIDInoteBuffer.cpp

 *
 *  Created on: Jan 28, 2015
 *      Author: dasvaclav
 */
#include "MIDInoteBuffer.h"
//#include <basic.h>
#include <Arduino.h>

#define LEFT_SHIFT 0
#define RIGHT_SHIFT 1
MIDInoteBuffer::MIDInoteBuffer(uint8_t *_midiBuffer, uint8_t *_velocityBuffer,uint8_t *_windowBuffer, uint8_t _bufferSize) : BUFFER_SIZE(_bufferSize){

	midiBuffer=_midiBuffer;
	velocityBuffer=_velocityBuffer;
	windowBuffer=_windowBuffer;


	priority=0;
	notesInWindowBuffer=0;
	legato=0;
	polyphony=1;
	notesInBuffer=0;
	windowPosition=0;

}

void MIDInoteBuffer::init(){
	for(uint8_t i=0;i<polyphony;i++) activeVoice[i]=255,whenReleased[i]=polyphony-i; //0,1,2,3
	clearBuffers();
}
#define LOW_P 0
#define HIGH_P 1
#define ORDER_P 2

bool MIDInoteBuffer::isNoteInBuffer(uint8_t note){
 return findNoteInBuffer(note)!=255;
}
void MIDInoteBuffer::handlePriority(){
	//uint8_t a;
	// unsigned char nextOne=255;
	 if(priority!=ORDER_P) orderMidiBuffer();

	switch(priority){
		case LOW_P:
			/*
		 for(int j=0;j<BUFFER_SIZE;j++)  orderedBuffer[j]=0;
		 nextOne=255;
		  for(uint8_t i=0;i<notesInBuffer;i++){
			if(midiBuffer[i] < nextOne) nextOne=midiBuffer[i];
		  }
		  orderedBuffer[0]=nextOne;

		  for(int j=1;j<notesInBuffer;j++){
			nextOne=255;
			for(int i=0;i<notesInBuffer;i++){
			  if(midiBuffer[i]<nextOne && midiBuffer[i] > orderedBuffer[j-1] ) nextOne=midiBuffer[i];
			}
			orderedBuffer[j]=nextOne;
		  }
*/
		  break;
		case HIGH_P:


			break;
		case ORDER_P:
			break;
	}
}
void MIDInoteBuffer::orderMidiBuffer(){
	for (uint8_t i = 0; i < notesInBuffer; ++i){
		for (uint8_t j = i + 1; j < notesInBuffer; ++j){
			if (midiBuffer[i] > midiBuffer[j]){
				uint8_t a =  midiBuffer[i];
				midiBuffer[i] = midiBuffer[j];
				midiBuffer[j] = a;
			}
		}
	}
}

void MIDInoteBuffer::copyMidiBufferToWindowBuffer(){
	for (uint8_t i = 0; i < BUFFER_SIZE; i++){
		windowBuffer[i] = midiBuffer[i];
	}
}
void MIDInoteBuffer::orderWindowBuffer(){
	for (uint8_t i = 0; i < notesInWindowBuffer; ++i){
		for (uint8_t j = i + 1; j < notesInWindowBuffer; ++j){
			if (windowBuffer[i] > windowBuffer[j]){
				uint8_t a =  windowBuffer[i];
				windowBuffer[i] = windowBuffer[j];
				windowBuffer[j] = a;
			}
		}
	}
}
void MIDInoteBuffer::testOrdering(){
	for(uint8_t i=0;i<BUFFER_SIZE;i++){
	//	midiBuffer[i]=random(255);
	//	Serial.print(midiBuffer[i]);
	//	Serial.print(", ");
	}
//	Serial.println();

	notesInBuffer=BUFFER_SIZE;
	orderMidiBuffer();
	for(uint8_t i=0;i<BUFFER_SIZE;i++){

	//		Serial.print(midiBuffer[i]);
		//	Serial.print(", ");
	}
//	Serial.println();
}
void MIDInoteBuffer::setBuffer(uint8_t _index,uint8_t _value){
	notesInBuffer=_index;
	midiBuffer[_index]=_value;
}
uint8_t MIDInoteBuffer::getBuffer(uint8_t _index){
	return midiBuffer[_index];
}
void MIDInoteBuffer::setPolyphony(uint8_t _polyphony){
	polyphony=_polyphony;
}
void MIDInoteBuffer::setPriority(uint8_t _priority){
	priority=_priority;
}
uint8_t MIDInoteBuffer::getNoteToBePlayedAgain(){
	return midiBuffer[polyphony];
}
uint8_t MIDInoteBuffer::getVelocityOfNoteToBePlayedAgain(){
	return velocityBuffer[polyphony];
}
uint8_t MIDInoteBuffer::getNumberOfNotesInBuffer(){
	return notesInBuffer;
}
uint8_t MIDInoteBuffer::getNoteFromBuffer(uint8_t _note){
	return midiBuffer[_note];
}
uint8_t MIDInoteBuffer::getVelocityFromBuffer(uint8_t _note){
	return velocityBuffer[findNoteInBuffer(_note)];
}
uint8_t MIDInoteBuffer::getNoteFromWindowBuffer(uint8_t _note){
	return windowBuffer[_note];
}
void MIDInoteBuffer::clearBuffers(){
	for(int i=0;i<BUFFER_SIZE;i++){
		midiBuffer[i]=0;
		velocityBuffer[i]=0;
		windowBuffer[i]=0;
	}
	notesInBuffer=0;
}
void  MIDInoteBuffer::shiftBuffer(uint8_t direction,uint8_t from){
	if(direction==LEFT_SHIFT){
		for(int i=from;i<notesInBuffer;i++){
			midiBuffer[i]=midiBuffer[i+1];
			velocityBuffer[i]=velocityBuffer[i+1];
		 }
	}
	else if(direction==RIGHT_SHIFT){
		  for(int i=notesInBuffer;i>from;i--){
			midiBuffer[i]=midiBuffer[i-1];
			velocityBuffer[i]=velocityBuffer[i-1];
		  }
	}
}

void  MIDInoteBuffer::shiftWindowBuffer(uint8_t direction,uint8_t from){
	if(direction==LEFT_SHIFT){
		for(int i=from;i<notesInWindowBuffer;i++){
			windowBuffer[i]=windowBuffer[i+1];
		 }
	}
	else if(direction==RIGHT_SHIFT){
		  for(int i=notesInWindowBuffer;i>from;i--){
			windowBuffer[i]=windowBuffer[i-1];
		  }
	}
}

bool MIDInoteBuffer::removeNoteFromWindowBuffer(uint8_t _note){

	if(notesInWindowBuffer>0){ //midi noteOnBuffer
		uint8_t position=findNoteInWindowBuffer(_note);
		if(position!=255) shiftWindowBuffer(LEFT_SHIFT,position),notesInWindowBuffer--;
	}
	//voice alocation

	if(notesInWindowBuffer>=polyphony) return 1; // signal realocate voice externaly
	else return 0;

}
uint8_t  MIDInoteBuffer::findNoteInWindowBuffer(uint8_t note){
	for(int i=0;i<notesInWindowBuffer;i++){
		if(windowBuffer[i]==note) return i;
	}
	return 255;
}


uint8_t  MIDInoteBuffer::findNoteInBuffer(uint8_t note){
	for(int i=0;i<notesInBuffer;i++){
		if(midiBuffer[i]==note) return i;
	}
	return 255;
}
void MIDInoteBuffer::removeAllSustainedNotes(){
	uint8_t howManyTimes=notesInBuffer;
	for(uint8_t i=howManyTimes;i>0;i--){
		if(isNoteSustained(midiBuffer[i-1])) removeNoteFromBuffer(midiBuffer[i-1]),removeSustainedNote(midiBuffer[i-1]);
	}
}

bool  MIDInoteBuffer::isNoteSustained(uint8_t _note){
	if(_note>127) return false;
	else return bitRead(sustainedNote[_note/8],_note%8);
}

void  MIDInoteBuffer::addSustainedNote(uint8_t _note){
	bitWrite(sustainedNote[_note/8],_note%8,1);
}
void  MIDInoteBuffer::removeSustainedNote(uint8_t _note){
	bitWrite(sustainedNote[_note/8],_note%8,0);
}

void MIDInoteBuffer::setSustain(bool _state){
	if(_state) _sustain=_state;
	else{
		removeAllSustainedNotes();
		_sustain=_state;
	}
}

void MIDInoteBuffer::noteOff(uint8_t _note){
	if(_sustain) addSustainedNote(_note);
	else removeNoteFromBuffer(_note);
}
bool MIDInoteBuffer::removeNoteFromBuffer(uint8_t _note){

	if(notesInBuffer>0){ //midi noteOnBuffer
		uint8_t position=findNoteInBuffer(_note);
		if(position!=255) shiftBuffer(LEFT_SHIFT,position),notesInBuffer--;
	}
	deallocateVoice(_note);
	//voice alocation

	if(notesInBuffer>=polyphony) return 1; // signal realocate voice externaly
	else return 0;

}
void MIDInoteBuffer::addNoteToWindowBuffer(uint8_t _note){

	removeNoteFromWindowBuffer(_note);
	shiftWindowBuffer(RIGHT_SHIFT,0);

	if(notesInWindowBuffer<BUFFER_SIZE){
		notesInWindowBuffer++;
	}

	windowBuffer[0]=_note;

}

void MIDInoteBuffer::addNoteToBuffer(uint8_t _note, uint8_t _velocity){

	removeNoteFromBuffer(_note);
	removeSustainedNote(_note);
	shiftBuffer(RIGHT_SHIFT,0);

	if(notesInBuffer<BUFFER_SIZE){
		notesInBuffer++;
	}

	midiBuffer[0]=_note;
	velocityBuffer[0]=_velocity;
//	handlePriority();
	allocateVoice(_note);
}

void MIDInoteBuffer::allocateVoice(uint8_t _note){
	if(notesInBuffer==1){
		//added first note which means legato is over - reset window buffer
		for(uint8_t i=0;i<BUFFER_SIZE;i++) windowBuffer[i]=0;
		notesInWindowBuffer=0;
		windowPosition=0;

	}
	addNoteToWindowBuffer(_note);

	uint8_t voiceToUse=255;
	if(priority==ORDER_P){
		//voice steal - steal the voice from the last played note >the the one that just came out
		if(notesInBuffer>polyphony){
			for(uint8_t i=0;i<polyphony;i++) if(activeVoice[i]==midiBuffer[polyphony]) voiceToUse=i;
		}

		for(uint8_t i=0;i<polyphony;i++) if(activeVoice[i]==_note) voiceToUse=i;
		if(voiceToUse==255){ //if not find a voice with lowest number (the last used), voice with 255 is voice that is occupied
			//
			for(uint8_t i=0;i<polyphony;i++) if(whenReleased[i]<voiceToUse) voiceToUse=whenReleased[i]; //find the lowest value
			for(uint8_t i=0;i<polyphony;i++) if(whenReleased[i]==voiceToUse) voiceToUse=i; //find the index of the lowest value

		}
		if(voiceToUse<polyphony) whenReleased[voiceToUse]=255,activeVoice[voiceToUse]=_note; //voice occupied

		for(uint8_t i=0;i<polyphony;i++) windowBuffer[i]=activeVoice[i]; //have these in the same order

	}
	else{

		orderMidiBuffer();
		orderWindowBuffer();
		for(uint8_t i=0;i<polyphony;i++) whenReleased[i]=0;
		if(priority==HIGH_P){
			if(notesInBuffer==0){ //no held notes
				if(notesInWindowBuffer>=polyphony){
					for(uint8_t i=0;i<polyphony;i++) activeVoice[i]=windowBuffer[notesInWindowBuffer-i-1];
				}

			}
			else if(notesInBuffer>=polyphony){ // full house
				for(uint8_t i=0;i<polyphony;i++) activeVoice[i]=midiBuffer[notesInBuffer-i-1],whenReleased[i]=255;
			}
			else{ //anything in between
				for(uint8_t i=0;i<notesInBuffer;i++) whenReleased[i]=255;
				for(uint8_t i=0;i<notesInBuffer;i++) activeVoice[i]=midiBuffer[notesInBuffer-i-1];
				for(uint8_t i=notesInBuffer;i<polyphony;i++) activeVoice[i]=windowBuffer[notesInWindowBuffer-i-1];
			}

			//for(uint8_t i=0;i<notesInBuffer;i++) windowBuffer[notesInWindowBuffer-i-1]=activeVoice[i];
		}

		else if(priority==LOW_P){
			if(notesInBuffer==0){ //no held notes
				if(notesInWindowBuffer>=polyphony){
					for(uint8_t i=0;i<polyphony;i++) activeVoice[i]=windowBuffer[i];
				}

			}
			else if(notesInBuffer>=polyphony){ // full house
				for(uint8_t i=0;i<polyphony;i++) activeVoice[i]=midiBuffer[i],whenReleased[i]=255;
			}
			else{ //anything in between
				for(uint8_t i=0;i<notesInBuffer;i++) whenReleased[i]=255;
				for(uint8_t i=0;i<notesInBuffer;i++) activeVoice[i]=midiBuffer[i];
				for(uint8_t i=notesInBuffer;i<polyphony;i++) activeVoice[i]=windowBuffer[i];
			}
		}

	}

	//activeVoiceBuffer[];
}
void MIDInoteBuffer::decreaseReleaseOrder(){
	for(uint8_t i=0;i<polyphony;i++) if(whenReleased[i]<=polyphony && whenReleased[i]>0) whenReleased[i]--;
}
void MIDInoteBuffer::deallocateVoice(uint8_t _note){
	uint8_t _whichVoice=255;
	switch(priority){
		case HIGH_P:
			for(uint8_t i=0;i<polyphony;i++){
				if(activeVoice[i]==_note) _whichVoice=i; // if yes replace it with something from midi buffer, or put it to release stage
			}
			if(_whichVoice<polyphony){ //is the removed note active note? recalculate the buffer
				for(uint8_t i=0;i<notesInBuffer;i++) activeVoice[i]=midiBuffer[notesInBuffer-i-1];
			}
			for(uint8_t i=notesInBuffer;i<polyphony;i++) whenReleased[i]=0;
			break;
		case LOW_P:
			for(uint8_t i=0;i<polyphony;i++){
				if(activeVoice[i]==_note) _whichVoice=i; // if yes replace it with something from midi buffer, or put it to release stage
			}
			if(_whichVoice<polyphony){ //is the removed note active note? recalculate the buffer
				for(uint8_t i=0;i<notesInBuffer;i++) activeVoice[i]=midiBuffer[i];
			}
			for(uint8_t i=notesInBuffer;i<polyphony;i++) whenReleased[i]=0;
			break;
		case ORDER_P:
			//is the note assigned to a voice?
			for(uint8_t i=0;i<polyphony;i++){
				if(activeVoice[i]==_note) _whichVoice=i; // if yes replace it with something from midi buffer, or put it to release stage
			}
			if(_whichVoice<polyphony){
				//should it be replaced?
				if(notesInBuffer>=polyphony){ //needs to be replaced
					activeVoice[_whichVoice]=midiBuffer[polyphony-1];
				}
				else{	//is actually released
					whenReleased[_whichVoice]=polyphony;
					decreaseReleaseOrder();
					 //lower all positions in the buffer

				}
			}
			break;
	}


	if(notesInBuffer==0){
		/*//protection but fucks things up at this point
		uint8_t _hanging;
		for(uint8_t i=0;i<polyphony;i++){
			if(whenReleased[i]==255) _hanging=i;
			whenReleased[_hanging]=polyphony;
			decreaseReleaseOrder();
		}
		*/

		legato=false;
		//lastNote released
	}

}

bool MIDInoteBuffer::getVoiceGate(uint8_t _voice){
	bool _gate=false;
	//if(whenReleased[_voice]<=polyphony) _
	if(whenReleased[_voice]==255) _gate=true;
	else _gate=false;
	return _gate;
}

uint8_t MIDInoteBuffer::getWindowNote(uint8_t _voice,uint8_t _windowPosition){
	if(notesInWindowBuffer<polyphony){
		 return activeVoice[(_voice+_windowPosition)%(polyphony-1)];
	}
	else{
	 return windowBuffer[(_voice+_windowPosition)%(notesInWindowBuffer-1)];
	}
}

uint8_t MIDInoteBuffer::getVoiceVelocity(uint8_t _voice){
	return getVelocityFromBuffer(activeVoice[_voice]);
}

uint8_t MIDInoteBuffer::getVoiceNote(uint8_t _voice){
	 return activeVoice[_voice];
	//if(_voice<polyphony) return activeVoice[_voice];
	//else return 255;

	/*
	uint8_t voiceNote=255;
	switch(priority){
		case LOW_P:
			if(_voice<notesInBuffer){
				voiceNote=midiBuffer[_voice];
			}
		  break;
		case HIGH_P:
			if(_voice<notesInBuffer){
				voiceNote=midiBuffer[notesInBuffer-_voice-1];
			}
			break;
		case ORDER_P:
			if(_voice<notesInBuffer){
				voiceNote=midiBuffer[_voice];
			}

		case NATURAL:

			naturalCounter++;
			if(naturalCounter>polyphony) naturalCounter=0;
			if(_voice<notesInBuffer){
				voiceNote=midiBuffer[naturalCounter];
			}

			break;
			*/
	//}
	//return voiceNote;

}
