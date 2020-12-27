#include <iostream>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <queue>

using namespace std;

/* TASK 1 */
const int NumTasks = 1024 * 1024;
const int NumThreads[4] = { 4, 8, 16, 32 };


bool Correct(int *arr)
{
    for(int i = 0; i < NumTasks; i++)
        if(!(arr[i] == 1)) return false;
    return true;
}

/* MUTEX */

mutex mtx;
int pointer = 0;

int safepointer()
{
	lock_guard<mutex> guard(mtx);
	return pointer++;
}

void processing_with_mutex(int* arr, int sleep = 0)
{
	int loc_ptr = 0;
	while (loc_ptr < NumTasks)
	{
		loc_ptr = safepointer();
		arr[loc_ptr]++;
		if (sleep)
			this_thread::sleep_for(chrono::nanoseconds(sleep));
	}
}

void task1_with_mutex(int *arr)
{
	
	for (int i = 0; i < 4; i++)
	{
		pointer = 0;
		for(int j = 0; j < NumTasks; j++)
		    arr[j] = 0;
		
		auto start = chrono::high_resolution_clock::now();
		
		thread* threads = new thread[NumThreads[i]];
		    for (int j = 0; j < NumThreads[i]; j++)
			    threads[j] = thread(processing_with_mutex, arr, 0);
		
		for (int j = 0; j < NumThreads[i]; j++)
			threads[j].join();
			
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double> duration = end - start;
		cout << NumThreads[i] << " threads with mutex duration: " << duration.count() << "\t";
		if(Correct(arr)) 
		    cout << "OK" << endl;
		else 
		    cout << "INCORRECT" << endl;
		delete[] threads;
	}
	
	for (int i = 0; i < 4; i++)
	{
		pointer = 0;
		for(int j = 0; j < NumTasks; j++)
		    arr[j] = 0;
		
		auto start = chrono::high_resolution_clock::now();
		
		thread* threads = new thread[NumThreads[i]];
		    for (int j = 0; j < NumThreads[i]; j++)
			    threads[j] = thread(processing_with_mutex, arr, 10);
		
		for (int j = 0; j < NumThreads[i]; j++)
			threads[j].join();
			
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double> duration = end - start;
	cout << NumThreads[i] << " threads with mutex duration with 10 ns sleep: " << duration.count() << "\t";
		if(Correct(arr)) 
		    cout << "OK" << endl;
		else 
		    cout << "INCORRECT" << endl;
		delete[] threads;
	}
	
}


/* ATOMIC */
atomic<int> atomic_pointer{0};

int getAtomicPtr(int sleep)
{
    int ptr = ++atomic_pointer;
    if(sleep)
        this_thread::sleep_for(chrono::nanoseconds(sleep));
    return ptr;
}

void processing_with_atomic(int *arr, int sleep)
{
    int loc_ptr = -1;   
    while(loc_ptr<NumTasks)
    {
        loc_ptr = getAtomicPtr(sleep);
        arr[loc_ptr]++;
    }
}

void task1_with_atomic(int* arr)
{
    for (int i = 0; i < 4; i++)
	{
		
		atomic_pointer = -1;
		for(int j = 0; j < NumTasks; j++)
		    arr[j] = 0;
		auto start = chrono::high_resolution_clock::now();
		
		thread* threads = new thread[NumThreads[i]];
		    for (int j = 0; j < NumThreads[i]; j++)
			    threads[j] = thread(processing_with_atomic, arr, 0);
		
		for (int j = 0; j < NumThreads[i]; j++)
			threads[j].join();
			
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double> duration = end - start;
		cout << NumThreads[i] << " threads with atomic duration: " << duration.count() << "\t";
		if(Correct(arr))
		    cout << "OK" << endl;
		else 
		    cout << "INCORRECT" << endl;
		delete[] threads;
	}
	
	for (int i = 0; i < 4; i++)
	{
		
		atomic_pointer = -1;
		for(int j = 0; j < NumTasks; j++)
		    arr[j] = 0;
		auto start = chrono::high_resolution_clock::now();
		
		thread* threads = new thread[NumThreads[i]];
		    for (int j = 0; j < NumThreads[i]; j++)
			    threads[j] = thread(processing_with_atomic, arr, 10);
		
		for (int j = 0; j < NumThreads[i]; j++)
			threads[j].join();
			
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double> duration = end - start;
	    cout << NumThreads[i] << " threads with atomic duration with 10 ns sleep: " << duration.count() << "\t";
		if(Correct(arr))
		    cout << "OK" << endl;
		else 
		    cout << "INCORRECT" << endl;
		delete[] threads;
	}
}

/* TASK 2 */

int ProducerNum[3] = {1, 2, 4};
int ConsumerNum[3] = {1, 2, 4};
int QueueSize[3] = {1, 4, 16};
const int TaskNum = 1024*1024;

class DynamicQueue
{
    public:
        uint size = 0;
        mutex mtx;
        queue<int8_t> q;
        condition_variable cond;
        int active_producers = 0;
        int ResultSum = 0;
        
        void push(int8_t val)
        {
            lock_guard<mutex> guard(mtx);
            q.push(val);
        }
        
