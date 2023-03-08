#include "chatgptrest_api.h"
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

bool cs::chatgptrest::connect()
{
    try {
        client = make_unique<http_client>(web::uri(static_cast<const utility::char_t*>(openai_endpoint.c_str())));
        return true;
    }
    catch (...) {
        return false;
    }
}

void cs::chatgptrest::prepare_request(http_request& request) const
{
#if defined _WIN32 || defined _WIN64
    std::wstring wbar = (U("Bearer "));
    wbar = wbar + utility::conversions::to_utf16string(api_key);
    request.headers().add(U("Authorization"), wbar);
    request.headers().add(U("OpenAI-Organization"), utility::conversions::to_utf16string(org_id));
    request.headers().set_content_type(U("application/json"));
#else
    std::string wbar = ("Bearer ");
    wbar = wbar + utility::conversions::to_utf8string(api_key);
    request.headers().add(U("Authorization"), wbar);
    request.headers().add(U("OpenAI-Organization"), org_id);
    request.headers().set_content_type(U("application/json"));
#endif
}

std::string cs::chatgptrest::get_text_response(std::string_view prompt)
{
    if (client == nullptr)
        return "";

    json::value root;
    root[U("model")] = json::value::string(U("text-davinci-003"));

#if defined _WIN32 || defined _WIN64
    std::wstring wprompt(prompt.begin(), prompt.end());
    root[U("prompt")] = json::value::string(wprompt);
#else
    root[U("prompt")] = json::value::string(static_cast<std::string>(prompt));
#endif

    root[U("temperature")] = temperature;
    root[U("max_tokens")] = max_tokens;
    root[U("frequency_penalty")] = frequency_penalty;
    root[U("presence_penalty")] = presence_penalty;

    http_request request(methods::POST);
    prepare_request(request);
    request.set_body(root.serialize().c_str());
    request.set_request_uri(web::uri(U("/v1/completions")));

    http_response response = client->request(request).get();

    if (response.status_code() == status_codes::OK) {
        json::value response_body = response.extract_json().get();
        return utility::conversions::to_utf8string(response_body[U("choices")][0][U("text")].as_string());
    }
    else {
        return utility::conversions::to_utf8string(response.extract_string().get());
    }
}

bool cs::chatgptrest::list_models(std::list<Model>& models)
{
    if (client == nullptr)
        return false;

    http_request request(methods::GET);
    prepare_request(request);
    request.set_request_uri(web::uri(U("/v1/models")));

    http_response response = client->request(request).get();

    if (response.status_code() == status_codes::OK) {
        json::value response_body = response.extract_json().get();
        auto _models = response_body.at(U("data")).as_array();
        for (auto& _model : _models) {
            Model mod;

            mod.id = _model.at(U("id")).as_string();
            mod.object = _model.at(U("object")).as_string();
            mod.owned_by = _model.at(U("owned_by")).as_string();

            models.emplace_back(mod);
        }

        return true;
    }

    return false;
}

bool cs::chatgptrest::get_image(std::string_view prompt, cv::Mat& image)
{
    if (client == nullptr)
        return false;

    bool ret = false;

    json::value root;
    root[U("n")] = 1;
#if defined _WIN32 || defined _WIN64
    std::wstring wprompt(prompt.begin(), prompt.end());
    root[U("prompt")] = json::value::string(wprompt);
    root[U("size")] = json::value::string(U("256x256"));
#else
    root[U("prompt")] = json::value::string(static_cast<string>(prompt));
    root[U("size")] = json::value::string(static_cast<string>("256x256"));
#endif
    http_request request(methods::POST);
    prepare_request(request);
    request.set_body(root.serialize().c_str());
    request.set_request_uri(web::uri(U("/v1/images/generations")));

    http_response response = client->request(request).get();
    if (response.status_code() == status_codes::OK) {
        json::value response_body = response.extract_json().get();
        auto _images = response_body.at(U("data")).as_array();
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

bool cs::chatgptrest::load_image(std::string_view url, cv::Mat& image)
{
#if defined _WIN32 || defined _WIN64
    std::wstring wurl(url.begin(), url.end());
    http_client cli(wurl);
#else
    http_client cli(static_cast<string>(url));
#endif
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
