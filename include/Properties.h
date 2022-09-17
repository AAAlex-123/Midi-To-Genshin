#pragma once

#include <algorithm>
#include <istream>
#include <string>
#include <unordered_map>

class properties
{
public:
	static properties load(std::istream& in);

	void store(std::ostream& out) const;

	std::string get(const std::string& key) const;
	std::string get_or_default(const std::string& key, const std::string& default_value) const;
	void set(const std::string& key, const std::string& value);
	bool contains(const std::string& key) const;

private:
	static constexpr char DELIMITER = '=';

	std::unordered_map<std::string, std::string> backing_map;


	class properties_parser
	{
	public:
		static properties parse(std::istream& in);

	private:
		static std::string read_logical_line(std::istream& ifs);
		static std::pair<std::string, std::string> parse_logical_line(const std::string& trimmed_line);
	};
};

