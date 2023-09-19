#include <curl/curl.h>
#include <spdlog/spdlog.h>

/*
    interface overview :
        curl_global_init(long flag)
        flag:
            CURL_GLOBAL_ALL                      initialize every invoking possible
            CURL_GLOBAL_SSL                      initialize safe socket
            CURL_GLOBAL_WIN32                    initialize win32 socket
            CURL_GLOBAL_NOTHING                  no specail func

        Description:
            use this function initalize libcurl. you master invoke this api before you use other api. 
            
-------------------------------------------------------------------------------------------------------
        CURL *curl_easy_init() 
        
        Description:
            this function allocates and return a CURL ptr. This call must have a corresponding call 
            to curl_easy_cleanup when the operation is complete.

            If you did not already call curl_global_init before calling this function, curl_easy_init 
            does it automatically. This maybe lethal in multi-threaded cases, if curl_global_init is 
            not thread-safe in your program, and it may then result in resource problems because there 
            is no corresponding cleanup

        Return Value:
            if this function return NULL, something went wrong and you cannot use the other curl functions

--------------------------------------------------------------------------------------------------------
        CURLcode curl_easy_setopt(CURL* handle, CURLoption option, parameter);

        Description:
            curl_easy_setopt is used to tell libcurl how to behave. By setting the appropriate options,
            All options are set with an option followed by a parameter. That parameter depend on what 
            the specific option expects. You can only set one option in each function call. A typical 
            application uses many curl_easy_setopt calls in the setup phase.

            Options set with this function call are vaild for all forthcoming performed using this handle.
            The options are not in any way reset between transfers, so if you want subsequent transfers
            with different options, you must change them between the transfers. You can optionally reset
            all options back to interal default with curl_easy_reset interface.

            Strings passed to libcurl as 'char*' args, are copied by the library. The string storage
            associated to the pointer argument may be discarded or reused after curl_easy_setopt returns.
            The only exception to this rule is really CURLOPT_POSTFIELDS, but the alternative that copies
            the string CURLOPT_COPYPOSTFIELDS has come usage characteristics you need to read up on. This 
            function does not accpet input strings longer than CURL_MAX_INPUT_LENGTH(8 MB)

        Behavior options used in this project:
            CURLOPT_URL
                Use URL to download
                Example:
                    CURL *curl = curl_easy_init();
                    if(curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
                    
                    curl_easy_perform(curl);
                    }

            CURLOPT_WRITEFUNCTION
                Set write callback function invoked when url download action
                Templete code:
                    CURLcode curl_easy_setopt(CURL* handle, CURLOPT_WRITEFUNCTION, write_callback_function);
                Example:
                    struct memory {
                        char* respond = nullptr;
                        size_t size = 0;
                    };

                    static size_t cb(void* data, size_t size, size_t memb, void* clientp) {
                        size_t realsize = size * memb;
                        struct memory* mem = (struct memory*) clientp;

                        char* ptr = realloc(mem->respond, mem->size + realsize + 1);
                        if (ptr == NULL) {
                            spdlog::warn("respond realloc error! out of memb");
                            return 0;
                        }

                        memb->respond = ptr;
                        memcpy(&(mem->respond[mem->size]), data, realsize);
                        mem->size += realsize;
                        mem->respond[mem->size] = 0;

                        return realsize;
                    }

                    struct memory chunk = {0};
                    CURLcode res;
                    CURL* curl = curl_easy_init();

                    if (curl) {
                        // send all data to this function 
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);

                        // pass chunk to the callback function
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

                        // send a request to action
                        res = curl_easy_perform(curl_handle);

                        // free memory
                        free(chunk.respond);

                        curl_easy_cleanup(curl);
                    }

        Return value:
            CURLE_OK(0) means that the option was set properly, non-zero means an error occurred as
            <curl/curl.h> defines. See the libcurl-errors man page for the full list with descriptions.
            
--------------------------------------------------------------------------------------------------------
        CURLcode curl_easy_perform(CURL* easy_handle)

        Description:
            curl_easy_perform - perform a file transfer synchronously.
            curl_easy_perform performs a network in a blocking manner and returns when done, or earlier
            if fails. For non-blocking behavior, see curl_multi_perform.

            Invoke this function after curl_easy_init and all the curl_easy_setopt calls are made, and
            it performs the transfer as described in the options. It must be called with the same easy_handle
            as input as the curl_easy_init call returned.

            You can do any amount of calls to curl_easy_perform while using the same easy_handle. If you intend
            to transfer more than one file, you are even encouraged to do so. Libcurl attempts to reuse existing
            connections for the following transfers, thus making the operations faster, less CPU intense and 
            using less network resources. You probably want to use curl_easy_setopt between the invokes to set
            options for the following curl_easy_perform call.

            You must never call this function simulaneously from two places using the same easy_handle. Let the
            function return first before invoking it another time. If you want parallel transfers, you must use
            serveral curl easy_handles.

            A network transfer moves data to a peer or from a peer. An application tells libcurl how to receive
            data by setting the CURLOPT_WRITEFUNTION and CURLOPT_WRITEDATA options. To tell libcurl what data to
            send, there are a few more alternatives but two common ones are CURLOPT_READFUNCTION and CURLOPT_POSTFIFLEDS.

            While the easy_handle is added to a multi handle, it cannot be used by curl_easy_perform.

        Example:
            CURL *curl = curl_easy_init();
            if(curl) {
            CURLcode res;
            curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            }
        
        Return value:

            CURLE_OK(0) means everthing OK, non-zero means an error occurred as <curl/curl.h> defines-see libcurl-errors.
            If the CURLOPT_ERRORBUFFER was set with curl_easy_setopt there is a readable error message stored in the
            error buffer when non-zero is returned.

--------------------------------------------------------------------------------------------------------
        void curl_easy_clenup(CURL* handle)

        Description:
            This funtion is the opposite of curl_easy_init. It closes down and frees all resources previously
            associated with this easy handle.

            This call closes all connections this handle has used and possibly has kept open until now unless
            the easy handle was attached to a multi handle while doing the transfers. Do not call this function
            if you intend to transfer more files, reusing handles is a key to good performance with libcurl.

            Occasionally you may get you progress callback or header callback called from within curl_easy_cleanup
            (if previously set for the handle using curl_easy_setopt). Like if libcurl decides to shut down the
            connection and the protocol is of a kind that requires a command/respond sequence before disconnect.
            Example of such protocols are FTP, POP3, and IMAP.

            Any use of the easy handle after this function has been called and have returned, is illegal.

            To close an easy handle that has been used with the mulit interface, make sure to first call curl_multi_remove_handle
            to remove it from the multi handle before it is closed.

            Passing in a NULL ptr in handle makes this function return immediately whit no action.

        Example:
            CURL *curl = curl_easy_init();
            if(curl) {
            CURLcode res;
            curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            }

        Return value:
            Void

--------------------------------------------------------------------------------------------------------

*/ 



