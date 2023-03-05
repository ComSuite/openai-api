#pragma once

#include <string>
#include <string_view>
#include <list>
#include <cpprest/http_client.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

//https://platform.openai.com/docs/api-reference/completions/create
//https://github.com/microsoft/cpprestsdk

namespace cs
{
	class Model
	{
	public:
		std::string id = "";
		std::string object = "";
		std::string owned_by = "";
	};

	class ChatGPTREST
	{
	public:
		bool connect();

		void set_model(std::string_view mod) { model = mod; };
		void set_api_key(std::string_view key) { api_key = key; };
		void set_org_id(std::string_view id) { org_id = id; };
		void set_max_tokens(const uint32_t tokens) { max_tokens = tokens; };
		void set_temperature(const double temp) { temperature = temp; };
		void set_frequency_penalty(const double freq) { frequency_penalty = freq; };
		void set_presence_penalty(const double pres) { presence_penalty = pres; };

		std::string get_text_response(std::string_view prompt);
		bool list_models(std::list<Model>& models);
		bool get_image(std::string_view promptm, cv::Mat& image);
	private:
		void prepare_request(web::http::http_request& request);
		bool load_image(std::string_view url, cv::Mat& image);

		std::unique_ptr<web::http::client::http_client> client = nullptr;

		std::string model = "text-davinci-003";
		std::string api_key = "";
		std::string org_id = "";
		uint32_t max_tokens = 4000;
		double temperature = 0; //form 0 to 2
		double frequency_penalty = 0.2; //fromn -2 to 2
		double presence_penalty = 0; //fromn -2 to 2

		const std::string openai_endpoint = "https://api.openai.com";
	};
}
