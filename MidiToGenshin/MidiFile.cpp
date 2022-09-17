#include <fstream>

#include "MidiFile.h"
#include "Properties.h"

void CringeMidiFile::play()
{
	size_t track_no = tracks.size();
	std::vector<int> pointers;
	for (size_t i = 0; i < track_no; i++)
		pointers.push_back(0);

	auto get_note = [this, &pointers](size_t track_number) {
		return tracks[track_number]->notes[pointers[track_number]];
	};

	auto end1 = [this, &pointers, track_no]() {
		for (size_t i = 0; i < track_no; i++)
		{
			if (pointers[i] < tracks[i]->notes.size())
				return false;
		}
		return true;
	};

	auto get_key_note = [](int note_number) {
		static std::array<std::string, 12> note_names =  { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
		std::string note = std::to_string((note_number / 12) - 1) + note_names[note_number % 12];

		std::ifstream properties_file;
		properties_file.open("path/to/file", std::fstream::in);
		properties props = properties::load(properties_file);
		properties_file.close();

		char letter = props.get_or_default(note, "#").at(0);

		if (letter == '#')
			return -1;

		return 0x41 + (letter - 'A');
	};

	int prev_wt = 0;
	while (!end1()) {
		// find min
		size_t min_track = 9999999;
		int min_wall_time = 9999999;
		for (size_t i = 0; i < track_no; i++)
		{
			if (pointers[i] >= tracks[i]->notes.size())
				continue;

			int cwt = get_note(i).wall_time;
			if (cwt < min_wall_time)
			{
				min_track = i;
				min_wall_time = cwt;
			}
		}

		if (min_track == 9999999)
		{
			printf("wtf");
		}

		// wait min
		printf("sleeping for: %d\n", (min_wall_time - prev_wt) * 4);
		std::this_thread::sleep_for(std::chrono::milliseconds((min_wall_time - prev_wt) * 4 + 1));
		prev_wt = min_wall_time;

		// send key press
		WORD virtual_key_code = get_key_note(get_note(min_track).key_number);
		if (virtual_key_code != 0xffff)
		{
			INPUT inputs[2] = {};
			ZeroMemory(inputs, sizeof(inputs));

			inputs[0].type = INPUT_KEYBOARD;
			inputs[0].ki.wVk = virtual_key_code;

			inputs[1].type = INPUT_KEYBOARD;
			inputs[1].ki.wVk = virtual_key_code;
			inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

			printf("sending 0x%x\n", virtual_key_code);
			UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
			if (uSent != ARRAYSIZE(inputs))
			{
				printf("SendInput failed\n");
			}
		}

		pointers[min_track]++;
	}
}
