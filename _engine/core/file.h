#ifndef __MONSTER_FILE_H__
#define __MONSTER_FILE_H__

#include <cstdint>

namespace monster
{
	enum class FileOpenMode : std::uint8_t
	{
		read = 1,
		write = 2
	};

	class File
	{
	public:
		explicit File(FileOpenMode mode, bool async = false);
		virtual ~File();

		virtual size_t read(void* buffer, size_t length) = 0;
		virtual size_t write(const void* buffer, size_t length) = 0;
		virtual void seek(size_t position) = 0;
		virtual void seekEnd() = 0;
		virtual void skip(size_t bytes) = 0;
		virtual size_t tell() const = 0;
	};
}

#endif