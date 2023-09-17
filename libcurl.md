# libCurl API

```
//设置请求的url【这是唯一要求必须设置的选项，不然咋知道是向谁请求或发送数据】
// Even if you set a crazy value here, curl_easy_setopt will still return CURLE_OK.
// 该url相当于浏览器输入的地址，如果可以带文件，执行后就可以下载文件
curl_easy_setopt(curl_handler, CURLOPT_URL, url);
 
//设置是否返回请求头
// 设置的值为1，意味要返回请求头；0表示不返回。无其他值
curl_easy_setopt(curl_handler, CURLOPT_HEADER, 1L);
 
//设置屏蔽其他信号
curl_easy_setopt(curl_handler, CURLOPT_NOSIGNAL, 1L);
 
//设置下载数据回调函数
// 若置为NULL，则其默认值为fwrite
/*
回调函数原型：
    size_t function( void *ptr, size_t size, size_t nmemb, void *userp);
        函数将在libcurl接收到数据后被调用。
        void *ptr是下载回来的数据.
        void *userp是用户指针, 用户通过这个指针传输自己的数据.
CURLOPT_WRITEDATA：设置回调函数中的void *userp指针的来源。
*/
curl_easy_setopt(curl_handler, CURLOPT_WRITEFUNCTION, write_func);
// 如果设置这个值，则必须怼个CURLOPT_WRITEFUNCTION，否则会崩溃
// If you're using libcurl as a win32 DLL, you MUST use a CURLOPT_WRITEFUNCTION if you set this option or you will experience crashes.
curl_easy_setopt(curl_handler, CURLOPT_WRITEDATA, write_data);
 
 
//设置是否使用下载进度控制函数
// 为0，表示把下载进度显示在控制台；为1，表示不显示下载进度
curl_easy_setopt(curl_handler, CURLOPT_NOPROGRESS, 0L);
curl_easy_setopt(curl_handler, CURLOPT_PROGRESSFUNCTION, prog_func);
curl_easy_setopt(curl_handler, CURLOPT_PROGRESSDATA, pdata);
// 下载进度回调函数与下载数据的回调函数原型相同，data也相同。
 
//设置请求头
struct curl_list *header_list = NULL;
header_list = curl_slist_append(header_list, headers);
 
curl_easy_setopt(curl_handler, CURLOPT_HTTPHEADER, header_list);
curl_slist_free_all(header_list);
 
//其他选项
     CURLOPT_HEADERFUNCTION
// 只取HTTP头部数据, 处理与下载数据回调的处理相同. 
     CURLOPT_HEADERDATA
// 超时时间
     CURLOPT_TIMEOUT
//   连接等待时间.
     CURLOPT_CONNECTIONTIMEOUT   
// 设置支持302重定向
     CURLOPT_FOLLOWLOCATION
// 断点续传, 指定传输分片, 格式:"0-200"　   
　　 CURLOPT_RANGE

```