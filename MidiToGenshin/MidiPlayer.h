#pragma once

#include <istream>

class MidiPlayer
{
public:
	static void play(std::istream& midi_file, std::istream& properties_file);
};

