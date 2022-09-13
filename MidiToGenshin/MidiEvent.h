#pragma once

enum MidiMessage : uint8_t
{
	NoteOff = 0x80,
	NoteOn = 0x90,
	PolyphonicKeyPressure = 0xA0,
	ControlChange = 0xB0,
	ProgramChange = 0xC0,
	ChannelPressure = 0xD0,
	PitchWheelChange = 0xE0,
	SystemMessage = 0xF0,
};

struct NoteEvent
{
	uint32_t wall_time;
	uint8_t key_number;
};

/*
struct MidiEvent
{
	uint32_t delta_time;
	MidiMessage event_type;

	virtual std::string to_string() final
	{
		return std::format("delta time: %4d %s: %s", delta_time, event_type, to_string0());
	}

	virtual std::string to_string0() = 0;
};

struct NoteOffEvent : MidiEvent
{
	uint8_t key_number;
	uint8_t velocity;
};

struct NoteOnEvent : public MidiEvent
{
	uint8_t key_number;
	uint8_t velocity;

	std::string to_string0()
	{
		return std::format("Note %s (%d), Velocity %d", get_note_from_key_number(key_number), key_number, velocity);
	}
};
*/

