#include <iostream>
#include <array>

#include "FileUtil.h"
#include "MidiParser.h"

using namespace FileUtil;

class Tester {
public:
	void test_MidiParser_parseByteStream(const std::string& testCaseFilename) const
	{
		FileByteStream bs(testCaseFilename);
		CringeMidiFile* cmf = MidiParser(&bs).parseByteStream();
		for (auto& track : cmf->tracks)
		{
			printf("\n\n\nnew track\n");
			for (auto& note : track->notes)
			{
				printf("delta time: %6d - Note: %s\n", note.wall_time,
					Tester::get_note_from_key_number(note.key_number).c_str());
			}
		}
		cmf->play();
	}

	void test_MidiParser_parseValLength(const std::string& testCaseBytes) const
	{
		TestByteStream bs(testCaseBytes);
		uint32_t result = MidiParser(&bs).parseValLength();
		printf("%08x\n", result);
		printf("%8d\n", result);
	}

	void test_MidiParser_parseString(const std::string& testCaseString, int testCaseLength) const
	{
		TestByteStream bs(testCaseString);
		printf("%s\n", MidiParser(&bs).parseString(testCaseLength).c_str());
	}

private:
	static std::string get_note_from_key_number(uint8_t key_number)
	{
		static std::array<std::string, 12> note_names{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
		return note_names[key_number % 12] + std::to_string((key_number / 12) - 1); // note name + octave
	}
};

void test_MidiParser_parseByteStream(const Tester&);
void test_MidiParser_parseValLength(const Tester&);
void test_MidiParser_parseString(const Tester&);

int main(int argc, char** argv)
{
	if (argc == 1) {
		printf("please specify a file");
		return -1;
	}
	Tester tester;
	tester.test_MidiParser_parseByteStream(argv[1]);
	// Tester tester;
	// test_MidiParser_parseByteStream(tester);
	// test_MidiParser_parseValLength(tester);
	// test_MidiParser_parseString(tester);
}

void test_MidiParser_parseByteStream(const Tester& tester)
{
	std::string testCases[] = {
		// "C:\\Users\\alexm\\musescore\\scores\\exports\\alex\\midi\\MidiTest-my_war.mid",
		// "C:\\Users\\alexm\\musescore\\scores\\exports\\alex\\midi\\MidiTest-1_track.mid",
		// "C:\\Users\\alexm\\musescore\\scores\\exports\\alex\\midi\\MidiTest_2track.mid",
		"C:\\Users\\alexm\\musescore\\scores\\exports\\alex\\midi\\MidiTest-a_winter_night.mid"
	};

	for (auto const& testCase : testCases) {
		printf(" --- TEST CASE: %s ---\n", testCase.c_str());
		tester.test_MidiParser_parseByteStream(testCase);
	}
}

void test_MidiParser_parseValLength(const Tester& tester)
{
	std::string testCases[] = {
		"00",
		"40",
		"7F",
		"8100",
		"C000",
		"FF7F",
		"818000",
		"C08000",
		"FFFF7F",
		"81808000",
		"C0808000",
		"FFFFFF7F",

		"83473C",
		"193E50",
		"83473E",
		"194050",
		"870F40",
	};

	for (auto const& testCase : testCases)
		tester.test_MidiParser_parseValLength(testCase);
}

void test_MidiParser_parseString(const Tester& tester)
{
	std::string testCaseStrings[] = {
		"68656C6C6F",
		"68656C6C6F",
		"68656C6C6F",
	};

	int testCaseLengths[] = {
		2, 3, 4,
	};

	for (size_t i = 0; i < 3; i++)
		tester.test_MidiParser_parseString(testCaseStrings[i], testCaseLengths[i]);
}

