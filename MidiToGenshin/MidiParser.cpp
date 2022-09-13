#include "MidiParser.h"

CringeMidiFile* MidiParser::parseByteStream()
{
	CringeMidiFile* cringe_midi_file = new CringeMidiFile();

	// header chunk
	std::string chunk_type = parseString(4);
	uint32_t chunk_length = parse32();

	uint16_t format;
	uint16_t ntrks;
	uint16_t division;

	uint16_t ticks_per_quarter_note;
	short frames_per_second;
	short ticks_per_frame;

	if (chunk_length != 6) {
		skipBytes(chunk_length);
		ntrks = 0;
	}
	else
	{
		format = parse16();
		ntrks = parse16();
		division = parse16();

		if ((division >> 15) == 0)
		{
			ticks_per_quarter_note = division & ((1 << 15) - 1);
		}
		else
		{
			short negative_SMPTE_format = (division >> 8) & ((1 << 7) - 1);
			frames_per_second = -negative_SMPTE_format;
			ticks_per_frame = division & ((1 << 8) - 1);
		}

		printf("format %d\n", format);
		printf("ntrks %d\n", format);
		printf("division %d\n", format);
	}

	// track chunks
	for (size_t i = 0; i < ntrks; i++)
	{
		CringeMidiTrack *track = new CringeMidiTrack();
		cringe_midi_file->tracks.push_back(track);

		chunk_type = parseString(4);
		chunk_length = parse32();

		bool endOfTrack = false;

		uint32_t wall_time = 0;
		uint32_t delta_time;
		uint8_t prev_status_byte = 0;
		uint8_t status_byte;

		while (in->has_next() && !endOfTrack) {
			printf("\n");

			// MTrk event
			delta_time = parseValLength();
			wall_time += delta_time;
			printf("delta_time: %4d\n", delta_time);

			status_byte = parse8();

			// rolling status
			if ((status_byte & 0xF0) < 0x80)
			{
				// printf("rolling status: %d - prev_status_byte: %d\n", status_byte, prev_status_byte);
				status_byte = prev_status_byte;
				in->unget();
			}

			printf("status_byte: %2x\n", status_byte);

			prev_status_byte = status_byte;
			// printf("prev_status_byte: %d\n", prev_status_byte);

			// Channel Message
			// status byte is always initialized because first byte cannot be omitted
			if ((status_byte & 0xF0) != MidiMessage::SystemMessage)
			{
				uint8_t channnel_number = status_byte & 0xF;

				switch (status_byte & 0xF0)
				{
				case MidiMessage::NoteOff: {
					// appendix 1.3
					uint8_t key_number = parse8();
					uint8_t velocity = parse8();
					printf("--- Note Off: Note number = %d, Velocity = %d\n", key_number, velocity);
					break;
				}
				case MidiMessage::NoteOn: {
					// appendix 1.3
					uint8_t key_number = parse8();
					uint8_t velocity = parse8();
					if (velocity != 0)
						track->notes.push_back({ wall_time, key_number});
					printf("--- Note On: Note number = %d, Velocity = %d\n", key_number, velocity);
					break;
				}
				case MidiMessage::PolyphonicKeyPressure: {
					skipBytes(2);
					break;
				}
				case MidiMessage::ControlChange: {
					skipBytes(2);
					break;
				}
				case MidiMessage::ProgramChange: {
					skipBytes(1);
					break;
				}
				case MidiMessage::ChannelPressure: {
					skipBytes(1);
					break;
				}
				case MidiMessage::PitchWheelChange: {
					skipBytes(2);
					break;
				}
				}
			}
			// System Message
			else
			{
				switch (status_byte & 0xF)
				{
				case 0x0: {
					uint32_t length = parseValLength();
					skipBytes(length);

					//skipBytes(1);
					//while (parse8() != 0xF7)
					//	;
					break;
				}
				case 0x2: {
					skipBytes(2);
					break;
				}
				case 0x3: {
					skipBytes(1);
					break;
				}
				case 0x7: {
					uint32_t length = parseValLength();
					skipBytes(length);
					break;
				}
				case 0xF: {
					// meta-event
					uint8_t type = parse8();
					uint32_t length = parseValLength();

					switch (type)
					{
					// TODO don't ignore some (e.g. tempo, ...)
					case MetaEvent::EndOfTrack: {
						endOfTrack = true;
						break;
					}
					default:
						skipBytes(length);
					}

					break;
				}
				default: {
					skipBytes(0);
				}
				}
			}
		}
	}

	return cringe_midi_file;
}

void MidiParser::skipBytes(int byteCount)
{
	for (size_t i = 0; i < byteCount; i++)
		in->get();
}

std::uint32_t MidiParser::parseValLength()
{
	std::uint32_t value = 0;
	std::uint8_t curr;

	do {
		value = (value << 7) | ((curr = in->get()) & 0x7f);
	} while (curr & 0x80);

	return value;
}

uint8_t MidiParser::parse8()
{
	return parseInt<uint8_t>(1);
}

uint16_t MidiParser::parse16()
{
	return parseInt<uint16_t>(2);
}

uint32_t MidiParser::parse32()
{
	return parseInt<uint32_t>(4);
}

uint64_t MidiParser::parse64()
{
	return parseInt<uint64_t>(8);
}

std::string MidiParser::parseString(int length)
{
	std::string s;
	for (int i = 0; i < length; i++)
	{
		s += in->get();
	}

	return s;
}

