#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
std::mutex g_cvMutex;	      //二元互斥信号量
std::condition_variable g_cv; //条件变量

//缓冲区队列/buffer
std::deque<int> g_data_deque;
//缓冲区buffer最大数目
const int MAX_NUM = 30;
//缓冲区指针
int g_next_index = 0;

//生产者，消费者线程个数
const int PRODUCER_THREAD_NUM = 3;
const int CONSUMER_THREAD_NUM = 3;

void producer_thread(int thread_id)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); //线程延时、睡眠
		//对缓冲区队列加锁
		std::unique_lock<std::mutex> lk(g_cvMutex);
		//当缓冲区队列未满时，继续添加数据
		g_cv.wait(lk, []()
			  { return g_data_deque.size() <= MAX_NUM; }); //队列未满
		g_next_index++;					       //指针下移
		g_data_deque.push_back(g_next_index);		       //数据加入队列
		std::cout << "producer_thread: " << thread_id << " producer data: " << g_next_index;
		std::cout << " queue size: " << g_data_deque.size() << std::endl;
		//唤醒其他线程
		g_cv.notify_all();
		//自动释放锁
	}
}

void consumer_thread(int thread_id)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(550));
		//对缓冲区队列加锁
		std::unique_lock<std::mutex> lk(g_cvMutex);
		//检测条件是否达成：队列不为空，有数据
		g_cv.wait(lk, []
			  { return !g_data_deque.empty(); });
		//互斥操作，从队列中取数据
		int data = g_data_deque.front();
		g_data_deque.pop_front();
		std::cout << "\tconsumer_thread: " << thread_id << " consumer data: ";
		std::cout << data << " deque size: " << g_data_deque.size() << std::endl;
		//唤醒其他线程
		g_cv.notify_all();
		//自动释放锁
	}
}

int main()
{
	std::thread arrRroducerThread[PRODUCER_THREAD_NUM];
	std::thread arrConsumerThread[CONSUMER_THREAD_NUM];

	for (int i = 0; i < PRODUCER_THREAD_NUM; i++)
	{
		arrRroducerThread[i] = std::thread(producer_thread, i);
	}

	for (int i = 0; i < CONSUMER_THREAD_NUM; i++)
	{
		arrConsumerThread[i] = std::thread(consumer_thread, i);
	}

	for (int i = 0; i < PRODUCER_THREAD_NUM; i++)
	{
		arrRroducerThread[i].join();
	}

	for (int i = 0; i < CONSUMER_THREAD_NUM; i++)
	{
		arrConsumerThread[i].join();
	}

	return 0;
}
