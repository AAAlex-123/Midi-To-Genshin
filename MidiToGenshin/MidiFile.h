#pragma once

#include <array>
#include <thread>
#include <vector>
#include <Windows.h>
#include <winuser.h>

#include "MidiEvent.h"

struct CringeMidiTrack
{
	std::vector<NoteEvent> notes;
};

struct CringeMidiFile
{
	std::vector<CringeMidiTrack*> tracks;

	void play();
	
};

