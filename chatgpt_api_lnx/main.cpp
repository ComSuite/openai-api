#include "ChatGPTREST.h"
#include <opencv2/highgui.hpp>

//std::wstring org_id() const
//{
//    std::wstring ret = std::format(L"OpenAI-Organization: {}", organization_id.c_str());
//    return ret;
//}
using namespace cs;

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 1;
    }

    auto rest = std::make_unique<ChatGPTREST>();
    if (rest == nullptr) {
        return 0;
    }

    rest->set_api_key("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");
    rest->set_model("text-davinci-003");
    rest->set_org_id("org-0wG2sCPzC10T6rW2uXprsrDK");
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
    int k = cv::waitKey(0);

    return 0;
}