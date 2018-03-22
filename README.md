# HuffMan-
HuffMan编码实现压缩算法，作了文件IO优化(缓冲),用了C++ STL Map数据结构和优先队列priority queue

1、文件IO优化-缓冲：
    由于文件的IO读写操作时间要远大于CPU运算时间，当文件的IO次数很多时，程序性能也会随之下降，所以可以考虑减少文件的IO次数，使用缓冲来读写文件，避免每次只读写单个字符等情况。
如分配一个比较大空间的字符数组，如1024b,10M等char *charbuffer=new char[1024]，
然后一次从文件流中读取sizeof(charbuffer)大小的内容到内存。
如果文件不是特别大，内存也足够的话，可以一次性读取磁盘文件数据到内存，再对
内存中的数据进行操作。
一次性读操作：
//一次性读取磁盘文件到内存中的inBuffstr
inBufferStr.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
do{	
for (int i = 0; i < inBufferStr.size(); i++) {
		ch = inBufferStr[i];
		 ……
	}	
	}while(getline(inFile, inBufferStr));
 
一次性写操作：
for i=1 to n
	bufferstr+=ch;
fout<<bufferstr; //一次性将字符串buffestr写到文件输出流fout中。

2、STL map：
map的内部实现是一种非常高效的平衡二叉树：红黑树。时间复杂度为O(logn)级别。当频繁的插入或删除节点时候，如果采用普通new方式分配节点则会导致内存碎片增加，而Map的STL采用allocator分配内存，以内存池的方式管理内存，可以大大减少内存碎片，从而提高程序的整体性能，所以，在频繁的删除和插入节点元素时候，速度比普通的数组查找要快很多。



 

