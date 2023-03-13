#include <iostream>
#include "chatgptrest_api.h"
#include <opencv2/highgui.hpp>
#if !defined _WIN32 && !defined _WIN64
#include <sys/utsname.h>
#endif

using namespace cs;

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 1;
    }

    auto rest = std::make_unique<chatgptrest>();
    if (rest == nullptr) {
        return 1;
    }

    rest->set_api_key(std::getenv("OPENAI_API_KEY"));
    rest->set_org_id(std::getenv("OPENAI_ORGANIZATION_ID"));
    if (!rest->connect()) {
        return 1;
    }

/*
    std::list<Model> models;
    if (rest->list_models(models)) {
        for (auto& model : models) {
#if defined _WIN32 || defined _WIN64
            std::wcout << model.id << std::endl << model.object << std::endl << model.owned_by << std::endl << std::endl;
#else
            std::cout << model.id << std::endl << model.object << std::endl << model.owned_by << std::endl << std::endl;
#endif
        }
    }
*/

    std::string response = "";

    rest->set_model("text-davinci-003");
    rest->get_text(argv[1], response);
    std::cout << response << std::endl;

    rest->set_model("gpt-3.5-turbo");
    rest->chat("user", argv[1], response);
    std::cout << response << std::endl;

    //cv::Mat img;
    //std::string prompt = argv[1];
    //rest->get_image(prompt, img);
    
    //cv::imwrite("test.png", img);
    //cv::imshow("DALL-E", img);
    //cv::waitKey(0);

    return 0;
}