// Properties.cpp : Defines the functions for the static library.
//

// #include "pch.h"
#include "Properties.h"

// --- properties ---

properties properties::load(std::istream& in)
{
	return properties_parser::parse(in);
}

void properties::store(std::ostream& out) const
{
	for (const auto& [key, value] : backing_map)
		out << key << properties::DELIMITER << value << std::endl;
}

std::string properties::get(const std::string& key) const
{
	return backing_map.at(key);
}

std::string properties::get_or_default(const std::string& key, const std::string& default_value) const
{
	return backing_map.contains(key) ? backing_map.at(key) : default_value;
}

void properties::set(const std::string& key, const std::string& value)
{
	backing_map[key] = value;
}

bool properties::contains(const std::string& key) const
{
	return backing_map.contains(key);
}

// --- parser ---

properties properties::properties_parser::parse(std::istream& in)
{
	properties props;
	std::string line;

	while ((line = read_logical_line(in)) != "") {
		const auto& [key, value] = parse_logical_line(line);
		props.set(key, value);
	}

	return props;
}

std::string properties::properties_parser::read_logical_line(std::istream& in)
{
	auto get_next_trimmed_natural_line = [](std::istream& in0) {
		if (in0.eof())
			return std::string("");

		std::string line;
		std::getline(in0, line);
		// left-trim
		line.erase(
			line.begin(),
			std::ranges::find_if(line.begin(), line.end(), [](unsigned char ch) {
				return !std::isspace(ch);
			})
		);
		return line;
	};

	auto is_blank = [](const std::string& trimmed_line) {
		return std::ranges::all_of(trimmed_line.begin(), trimmed_line.end(), isspace);
	};

	auto is_comment = [](const std::string& trimmed_line) {
		return trimmed_line.starts_with('#') || trimmed_line.starts_with('!');
	};

	auto ends_in_backslash = [](const std::string& natural_line) {
		int backslash_count = 0;
		for (int i = natural_line.size() - 1; i >= 0; i--, backslash_count++)
		{
			if (natural_line.at(i) != '\\')
				break;
		}
		return (backslash_count % 2) == 1;
	};

	std::string natural_line = get_next_trimmed_natural_line(in);

	// skip blank lines and comments
	while (is_blank(natural_line) || is_comment(natural_line))
		natural_line = get_next_trimmed_natural_line(in);

	// join together lines that end in '\'
	while (ends_in_backslash(natural_line))
	{
		// remove backslash
		natural_line.erase(natural_line.end().operator--(), natural_line.end());
		// join with next line
		natural_line += get_next_trimmed_natural_line(in);
	}

	return natural_line;
}

std::pair<std::string, std::string> properties::properties_parser::parse_logical_line(const std::string& trimmed_line)
{
	auto char_at = [trimmed_line](size_t index) {
		return trimmed_line.at(index);
	};

	auto is_non_whitespace_delimiter = [](char c) {
		return c == '=' || c == ':';
	};

	auto is_delimiter = [is_non_whitespace_delimiter](char c) {
		return is_non_whitespace_delimiter(c) || isspace(c);
	};

	size_t key_start = 0;
	size_t key_end = 1;

	// find first non-escaped delimiter
	for (; key_end < trimmed_line.size(); key_end++)
	{
		if (is_delimiter(char_at(key_end)) && char_at(key_end - 1) != '\\')
			break;
	}
	
	size_t value_start = key_end + 1;
	// skip all whitespace immediately after key
	while (isspace(char_at(value_start)))
		value_start++;

	// if '=' or ':' immediately after whitespace, skip it and all subsequent whitespace
	if (is_non_whitespace_delimiter(char_at(value_start))) {
		value_start++;

		while (isspace(char_at(value_start)))
			value_start++;
	}

	// end index is inclusive
	size_t value_end = trimmed_line.size() - 1;

	std::string key = trimmed_line.substr(key_start, key_end - key_start);
	std::string value = trimmed_line.substr(value_start, value_end - value_start);

	return std::pair(key, value);
}

