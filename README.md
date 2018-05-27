# 基于Redis的进程间通信——在C++里使用python的深度学习模型

- 本文主要是为进程间通信提供一种新的思路

- 博客地址:[基于Redis的进程间通信——在C++里使用python的深度学习模型](http://www.xiaomaidong.com/?p=707)

- 本想法来源于[RoboMaster](https://www.robomaster.com/zh-CN)比赛中的神符检测，神符是指一个9宫格的手写体数字(Mnist)或火焰体动态数字，需要用到机器学习或深度学习模型对数字进行识别，从检测大符到识别全部数字到发射子弹，整个过程不能超过1.5秒，全部的运算量都集中在搭载在机器小车上的miniPC，由于miniPC性能有限，且除了这个程序之外还有个非常占用资源的自动射击程序，所以整个程序使用的是C++的代码。一开始我们使用的是xgboost，在python上把模型训练完成之后，把预测代码再改写成C++。而后来发现xgboost在真实场景上的准确率较低(只有70%)，于是改为了复杂的深度学习模型，准确率上几乎可以达到100%。但此时的python代码已经比较复杂，再全部改成C++已经不现实，所以开始寻找一种能实现两者间通信的方案。

### 网上已有的通信方案

- C++ 与python的混合编程方案比较多，比如使用<python.h>，说实话，我没认真看，需要的自己去百度或谷歌吧

### 基于Redis的通信方案

#### 基本思路

- Redis是一种基于内存的NoSQL 数据库，所以读写速度非常快，且使用非常简单。
- 在本应用场景中，每次需要通信的数据是9张图片，每张图片是28x28像素，所以其实就是 9 x 28 x 28的矩阵
- 所以，在C++程序中进行检测和提取图像，将9张图片的像素值依次读取进来，存储进Redis，通过标志位的方式告诉python程序进行读取，完成预测之后再将结果存储进redis，并让C++程序进行调用。整个程序的活动图如下：
- 得益于redis的高效读取，在没有gpu的加速下，在mac i7 16G的配置上跑完一次读取和预测大概需要0.3秒，即使在性能较差的miniPC上，一次耗时大概在0.6秒，主要时间消耗都在预测上，连接和读取的时间几乎可以忽略不计


#### 代码实现

- C++ 使用hiredis

		mac上安装方法：brew install hiredis
		ubuntu安装方法：apt-get install hiredis

- demo1: 连接数据库

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

- demo2: 测试redis lrange 操作

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

- 更多demo 参考[HiredisTest](https://github.com/Maicius/HiredisTest/blob/master/main.cpp)

- 一点经验：由于关于hiredis的文档比较少，所以很多参数、返回值不得不自己进行测试，通过阅读源代码是一种方式，我比较喜欢的是通过jetbrain CLion的debug模式进行查看。如下图，在不清楚hiredis的lrange返回的参数的情况下，我通过在返回值那里设断点，通过添加监视等操作来判断如何正确获取返回值。并且，Clion是可以跨平台的，对学生免费。

##### Python 使用redis

- 这个非常简单，仅仅在这里仅提供一个[demo](https://github.com/Maicius/UniversityRecruitment-sSurvey/blob/master/jedis/jedis.py),这个demo是我自己写某个比较大的爬虫项目自己封装的一系列接口。

##### Redis 踩坑笔记

以前写的一篇博客，记录了Redis使用过程中的一些坑,欢迎交流
[Redis 踩坑笔记](http://www.xiaomaidong.com/?p=308)
