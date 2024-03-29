/***
 * Copyright (C) Alex Epstine (alex@comsuite.co.il). All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 * For the latest on this and related APIs, please see: https://github.com/ComSuite/openai-api
 * 
 * For the details of cpprest library, plese see: //https://github.com/microsoft/cpprestsdk
 ****/

#pragma once

#include <string>
#include <string_view>
#include <codecvt>
#include <cpprest/http_client.h>
#include "stdstrmacro.h"

namespace cs
{
	class chatgptcpprest
	{
	private:
		std::shared_ptr<web::http::client::http_client> client = nullptr;
	public:
		virtual ~chatgptcpprest() = default;

		void set_api_key(std::string_view key) { api_key = key; };
		void set_org_id(std::string_view id) { org_id = id; };

		bool connect(const std::string& url) 
		{
			try {
				client = std::make_shared<web::http::client::http_client>(FROM_STD_STR(url));
				return true;
			}
			catch (...) {
				return false;
			}
		}

		bool get(std::vector<unsigned char>& data) const
		{
			bool ret = false;

			web::http::http_response response = client->request(web::http::methods::GET).get();
			if (response.status_code() == web::http::status_codes::OK) {
				pplx::task<std::vector<unsigned char>> _data = response.extract_vector();
				_data.wait();

				data = _data.get();

				ret = true;
			}

			return ret;
		}

		bool get(std::string_view endpoint, std::string& response) const
		{ 
			bool ret = false;

			web::http::http_request request(web::http::methods::GET);
			prepare_request(request);
			request.set_request_uri(web::uri(U("/v1/models")));

			web::http::http_response resp = client->request(request).get();
			if (resp.status_code() == web::http::status_codes::OK) {
				response = resp.extract_json().get().to_string();

				ret = true;
			}

			return ret;
		};

		bool post(std::string_view endpoint, std::string_view request_body, std::string& response) const
		{
			bool ret = false;

			web::http::http_request request(web::http::methods::POST);
			prepare_request(request);
			request.set_body(std::string(request_body));
			request.set_request_uri(std::string(endpoint));

			web::http::http_response resp = client->request(request).get();
			if (resp.status_code() == web::http::status_codes::OK) {
				response = resp.extract_json().get().to_string();

				ret = true;
			}

			return ret;
		};
	private:
		std::string api_key = "";
		std::string org_id = "";

		void prepare_request(web::http::http_request& request) const
		{
#if defined _WIN32 || defined _WIN64
			std::wstring wbar = (U("Bearer "));
			wbar = wbar.append(utility::conversions::to_utf16string(api_key));
			request.headers().add(U("Authorization"), wbar);
			request.headers().add(U("OpenAI-Organization"), utility::conversions::to_utf16string(org_id));
			request.headers().set_content_type(U("application/json"));
#else
			std::string wbar = ("Bearer ");
			wbar = wbar + utility::conversions::to_utf8string(std::string(api_key));
			request.headers().add(U("Authorization"), wbar);
			request.headers().add(U("OpenAI-Organization"), org_id);
			request.headers().set_content_type(U("application/json"));
#endif
		}
	};
}

