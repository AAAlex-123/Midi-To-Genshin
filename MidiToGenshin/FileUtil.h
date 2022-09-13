#pragma once

#include <fstream>
#include <vector>
#include <string>

namespace FileUtil
{

	class ByteStream
	{
	public:
		virtual ~ByteStream() = default;

		virtual bool is_open() = 0;
		virtual bool has_next() const = 0;
		virtual uint8_t get() = 0;
		virtual size_t rewind(size_t numberOfBytes) = 0;
		virtual size_t unget() final;
	};

	class FileByteStream : public ByteStream
	{
	public:
		explicit FileByteStream(const std::string& filename);

		bool is_open() override;
		bool has_next() const override;
		uint8_t get() override;
		size_t rewind(size_t numberOfBytes) override;

		~FileByteStream();

	private:
		std::ifstream ifs;
	};

	class TestByteStream : public ByteStream
	{
	public:
		explicit TestByteStream(const std::string&);

		bool is_open() override;
		bool has_next() const override;
		uint8_t get() override;
		size_t rewind(size_t numberOfBytes) override;

	private:
		std::vector<uint8_t> bytes;

		size_t curr_byte = 0;
	};
}

