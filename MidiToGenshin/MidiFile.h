#pragma once

#include <ostream>
#include <vector>

#include "MidiEvent.h"

struct CringeMidiTrack
{
	std::vector<NoteEvent> notes;
};

struct CringeMidiFile
{
	std::vector<CringeMidiTrack*> tracks;

	void write_to_file(std::ostream& midi_ostream);
};

