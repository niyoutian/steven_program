#include "mxSem.h"

/*
信号量：首先是一个变量，其次是计数器。它是多线程环境下使用的一种设施，信号量在创建时需要设置一个初始值，
表示同时可以有几个任务(线程)可以访问某一块共享资源。一个任务要想访问共享资源，前提是信号量大于0，
当该任务成功获得资源后，将信号量的值减 1;若当前信号量的值小于 0，表明无法获得信号量，该任务必须被挂起，
等待信号量恢复为正值的那一刻;当任务执行完之后，必须释放信号量，对应操作就是信号量的值加 1。
另外，对信号量的操作(加、减)都是原子的。互斥锁(Mutex)就是信号量初始值为 1 时的特殊情形，
即同时只能有一个任务可以访问共享资源区。

原文链接：https://blog.csdn.net/qq_40989769/article/details/109621534

*/
/*
https://blog.csdn.net/weixin_33573857/article/details/116953710


int sem_init(sem_t *sem,int pshared,unsigned int value);
返回值 ：
sem_init() 成功时返回 0；错误时，返回 -1，并把 errno 设置为合适的值。

sem_init() 初始化一个定位在 sem 的匿名信号量。value 参数指定信号量的初始值。 
pshared 参数指明信号量是由进程内线程共享，还是由进程之间共享。
如果 pshared 的值为 0，那么信号量将被进程内的线程共享，
并且应该放置在这个进程的所有线程都可见的地址上(如全局变量，或者堆上动态分配的变量)。
如果 pshared 是非零值，那么信号量将在进程之间共享，并且应该定位共享内存区域(见 shm_open(3)、mmap(2) 和 shmget(2))。
因为通过 fork(2) 创建的孩子继承其父亲的内存映射，因此它也可以见到这个信号量。
所有可以访问共享内存区域的进程都可以用 sem_post(3)、sem_wait(3) 等等操作信号量。
初始化一个已经初始的信号量其结果未定义。


*/
mxSem::mxSem(u32_t value)
{
	sem_init(&mSem, 0, value);
}

mxSem::~mxSem()
{
	sem_destroy(&mSem);
}
/*
int sem_post(sem_t *sem);

资源增加1

*/
void mxSem::post(void)
{
	sem_post(&mSem);
}

/*
int sem_post(sem_t *sem);

资源增加1

*/
void mxSem::wait(void)
{
	sem_wait(&mSem);
}

