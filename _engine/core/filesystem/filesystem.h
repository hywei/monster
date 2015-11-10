#ifndef __MONSTER_FILESYSTEM_H__
#define __MONSTER_FILESYSTEM_H__

#include "core/filesystem/file.h"

namespace monster
{
	class FileSystem
	{
	private:
		FileSystem(const FileSystem&) = delete;
		FileSystem& operator = (const FileSystem&) = delete;

	public:
		FileSystem() {}
		virtual ~FileSystem() {}

		virtual File* open(const char* path, FileOpenMode mode) = 0;
		virtual void close(File* file) = 0;

		virtual bool isExist(const char* path) = 0;

		virtual bool createFile(const char* path) = 0;
		virtual bool deleteFile(const char* path) = 0;

		virtual bool createDirectory(const char* path) = 0;
		virtual bool delteDirectory(const char* path) = 0;

	};
}

#endif