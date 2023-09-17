#include <curl/curl.h>
#include <spdlog/spdlog.h>

//argc https://releases.ubuntu.com/22.04/ubuntu-22.04-live-server-amd64.iso.zsync ubuntu.zsync

size_t writeFunc(void* ptr, size_t size, size_t memb, void* userdata) {
    spdlog::info("writeFunc action! " + (std::to_string)(size * memb));
    // printf("writeFunc %dd", size * memb);
    return size * memb;
}

bool download(const char* url, const char* filename) {
    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);

    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        spdlog::warn("curl_easy_perform failed! res: " + std::to_string(res));
        return false;
    } else return true;

    curl_easy_cleanup(curl);

    return true;
}

int main(int argc, const char* argv[]) {
    download(argv[1], argv[2]);
    spdlog::info("download function over");
    return 0;
}