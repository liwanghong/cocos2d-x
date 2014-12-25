//
//  LambdaClient.cpp
//  cocos2d_libs
//
//  Created by sky4star on 14/12/14.
//
//

#include "LambdaClient.h"

using namespace lambdacloud;

const char* c_url = "http://api.lambdacloud.com/log";
const std::string c_jsonHeader = "Content-Type: application/json";
static LambdaClient* s_lambdaClient = nullptr;
std::string m_token;

LambdaClient* LambdaClient::getInstance()
{
    if (s_lambdaClient == nullptr) {
        s_lambdaClient = new (std::nothrow) LambdaClient();
    }
        
    return s_lambdaClient;
}

void LambdaClient::setToken(const std::string& token)
{
    m_token = token;
    CCLOG("set token %s", token.c_str());
}

void LambdaClient::debugLog()
{
    CCLOG("This is a debug log");
}

void LambdaClient::writeLog(const std::string& log, std::vector<std::string> tags)
{
    cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
    
    CCLOG("Lambda client will write log:%s to server", log.c_str());
    
    // Set headers
    std::vector<std::string> pHeaders;
    pHeaders.push_back("Token: " + m_token);
    pHeaders.push_back(c_jsonHeader);
    request->setHeaders(pHeaders);
    
    // Set url and request type
    request->setUrl(c_url);
    request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
    
    // Set data
    std::string jsonContent = generateJsonData(log, tags);
    request->setRequestData(jsonContent.c_str(), jsonContent.length());
    
    // Set callback and send out
    request->setResponseCallback(CC_CALLBACK_2(LambdaClient::onHttpRequestCompleted, this));
    cocos2d::network::HttpClient::getInstance()->send(request);
    
    //TODO why shall we call release explicitly?
    request->release();
    return;
}

std::string LambdaClient::generateJsonData(const std::string& log, std::vector<std::string> tags)
{
    rapidjson::Document document;
    document.SetObject();
    document.AddMember("Message", log.c_str(), document.GetAllocator());
    rapidjson::Value tagObjs;
    tagObjs.SetArray();
    for (int i=0; i<tags.size(); i++)
    {
        tagObjs.PushBack(tags[i].c_str(), document.GetAllocator());
    }
    document.AddMember("Tags", tagObjs, document.GetAllocator());
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return buffer.GetString();
}

void LambdaClient::onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
    //TODO shall we record some logs for lambda request callback
    // Check not null
    if (!response)
    {
        CCLOG("response in null in onHttpRequestCompleted");
        return;
    }
    
    // Check if completed
    if (!response->isSucceed())
    {
        CCLOG("request failed with message %s", response->getErrorBuffer());
        return;
    }
    else
    {
        CCLOG("response status code:%ld", response->getResponseCode());
        CCLOG("response data:%s", response->getResponseData()->data());
        return;
    }

    
}

LambdaClient::LambdaClient()
{
    // Nothing to init at present
}

LambdaClient::~LambdaClient()
{
    // Nothing to dispose at present
}


