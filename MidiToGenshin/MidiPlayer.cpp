#include "MidiPlayer.h"

#include <string>
#include <thread>
#include <Windows.h>

#include "Properties.h"

void MidiPlayer::play(std::istream& midi_istream, std::istream& properties_istream)
{
	properties props = properties::load(properties_istream);

	while (!midi_istream.eof())
	{
		// get next wait time and note
		std::string line;
		std::getline(midi_istream, line);

		// e.g. 123-C3
		// delimiter_index = 3
		// delimiter_index + 1 = 4
		// line.size() - delimiter_index - 1 = 6 - 3 - 1 = 2
		size_t delimiter_index = line.find('-');
		uint32_t delta_millis = stoul(line.substr(0, delimiter_index));
		std::string note_name = line.substr(delimiter_index + 1,
			line.size() - delimiter_index - 1);

		// wait delta time
		// TODO remove this print
		printf("sleeping for: %04d\n", delta_millis);
		std::this_thread::sleep_for(std::chrono::milliseconds(delta_millis));

		// figure out key press
		char key = props.get_or_default(note_name, "#").at(0);

		// key == '#' => no key mapped for this note => don't sent any key presses
		if (key != '#') {

			// send key press
			WORD virtual_key_code = 0x41 + (key - 'A');

			INPUT inputs[2] = {};
			ZeroMemory(inputs, sizeof(inputs));

			inputs[0].type = INPUT_KEYBOARD;
			inputs[0].ki.wVk = virtual_key_code;

			inputs[1].type = INPUT_KEYBOARD;
			inputs[1].ki.wVk = virtual_key_code;
			inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

			// TODO remove this print
			printf("sending 0x%x - %c\n", virtual_key_code, virtual_key_code - 0x41 + 'A');

			UINT uSent = SendInput(2, inputs, sizeof(INPUT));
			if (uSent != ARRAYSIZE(inputs))
				printf("Did not send: 0x%x - %c\n", virtual_key_code, virtual_key_code - 0x41 + 'A');
		}
	}
}

