/***
 * Copyright (C) Alex Epstine (alex@comsuite.co.il). All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 * For the latest on this and related APIs, please see: https://github.com/ComSuite/openai-api
 ****/

#pragma once

#include <string>
#include <string_view>
#include <list>
#include <codecvt>
#include <cpprest/http_client.h>
#include "chatgptcpprest.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

//https://platform.openai.com/docs/api-reference/completions/create

namespace cs
{
	class Model
	{
	public:
#if defined _WIN32 || defined _WIN64
		std::wstring id = L"";
		std::wstring object = L"";
		std::wstring owned_by = L"";
#else
		std::string id = "";
		std::string object = "";
		std::string owned_by = "";
#endif
	};

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

	template <typename T>
	class chatgptrest
	{
	public:
		bool connect() 
		{
			if (client != nullptr) {
				return client->connect(openai_host);
			}

			return false;
		}

		void set_api_key(std::string_view api_key) 
		{ 
			if (client != nullptr)
				client->set_api_key(api_key);
		}

		void set_org_id(std::string_view org_id) 
		{ 
			if (client != nullptr)
				client->set_org_id(org_id);
		};

		void set_model(std::string_view mod) { model = mod; };
		void set_max_tokens(const uint32_t tokens) { max_tokens = tokens; };
		void set_temperature(const double temp) { temperature = temp; };
		void set_frequency_penalty(const double freq) { frequency_penalty = freq; };
		void set_presence_penalty(const double pres) { presence_penalty = pres; };

		bool get_text(std::string_view prompt, std::string& response) const
		{
			if (client == nullptr)
				return false;

			bool ret = false;

			web::json::value root;
			root[U("model")] = web::json::value::string(FROM_STD_STR(model));  //json::value::string(model);
			root[U("temperature")] = temperature;
			root[U("max_tokens")] = max_tokens;
			root[U("frequency_penalty")] = frequency_penalty;
			root[U("presence_penalty")] = presence_penalty;

#if defined _WIN32 || defined _WIN64
			std::wstring wprompt(prompt.begin(), prompt.end());
			root[U("prompt")] = web::json::value::string(wprompt);
#else
			root[U("prompt")] = web::json::value::string(static_cast<std::string>(prompt));
#endif

			if (client->post(U("/v1/completions"), root.serialize().c_str(), response)) {
				try {
					root = web::json::value::parse(FROM_STD_STR(response));
					response = utility::conversions::to_utf8string(root[U("choices")][0][U("text")].as_string());

					ret = true;
				}
				catch (...) {}
			}

			return ret;
		}

		bool list_models(std::list<Model>& models)
		{
			if (client == nullptr)
				return false;

			bool ret = false;
			std::string response = "";

			if (client->get(U("/v1/models"), response)) {
				web::json::value root = web::json::value::parse(FROM_STD_STR(response));
				auto _models = root.at(U("data")).as_array();
				for (auto& _model : _models) {
					Model mod;

					mod.id = _model.at(U("id")).as_string();
					mod.object = _model.at(U("object")).as_string();
					mod.owned_by = _model.at(U("owned_by")).as_string();

					models.emplace_back(mod);
				}

				ret = true;
			}

			return ret;
		}

		bool get_image(std::string_view prompt, cv::Mat& image)
		{
			if (client == nullptr)
				return false;

			bool ret = false;
			std::string response = "";

			web::json::value root;
			root[U("n")] = 1;
#if defined _WIN32 || defined _WIN64
			std::wstring wprompt(prompt.begin(), prompt.end());
			root[U("prompt")] = web::json::value::string(wprompt);
			root[U("size")] = web::json::value::string(U("256x256"));
#else
			root[U("prompt")] = web::json::value::string(static_cast<std::string>(prompt));
			root[U("size")] = web::json::value::string(static_cast<std::string>("256x256"));
#endif

			if (client->post(U("/v1/images/generations"), root.serialize().c_str(), response)) {
				root = web::json::value::parse(FROM_STD_STR(response));
				auto _images = root.at(U("data")).as_array();
				if (_images.size() > 0) {
#if defined _WIN32 || defined _WIN64
					std::wstring wurl = _images[0].at(U("url")).as_string();
					using convert_type = std::codecvt_utf8<wchar_t>;
					std::wstring_convert<convert_type, wchar_t> converter;
					std::string url = converter.to_bytes(wurl);
					ret = load_image(url, image);
#else
					std::string url = _images[0].at(U("url")).as_string();
					ret = load_image(url, image);
#endif
				}
			}

			return ret;
		}

		bool chat(std::string_view role, std::string_view content, std::string& response)
		{
			if (client == nullptr)
				return false;

			bool ret = false;

			web::json::value messages;
			messages[0][U("role")] = web::json::value::string(FROM_STD_STR(static_cast<std::string>(role)));
			messages[0][U("content")] = web::json::value::string(FROM_STD_STR(static_cast<std::string>(content)));

			web::json::value root;
			root[U("n")] = 1;
			root[U("model")] = web::json::value::string(FROM_STD_STR(model));
			root[U("temperature")] = temperature;
			root[U("max_tokens")] = max_tokens;
			root[U("frequency_penalty")] = frequency_penalty;
			root[U("presence_penalty")] = presence_penalty;
			root[U("messages")] = messages;

			if (client->post(U("/v1/chat/completions"), root.serialize().c_str(), response)) {
				try {
					root = web::json::value::parse(FROM_STD_STR(response));
					response = utility::conversions::to_utf8string(root[U("choices")][0][U("message")][U("content")].as_string());

					ret = true;
				}
				catch (...) {}
			}

			return ret;
		}
	private:
		bool load_image(std::string_view url, cv::Mat& image) const
		{
			std::shared_ptr<T> clt = std::make_shared<T>();
			if (clt == nullptr)
				return false;

#if defined _WIN32 || defined _WIN64
			std::wstring wurl(url.begin(), url.end());
			if (!clt->connect(wurl))
				return false;
#else
			if (!clt->connect(static_cast<std::string>(url)))
				return false;
#endif
			bool ret = false;
			std::vector<unsigned char> data;

			if (clt->get(data)) {
				image = imdecode(cv::Mat(data), cv::IMREAD_ANYCOLOR);
				ret = true;
			}

			return ret;
		}

		std::shared_ptr<T> client = std::make_shared<T>(); //FROM_STD_STR(openai_endpoint)

		std::string model = "text-davinci-003";
		//std::string api_key = "";
		//std::string org_id = "";
		uint32_t max_tokens = 4000;
		double temperature = 0; //from 0 to 2
		double frequency_penalty = 0.2; //from -2 to 2
		double presence_penalty = 0; //from -2 to 2

#if defined _WIN32 || defined _WIN64
		const std::wstring openai_host = L"https://api.openai.com";
#else
		const std::string openai_host = "https://api.openai.com";
#endif
	};
}
