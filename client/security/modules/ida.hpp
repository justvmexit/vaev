#pragma once

#include <Windows.h>

#include <modules/lazy_importer.hpp>
#include <modules/xor_string.hpp>

namespace client
{
	class ida
	{
	public:
		/**
		* Checks the history of opened files in IDA.
		* @return Boolean containing true or false depending on history.
		*/
		auto check_history() -> bool
		{
			char CurrentLocation[MAX_PATH];
			GetModuleFileNameA(NULL, CurrentLocation, MAX_PATH);

			HKEY History;

			auto fn_get_value = LI_FN(RegGetValueA).safe();
			auto fn_open_key = LI_FN(RegOpenKeyA).safe();
			fn_open_key(HKEY_CURRENT_USER, _xor_("Software\\Hex-Rays\\IDA\\History64").c_str(), &History);

			if (History)
			{
				for (int i = 0; i < 10; i++)
				{
					char Value[256];
					auto Size = (DWORD)256;
					fn_get_value(HKEY_CURRENT_USER, _xor_("Software\\Hex-Rays\\IDA\\History64").c_str(), std::to_string(i).c_str(), RRF_RT_ANY, NULL, (PVOID)&Value, &Size);

					if (strstr(Value, CurrentLocation)) return true;
				}
			}
			return false;
		}
	};
}