//argc https://releases.ubuntu.com/22.04/ubuntu-22.04-live-server-amd64.iso.zsync ubuntu.zsync

size_t writeFunc(void* ptr, size_t size, size_t memb, void* userdata) {
    spdlog::info("writeFunc action! " + (std::to_string)(size * memb));
    // printf("writeFunc %dd", size * memb);
    return size * memb;
}

bool download(const char* url, const char* filename) {
    curl_global_init(CURL_GLOBAL_ALL); 

    CURL* curl = curl_easy_init();
    if (curl == NULL) {
        spdlog::warn("curl_easy_init error! return NULL");
        return false;
    }
    // return a easy interface ptr 

    curl_easy_setopt(curl, CURLOPT_URL, url);                   //set url opt
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);   // third arg is the callback function

    CURLcode res = curl_easy_perform(curl);
    // use this to action a mission
    
    if (res != CURLE_OK) {
        spdlog::warn("curl_easy_perform failed! res: " + std::to_string(res));
        return false;
    } else return true;

    curl_easy_cleanup(curl);
    // use this to clean curl to end a mission (can not ensure thread safety)

    return true;
}

long getDownloadLength(const char* url) {
    long downloadFileLength = 0;
    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLength);
    } else {
        spdlog::warn("getInfo failed!");
        return -1;
    }

    curl_easy_cleanup(curl);
    return downloadFileLength;
}

int main(int argc, const char* argv[]) {
    getDownloadLength(argv[1]);
    // download(argv[1], argv[2]);
    spdlog::info("download function over");
    return 0;
}