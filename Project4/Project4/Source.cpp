#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void uploadFileToGoogleDrive(const std::string& accessToken, const std::string& filePath, const std::string& fileName) {
    CURL* curl;
    CURLcode res;
    struct curl_slist* headers = NULL;

    curl = curl_easy_init();
    if (curl) {
        std::ifstream fileStream(filePath, std::ios::binary);
        std::string fileData((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

        std::string url = "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart";

        std::string boundary = "------------------------abc123xyz";
        std::string metadata = "--" + boundary + "\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n\r\n"
            "{ \"name\": \"" + fileName + "\", \"parents\": [\"your_folder_id\"] }\r\n";

        std::string filePart = "--" + boundary + "\r\n"
            "Content-Type: text/html\r\n"
            "Content-Transfer-Encoding: binary\r\n\r\n" + fileData + "\r\n";

        std::string end = "--" + boundary + "--\r\n";
        std::string postData = metadata + filePart + end;

        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, ("Content-Type: multipart/related; boundary=" + boundary).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            std::cout << "File uploaded successfully!" << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
}

int main() {
    std::string accessToken = "your_access_token";
    std::string filePath = "/path/to/your/file.html";
    std::string fileName = "file.html";

    uploadFileToGoogleDrive(accessToken, filePath, fileName);

    return 0;
}
