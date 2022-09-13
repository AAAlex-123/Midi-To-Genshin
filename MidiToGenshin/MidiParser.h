#pragma once

#include <cstdint>

#include "FileUtil.h"
#include "MidiFile.h"

class MidiParser
{
public:
	static CringeMidiFile* parse(const std::string& filename);

private:
	explicit MidiParser(FileUtil::ByteStream * const bs) : in(bs) {}

	CringeMidiFile* parseByteStream();

	void skipBytes(int byteCount);
	std::uint32_t parseValLength();
	std::string parseString(int length);

	template<typename T>
	T parseInt(int length);
	uint8_t parse8();
	uint16_t parse16();
	uint32_t parse32();
	uint64_t parse64();

	FileUtil::ByteStream * const in;

	friend class Tester;
};

enum MetaEvent : uint8_t
{
	SequenceNumber = 0x00,
	TextEvent = 0x01,
	CopyrightNotice = 0x02,
	SequenceOrTrackName = 0x03,
	InstrumentName = 0x04,
	Lyric = 0x05,
	Marker = 0x06,
	CuePoint = 0x07,
	MidiChannelPrefix = 0x20,
	EndOfTrack = 0x2F,
	SetTempo = 0x51,
	SmpteOFfset = 0x54,
	TimeSignature = 0x58,
	KeySignature = 0x59,
	SequencerSpecificMetaEvent = 0x7F,
};

template<typename T>
T MidiParser::parseInt(int numberOfBytes)
{
	T value = 0;
	for (int i = 0; i < numberOfBytes; i++)
		value = (value << 8) | in->get();
	return value;
}

