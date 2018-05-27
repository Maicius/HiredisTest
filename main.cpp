#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <utility>
#include <opencv/cv.hpp>
#include <hiredis/hiredis.h>
using namespace std;
using namespace cv;

const char *IMAGE_NAME_LIST = "IMAGE_NAME_LIST";
const char *IMAGE_RESULT = "IMAGE_RESULT";

// 连接redis
redisContext* connect_redis();

// 具体执行redis的函数
void execute_redis(redisContext *context, const char* cmd1);

// 读取图片像素值到redis
void save_image_in_redis(redisContext *context, Mat &img, const char* image_name);

// 测试 redis lrange操作
void test_redis_lrange(redisContext *context);

// 测试 redis lpush操作
void test_redis_lpush();

// // 测试 redis lpush操作
void execute_redis_lpush(const char* image_name);

// 测试redis set 操作
void test_set();
int main(){
    //redisContext *context = connect_redis();
//    if(context != NULL){
//        test_redis(context);
//    }
//    redisReply *length = (redisReply *)redisCommand(context, "llen ysu_company_info");
//    cout << length -> integer<<endl;
//    cout << "hello" <<endl;
//    test_redis_lrange(context);
//    test_redis_lpush();
//    execute_redis_lpush("cell_10");
//    Mat image = imread("../11.jpg");
//    Mat gray_image;
//    cvtColor(image, gray_image, CV_BGR2GRAY);
    //save_image_in_redis(context, gray_image, "IMAGE_11");
    test_set();
}

void test_redis_lrange(redisContext *context){
    redisReply *result = (redisReply *)redisCommand(context, "lrange IMAGE_NAME_LIST 0 -1");
    redisReply **elements = result->element;
    size_t size = result->elements;
    for(int i=0; i < size; i++){
        string re = elements[i]->str;
        int r = re.at(7) - 48;
        cout << "r:" << r<<endl;
    }
    freeReplyObject(result);
    cout << endl;
}

void test_redis_lpush(){
    redisContext *redisContext1 = connect_redis();
    const char* image_name_cell8 = "cell8";
    const char* save_image_name = ("LPUSH " + (string)IMAGE_NAME_LIST + " " + (string)image_name_cell8).c_str();
    printf("cmd cell8:%s\n",save_image_name);
    redisReply *result = (redisReply *)redisCommand(redisContext1, save_image_name);
    if(result == NULL) {
        printf("execute failed:%s", save_image_name);
    }
    else {
        cout << "str result" << endl;
        freeReplyObject(result);
        redisFree(redisContext1);
    }
}

void test_set(){
    redisContext *context = connect_redis();
    const char* save_finish = "SET SAVE 1";
    redisReply *result = (redisReply *)redisCommand(context, save_finish);
    freeReplyObject(result);
    const char* save_finish2 = "SET SAVE 2";
    redisReply *result2 = (redisReply *)redisCommand(context, save_finish2);
    freeReplyObject(result2);
    redisFree(context);
}

void execute_redis_lpush(const char* lpush_image_name){
    cout << "enter lpush" <<endl;
    redisContext *redisContext2 = connect_redis();
    const char* LPUSH = ("LPUSH " + (string)IMAGE_NAME_LIST + " " + (string)lpush_image_name).c_str();
    printf("cmd cell10:%s\n", LPUSH);
    redisReply *reply = (redisReply *)redisCommand(redisContext2, LPUSH);
    if(reply == NULL){
        printf("execute failed");
    }else {
        cout <<"str result:" << reply->str << endl;
    }
    freeReplyObject(reply);
    redisFree(redisContext2);
}

redisContext* connect_redis(){
    redisContext *context = redisConnect("127.0.0.1", 6379);
    if(context->err) {
        redisFree(context);
        printf("connect redisServer err:%s\n", context->errstr);
        return NULL;
    }
    printf("connect redisServer success\n");
    return context;
}

void execute_redis(redisContext *context, const char* cmd1){
//    const char *cmd = "SET TEST_IMAGE 100";
    redisReply *reply1 = (redisReply *)redisCommand(context, cmd1);

    if(NULL == reply1) {
        printf("command execute failure\n");
        //redisFree(context);
        return ;
    }

    freeReplyObject(reply1);
    printf("%s execute success\n", cmd1);
}

void save_image_in_redis(redisContext *context, Mat &img, const char* image_name){
    string image_data = "";
    for (int b = 0; b < 28; b++)
    {
        for (int x = 0; x < 28; x++)
        {
            image_data += to_string((int)img.at<uchar>(b, x)) + "-";
        }
    }
    //cout << image_data <<endl;
    //保存图片名称
    const char *save_image_name = ("LPUSH " + (string)IMAGE_NAME_LIST + " " + (string)image_name).c_str();
    execute_redis(context, save_image_name);

    const char * save_image_data = ("SET " + string(image_name) + " " + image_data).c_str();
    execute_redis(context, save_image_data);
}
