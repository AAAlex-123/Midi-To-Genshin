#include "MidiFile.h"

#include <array>
#include <limits>
#include <string>

void CringeMidiFile::write_to_file(std::ostream& midi_ostream)
{
	std::array<std::string, 12> note_names = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

	size_t track_no = tracks.size();
	std::vector<int> pointers;
	for (size_t i = 0; i < track_no; i++)
		pointers.push_back(0);

	auto get_latest_note_from_track = [this, &pointers](size_t track_number) {
		return tracks[track_number]->notes[pointers[track_number]];
	};

	auto get_note_name = [&note_names](int midi_note_number) {
		return std::to_string((midi_note_number / 12) - 1) + note_names[midi_note_number % 12];
	};

	int current_wall_time = 0;
	while (true)
	{
		// find next note out of all tracks (note with least wall time)
		size_t min_track = ULLONG_MAX;
		int min_wall_time = INT_MAX;
		for (size_t i = 0; i < track_no; i++)
		{
			if (pointers[i] == tracks[i]->notes.size())
				continue;

			int track_wall_time = get_latest_note_from_track(i).wall_time;
			if (track_wall_time < min_wall_time)
			{
				min_track = i;
				min_wall_time = track_wall_time;
			}
		}

		// no min_track -> all pointers have reached max value
		if (min_track == ULLONG_MAX)
			break;

		// write "`delta_millis`-`note_name`"
		// TODO figure out delta time from track info
		uint32_t delta_millis = (min_wall_time - current_wall_time) * 4;
		std::string note_name = get_note_name(get_latest_note_from_track(min_track).key_number);
		midi_ostream << delta_millis << '-' << note_name << std::endl;

		// update wall time and pointer of track used
		current_wall_time = min_wall_time;
		pointers[min_track]++;
	}
}

