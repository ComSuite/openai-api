#include "chatgptrest_api.h"
#include <opencv2/highgui.hpp>

using namespace cs;

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 1;
    }

    auto rest = std::make_unique<chatgptrest>();
    if (rest == nullptr) {
        return 0;
    }

    rest->set_api_key(std::getenv("OPENAI_API_KEY"));
    rest->set_model("text-davinci-003");
    rest->set_org_id(std::getenv("OPENAI_ORGANIZATION_ID"));
    rest->connect();

    //std::list<Model> models;
    //rest->list_models(models);
    //for (auto model : models) {
    //    std::cout << model.id << std::endl << model.object << std::endl << model.owned_by << std::endl << std::endl;
    //}

    cv::Mat img;
    std::string prompt = argv[1];
    rest->get_image(prompt, img);
    //cv::imwrite("test.png", img);
    cv::imshow("DALL-E", img);
    cv::waitKey(0);

    return 0;
}