        bool empty()
        {
            if(q.empty())
                return true;
            else 
                return false;
        }
        
        bool pop(int8_t& val)
        {
            lock_guard<mutex> lock(mtx);
            if(this->empty())
                return false;
            
            else
            {
                val = q.front();
                q.pop();
               
                return true;
            }
            
        }
        
        
};


class FixedQueue
{
    public:
        uint size;
        mutex mtx;
        queue<int8_t> q;
        condition_variable cond;
        int active_producers = 0;
        int ResultSum = 0;
        
        FixedQueue(uint n, int c)
        {
            size = n;
            active_producers = c;
        }
        
        void push(int8_t val)
        {
            unique_lock<mutex> lock(mtx);
		    cond.wait(lock, [this] {return q.size() < size;});
		    q.push(val);
		    cond.notify_one();
        }
        
        
        
        bool pop(int8_t& val)
        {
            unique_lock<mutex> lock(mtx);
            
            
                if(!cond.wait_for(lock, chrono::milliseconds(1), [this]{return !this->q.empty();}))
                {
                    return false;
                }
                    val = q.front();
                    q.pop();
                    cond.notify_one();
                    return true;
                
        
        }
        
        bool empty()
        {
            unique_lock<mutex> lock(mtx);
            if(q.empty())
                return true;
            else 
                return false;
        }
};


mutex del_mtx;

void produce1(DynamicQueue* q)
{
	for (int i = 0;i < TaskNum;i++) {
		q->push(1);
	}
	//lock_guard<mutex> del(del_mtx);
	unique_lock<mutex> lock(del_mtx);
	q->active_producers--;
	
}

void produce2(FixedQueue* q)
{
	for (int i = 0;i < TaskNum;i++) {
		q->push(1);
	}
	lock_guard<mutex> del(del_mtx);
	q->active_producers--;
	
}

mutex consumer_mtx;
mutex result_mtx;
void consume1(DynamicQueue* q)
{
	int current_result = 0;
	while (q->active_producers > 0 || !q->empty()) {
		int8_t val;
		if (q->pop(val)) {
			current_result += val;
		}
	}
	//lock_guard<mutex> result(result_mtx);
	unique_lock<mutex> result(result_mtx);
	q->ResultSum += current_result;
	
	return;
}

void consume2(FixedQueue* q)
{
	int current_result = 0;
	while (q->active_producers > 0 || !q->empty()) {
		int8_t val;
		if (q->pop(val)) {
			current_result += val;
		}
	}
	lock_guard<mutex> result(result_mtx);
	q->ResultSum += current_result;
	
	return;
}


bool isCorrect(int a, int b)
{
    return a == b;
}

void Task2_dynamic()
{
    for(int i = 0; i < 3; i++)
	{
	    DynamicQueue q;
	    q.active_producers = ProducerNum[i];
	    
	    thread *consumers = new thread[ConsumerNum[i]];
	    thread *producers = new thread[ProducerNum[i]];
	    
	    for(int j = 0; j < ConsumerNum[i]; j++)
	    {
	        producers[j] = thread(produce1, &q);
	        consumers[j] = thread(consume1, &q);
	        
	    }
	    auto start = chrono::high_resolution_clock::now();
	    
	    for(int j = 0; j < ConsumerNum[i]; j++)
	    {
	        producers[j].join();
	        consumers[j].join();
	    }
	    
	    
	    isCorrect(q.ResultSum, TaskNum*ProducerNum[i])?cout << "OK\n": cout << "INCORRECT" << endl;
	    auto end = chrono::high_resolution_clock::now();
	    chrono::duration<float> duration = end - start;
	    cout << "Duration: " << duration.count() << endl;
	    
	}
}

void Task2_Fixed()
{
    for(int i = 0; i < 3; i++)
	{
	    FixedQueue q(QueueSize[i], ProducerNum[i]);
	    q.active_producers = ProducerNum[i];
	    
	    thread *consumers = new thread[ConsumerNum[i]];
	    thread *producers = new thread[ProducerNum[i]];
	    
	    for(int j = 0; j < ConsumerNum[i]; j++)
	    {
	        producers[j] = thread(produce2, &q);
	        consumers[j] = thread(consume2, &q);
	        
	    }
	    auto start = chrono::high_resolution_clock::now();
	    
	    for(int j = 0; j < ConsumerNum[i]; j++)
	    {
	        producers[j].join();
	        consumers[j].join();
	    }
	    
	    isCorrect(q.ResultSum, TaskNum*ProducerNum[i])?cout << "OK\n": cout << "INCORRECT" << endl;
	    auto end = chrono::high_resolution_clock::now();
	    chrono::duration<float> duration = end - start;
	    cout << "Duration: " << duration.count() << endl;
	    
	}
}



int main()
{
	int *arr = new int[NumTasks] {0};
	cout << "Task 1\n" << endl;
	task1_with_mutex(arr);
	task1_with_atomic(arr);
	cout << "\nTask 2\n" << endl;
	cout << "\tDynamic Queue\n";
	Task2_dynamic();
	cout << "\tFixed Queue\n";
    Task2_Fixed();
	
	
	return 0;
}
