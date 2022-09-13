#include "FileUtil.h"

// ByteStream

size_t FileUtil::ByteStream::unget()
{
	return rewind(1);
}

// FileByteStream

FileUtil::FileByteStream::FileByteStream(const std::string& filename)
{
	ifs.open(filename, std::fstream::in | std::ios::binary);
}

bool FileUtil::FileByteStream::is_open()
{
	return ifs.is_open();
}

bool FileUtil::FileByteStream::has_next() const
{
	return !ifs.eof();
}

uint8_t FileUtil::FileByteStream::get()
{
	uint8_t next = ifs.get();
	printf("%02x ", next);
	return next;
}

size_t FileUtil::FileByteStream::rewind(size_t numberOfBytes)
{
	size_t rewinded;
	for (rewinded = 0; rewinded < numberOfBytes; rewinded++)
	{
		ifs.unget();
		if (ifs.fail())
			break;
	}
	return rewinded;

	// alternative failed implementation:
	//size_t rewinded = std::min(numberOfBytes, ifs.tellg());
	//ifs.seekg(-rewinded, std::ios_base::cur);
}

FileUtil::FileByteStream::~FileByteStream()
{
	ifs.close();
}

// TestByteStream

FileUtil::TestByteStream::TestByteStream(const std::string& hex)
{
	for (int i = 0; i < hex.size(); i += 2) {
		std::string nextByte = hex.substr(i, 2);
		uint8_t byte = strtol(nextByte.c_str(), nullptr, 16);
		bytes.push_back(byte);
	}
}

bool FileUtil::TestByteStream::is_open()
{
	return true;
}

bool FileUtil::TestByteStream::has_next() const
{
	return curr_byte < bytes.size();
}

uint8_t FileUtil::TestByteStream::get()
{
	if (curr_byte >= bytes.size())
		throw (-1);

	uint8_t nextByte = bytes[curr_byte];
	++curr_byte;
	return nextByte;
}

size_t FileUtil::TestByteStream::rewind(size_t numberOfBytes)
{
	size_t rewinded = std::min(numberOfBytes, curr_byte);
	curr_byte -= rewinded;
	return rewinded;
}

