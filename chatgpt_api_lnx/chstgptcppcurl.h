#pragma once

#include <string>
#include <string_view>
#include <codecvt>

class chatgptcppcurl
{
public:
	virtual ~chatgptcppcurl() = default;
	bool connect() const { return false; };
	bool get() const { return false; };
	bool post() const { return false; };
private:
	std::shared_ptr<web::http::client::http_client> client = nullptr;
};
