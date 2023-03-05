#include "ChatGPTREST.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <string_view>
#include <locale> 
#include <codecvt>
#include <cpprest/http_client.h>

using namespace std;
using namespace web;
using namespace web::http;
using namespace web::http::client;

bool cs::ChatGPTREST::connect()
{
    try {
        client = make_unique<http_client>(uri(openai_endpoint.c_str()));
        return true;
    }
    catch (...) {
        return false;
    }
}

void cs::ChatGPTREST::prepare_request(http_request& request)
{
    std::string wbar = ("Bearer ");
    wbar = wbar + utility::conversions::to_utf8string(api_key);
    request.headers().add(U("Authorization"), wbar);
    request.headers().add(U("OpenAI-Organization"), org_id);
    request.headers().set_content_type(U("application/json"));
}

std::string cs::ChatGPTREST::get_text_response(std::string_view prompt)
{
    if (client == nullptr)
        return "";

    json::value root;
    root[U("model")] = json::value::string(U("text-davinci-003"));
    root[U("prompt")] = json::value::string(static_cast<string>(prompt));
    root[U("temperature")] = temperature;
    root[U("max_tokens")] = max_tokens;
    root[U("frequency_penalty")] = frequency_penalty;
    root[U("presence_penalty")] = presence_penalty;

    http_request request(methods::POST);
    prepare_request(request);
    request.set_body(root.serialize().c_str());
    request.set_request_uri(web::uri("/v1/completions"));

    http_response response = client->request(request).get();

    if (response.status_code() == status_codes::OK) {
        json::value response_body = response.extract_json().get();
        return utility::conversions::to_utf8string(response_body[U("choices")][0][U("text")].as_string());
    }
    else {
        return utility::conversions::to_utf8string(response.extract_string().get());
    }
}

bool cs::ChatGPTREST::list_models(std::list<Model>& models)
{
    if (client == nullptr)
        return false;

    http_request request(methods::GET);
    prepare_request(request);
    request.set_request_uri(web::uri("/v1/models"));

    http_response response = client->request(request).get();

    if (response.status_code() == status_codes::OK) {
        json::value response_body = response.extract_json().get();
        auto _models = response_body.at(U("data")).as_array();
        for (size_t i = 0; i < _models.size(); ++i) {
            Model model;

            model.id = _models[i].at(U("id")).as_string();
            model.object = _models[i].at(U("object")).as_string();
            model.owned_by = _models[i].at(U("owned_by")).as_string();

            models.emplace_back(model);
        }

        return true;
    }

    return false;
}

bool cs::ChatGPTREST::get_image(std::string_view prompt, cv::Mat& image)
{
    if (client == nullptr)
        return false;

    bool ret = false;

    json::value root;
    root[U("prompt")] = json::value::string(static_cast<string>(prompt));
    root[U("n")] = 1;
    root[U("size")] = json::value::string(static_cast<string>("256x256"));

    http_request request(methods::POST);
    prepare_request(request);
    request.set_body(root.serialize().c_str());
    request.set_request_uri(web::uri("/v1/images/generations"));

    http_response response = client->request(request).get();
    if (response.status_code() == status_codes::OK) {
        json::value response_body = response.extract_json().get();
        auto _images = response_body.at(U("data")).as_array();
        if (_images.size() > 0) {
            std::string url = _images[0].at(U("url")).as_string();
            ret = load_image(url, image);
        }
    }

    return ret;
}

bool cs::ChatGPTREST::load_image(std::string_view url, cv::Mat& image)
{
    http_client cli(static_cast<string>(url));
    http_response response = cli.request(methods::GET).get();
    if (response.status_code() == status_codes::OK) {
        pplx::task<std::vector<unsigned char>> data = response.extract_vector();
        data.wait();

        std::vector<unsigned char> arr = data.get();
        image = imdecode(cv::Mat(arr), cv::IMREAD_ANYCOLOR);

        return true;
    }

    return false;
}
