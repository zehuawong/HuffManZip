#include <iostream>  
#include <fstream>  
#include <map>  
#include <string>    
#include<queue>
#include<vector>
#include<time.h>
#include<iomanip>
#include<cstdlib>
using namespace std;

#define TEST_NUM 20  //测试次数

//结构不能包含显式的无参数构造函数。结构成员讲自动初始化为它们的默认值。
//定义一个 struct 变量的同时不能对其（成员）进行初始化 ，即成员没有默认值。它只能在定义对象时，对对象的成员变量进行初始化。
struct  HuffNode
{
	char value;//字母
	int freq; //字母出现频率
	string huffcode;//编码串
	HuffNode* left;
	HuffNode* right;
};

struct cmpByFreq {

	cmpByFreq() {}
	bool operator()(const HuffNode *node1,const HuffNode *node2)const {
		return node1->freq > node2->freq;//根据字符频率最小值优先
	}
};
struct cmpByFreq2 {

	cmpByFreq2(){}
	bool operator()(const HuffNode *node1, const HuffNode *node2)const {
		return node1->freq < node2->freq;//根据字符频率最大值优先
	}
};
void printLeaf(HuffNode * node)
{
	if (node) {	
		printLeaf(node->left);
		printLeaf(node->right);		
		if (node->left == NULL || node->right == NULL)
			cout << node->value << ":" << node->freq <<" "<<node->huffcode<< endl;
	}
}

void printCharToBin(char tempchar)
{
	int T = 8;
	while (T--) //输出字符对应的二进制串
	{
		if ((tempchar&(1 << T)))
			cout << 1;
		else cout << 0;

	}
	cout << endl;
}

void makeHuffmanCode(HuffNode *node,string codestr)
{
	node->huffcode += codestr;
	if (node->left != NULL)
		makeHuffmanCode(node->left, node->huffcode + "0");
	if (node->right != NULL)
		makeHuffmanCode(node->right, node->huffcode + "1");

}

void readFile(string filepath)
{
	cout << "压缩后的文件：" << endl;
		
	ifstream inFile(filepath, ios::binary);
	long filelength = 0;
	inFile.read((char*)&filelength, sizeof(long));
	cout << "文件总长度为：" << filelength << "bytes" << endl;
	short chNum;
	inFile.read((char*)&chNum, sizeof(short));
	cout << "文件字符种类数量：" <<chNum<< endl;
	char ch; int freq;
	for(int i=0;i<chNum;i++)
	{
		inFile.read(&ch, sizeof(char));
		inFile.read((char*)&freq, sizeof(int));
		cout << ch << ":" << freq << endl;
	}
	while (!inFile.eof()) {
		inFile.read(&ch, sizeof(char));
		printCharToBin(ch);
	}
	inFile.close();
}

//释放内存
void destroy(HuffNode *node)
{
	if (node) {
		destroy(node->left);
		destroy(node->right);
		delete node;
	}
}

//输出Huffman树节点频率-编码串到excel文件
void printLeafNode(map<char,HuffNode*> huffmanWordMap,string filename)
{
	ofstream fout(filename + ".xls",ios::binary);

	priority_queue<HuffNode*, vector<HuffNode*>, cmpByFreq2> min_queue;
	map<char, HuffNode*>::iterator iter;
	for (iter = huffmanWordMap.begin(); iter != huffmanWordMap.end(); iter++)
	{
		min_queue.push(iter->second);
	}

	HuffNode *node;
	while (!min_queue.empty())
	{
		node = min_queue.top();
		printf("%x", node->value);
		//cout << node->value << ":" << node->freq << " " << node->huffcode << endl;
		cout<< ":" << node->freq << " " << node->huffcode << endl;
		fout << node->value<< "\t";
		fout << node->freq << "\t";
		fout << node->huffcode << "\r\n";
		min_queue.pop();
	}

	fout.close();
}

//压缩文件
void huffmanZip(string filepath,string zipfilename)
{
	ifstream inFile;
	inFile.open(filepath, ios::binary);
	if (!inFile) {
		cout << "open file error!";
		exit(1);
	}
	cout << "开始压缩文件……" << endl;
	long fbeg = inFile.tellg();
	inFile.seekg(0, ios::end);
	long fend = inFile.tellg();
	long filelength = fend - fbeg; //获取源文件总长度
	inFile.seekg(0, ios::beg);
	char ch; //字符
	short chNum = 0;//字符种类数
	map<char, int> wordMap; //字符-频率对
	short size_char = sizeof(ch);
	//for(long fp=fbeg;fp<fend;fp++)
	//while (!inFile.eof())  //这种方法会多读取一个字符
	string inBufferStr; //改进方法，一次性读取磁盘文件到内存中的buffstr
	inBufferStr.assign(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>());
	do
	{	
		//int strsize = bufferstr.size();
		for (int i = 0; i < inBufferStr.size(); i++) {
			ch = inBufferStr[i];
			if (wordMap.find(ch) == wordMap.end())//如果map中没有字符ch
				wordMap[ch] = 1;
			else wordMap[ch]++;
		}	
	}while(getline(inFile, inBufferStr));

	inFile.close(); //关闭文件流
	
	chNum = wordMap.size();//获取字符种类数

	//cout << "chNum=" << chNum << endl;
	map<char, HuffNode*> huffmanWordMap; //字符-Huffman编码Map
	ofstream outFile(zipfilename+".huffzip", ios::binary);//压缩文件输出流
	string outBufferStr;
	outFile.write((char*)&filelength, sizeof(long)); //向压缩文件写入源文件总长度
	outFile.write((char*)&chNum, sizeof(short));//向压缩文件写入字符种类数量
											
	//最小优先队列 priority_queue
	priority_queue<HuffNode*, vector<HuffNode*>, cmpByFreq> min_queue;
	map<char, int>::iterator iter; int chi = 0;
	for (iter = wordMap.begin(); iter != wordMap.end(); iter++) {
		HuffNode *huffNode = new HuffNode();
		huffNode->left = NULL; huffNode->right = NULL;
		huffNode->value = iter->first;
		huffNode->freq = iter->second;
		min_queue.push(huffNode); //入队列
		huffmanWordMap[huffNode->value] = huffNode; //入map
		//cout << huffmanWordMap[iter->first]->value <<":"<<huffNode->freq<< endl;
		//向压缩文件写入每个字符以及字符的数量
		outFile.write(&iter->first, sizeof(char));//此处写入时候必须是const char*类型
		outFile.write((char*)&iter->second, sizeof(int));
	}


	if (min_queue.empty())
		return;

	//构造哈夫曼树
	HuffNode *node1, *node2,*root=NULL;
	for (int i = 0; i < chNum - 1; i++) {	//n-1次
		HuffNode *newNode = new HuffNode();
		if (min_queue.size() == 2)	//为了避免每次循环都要判断，可以先循环处理n-2次，最后一次再单独处理
			root = newNode;
		node1 = min_queue.top(); min_queue.pop();
		node2 = min_queue.top(); min_queue.pop();
		newNode->left = node1;
		newNode->right = node2;
		newNode->freq = node1->freq + node2->freq;//权重之和
		//cout << node1->freq << " " << node2->freq <<" "<<newNode->freq << endl;
		min_queue.push(newNode);
	}

	makeHuffmanCode(root, ""); //生成各个字符的Huffman编码串

	//printLeaf(root);
	//printLeafNode(huffmanWordMap,filepath);

	string codestr = "";

	//将源文件的每个字符的对应哈弗曼码写入压缩文件
	ifstream inf(filepath, ios::binary);
	//此处考虑增加缓存，避免每次都向源文件写入单个字符,减少IO次数，优化性能
	inBufferStr.assign(istreambuf_iterator<char>(inf), istreambuf_iterator<char>());
	outBufferStr = "";

	char tempchar = 0;
	int length = 8;
	do {
		for (int j = 0; j < inBufferStr.size(); j++)
		{
			ch = inBufferStr[j];
			codestr = huffmanWordMap[ch]->huffcode; //获取Huffman编码串
													//cout <<ch<<":"<< codestr << endl;
			for (int i = 0; i < codestr.size(); i++) {
				length--;
				if (codestr[i] == '0')
					tempchar = tempchar | (0 << length);
				else //(codestr[i] == '1')
					tempchar = tempchar | (1 << length);
				if (length == 0) {
					outBufferStr += tempchar;
					length = 8;
					tempchar = 0;
				}
			}
		}
	} while (getline(inf, inBufferStr));
	 
	if (length != 0) { //最后一个字节，防止写入压缩文件的时候未写满8位的倍数
		outBufferStr += tempchar;		
	}
	outFile << outBufferStr; //缓冲字符串(huffman编码串)写入到磁盘压缩文件

	inf.close();
	outFile.close();
	cout << "压缩完成！" << endl;
	//释放内存
}

//解压缩文件
void unzipHuffFile(string filename)
{	
	//cout << "开始解压缩文件……" << endl;
	ifstream fin;
	fin.open(filename, ios::binary); //压缩文件
	if (!fin) {
		cout << "open file error!";
		exit(1);
	}
	cout << "开始解压缩文件……" << endl;

	ofstream fout(filename + ".unzip", ios::binary); //解压出来的文件
	//缓冲字符串
	string bufferInStr;
	string bufferOutStr;
	
	long filelength = 0;
	fin.read((char*)&filelength, sizeof(long)); //获取源文件总长度
	//cout << "文件总长度为：" << filelength << "bytes" << endl;
	short chNum; //字符种类数	
	fin.read((char*)&chNum, sizeof(short)); //读取字符种类数
	//cout << "字符种类：" << chNum << endl;
	map<char, HuffNode*> huffmanWordMap; //字符-Huffman编码Map
	char ch; int freq;

	priority_queue<HuffNode*, vector<HuffNode*>, cmpByFreq> min_queue;//最小优先队列 priority_queue
	for(int i=0;i<chNum;i++)
	{
		//cout << "i=" << i<<"  ";
		fin.read(&ch, sizeof(char));
		fin.read((char*)&freq, sizeof(int));
		//cout << ch << ":" << freq<<endl;
		HuffNode *huffnode = new HuffNode();
		huffnode->left = NULL; huffnode->right = NULL;
		huffnode->value = ch;
		huffnode->freq = freq;
		//入队列
		min_queue.push(huffnode);
	}

	//构造哈夫曼树
	HuffNode *node1, *node2, *root = NULL;
	for (int i = 0; i < chNum - 1; i++) {	//n-1次
		HuffNode *newNode = new HuffNode();
		if (min_queue.size() == 2)	//为了避免每次循环都要判断，可以先循环处理n-2次，最后一次再单独处理
			root = newNode; //更新根节点指针
		node1 = min_queue.top(); min_queue.pop();
		node2 = min_queue.top(); min_queue.pop();
		newNode->left = node1;
		newNode->right = node2;
		newNode->freq = node1->freq + node2->freq;//权重之和
		//cout << node1->freq << " " << node2->freq <<" "<<newNode->freq << endl;
		min_queue.push(newNode);
	}

	if (root == NULL)
		return;

	makeHuffmanCode(root, ""); //生成各个字符的Huffman编码串
	//printLeaf(root);

	//cout << "开始解压:" << endl;
	bufferOutStr = "";
	bufferInStr.assign(istreambuf_iterator<char>(fin), istreambuf_iterator<char>());
	char tempchar = 0; long charLen = filelength;
	HuffNode *tempNode = root; //初始指向根节点
	do {
		//cout << bufferInStr;
		for (int i = 0; i < bufferInStr.size(); i++) {
			//printCharToBin(bufferInStr[i]);
			ch = bufferInStr[i];
			for (int i = sizeof(char) * 8 - 1; i >= 0; i--)
			{
				if (tempNode->left != NULL)
				{
					if ((ch >> i) & 1)
						tempNode = tempNode->right;
					else tempNode = tempNode->left;
				}
				if (tempNode->left == NULL) //找到叶节点则将字符写入文件
				{
					if (charLen == 0)
						break;
					charLen--;
					tempchar = tempNode->value;			
					bufferOutStr += tempchar;
					//cout << tempchar;
					tempNode = root; //将tempNode重新指向根节点
				}
			}
		}

	} while (getline(fin, bufferInStr));

	fout << bufferOutStr; //将缓冲字符串输出到解压文件

	fin.close();
	fout.close();
	cout << "解压完成！" << endl;
}
void printFileToBin(string filename)
{
	ifstream fin(filename, ios::binary);
	char ch=0;
	while (!fin.eof())
	{
		fin.read(&ch, sizeof(char));
		printCharToBin(ch);
	}
	fin.close();
}

void compareFiles(string srcfile, string zipfile)
{

	ifstream srcfin(srcfile);
	ifstream zipfin(zipfile);
	long fbeg1 = srcfin.tellg();
	srcfin.seekg(0, ios::end);
	long fend1 = srcfin.tellg();

	long fbeg2 = zipfin.tellg();
	zipfin.seekg(0, ios::end);
	long fend2 = zipfin.tellg();

	cout << srcfile << "大小：" << (fend1 - fbeg1)*1.0/ 1024 <<"kb"<< endl;
	cout << zipfile<< "大小：" << (fend2 - fbeg2)*1.0 / 1024 <<"kb"<<endl;
	cout << setiosflags(ios::fixed) << setprecision(2);
	cout << "压缩率：" << (fend2 - fbeg2) *1.0/ (fend1 - fbeg1)*100<<"%"<< endl;

	srcfin.close();
	zipfin.close();
}

int main(int argc,char *argv[])
{
	if (argc!=2)
	{
		cout << "命令格式错误！"<<endl;
		//cout << "压缩命令格式应该为：huffmanZip filename zipfilename" << endl;
		cout << "解压缩命令格式应该为：unzipHuffFile filename" << endl;
		exit(1);
	}
	clock_t startTime, endTime, sumTime = 0;
	startTime = clock();
	string filename = argv[1];
	//string zipfilename = argv[2];
	//huffmanZip(argv[1],argv[2]);	
	unzipHuffFile(argv[1]);
	endTime = clock();
	cout << "用时：" << (endTime - startTime)*1.0 / 1000.0 << "s" << endl;
/*
	clock_t startTime, endTime, sumTime = 0;
	//string filename = "files/oldfile.txt";
	//string filename = "files/Martin Luther King - I Have A Dream.txt";
	//string filename = "files/生命不能承受之轻（英文版）.txt";
	//string filename = "files/Pride and Prejudice.txt";
	//string filename = "files/老人与海英文版.txt";
	//string filename = "files/麦田里的守望者-英文版.txt";
	//string filename = "files/百年孤独-英文版.txt";
	//string filename = "files/明朝那些事儿.txt";
	//string filename = "files/万历十五年.txt";
	string filename = "files/cacm.all";

	for (int i = 0; i < TEST_NUM; i++)
	{
		startTime = clock();
		huffmanZip(filename,filename); //压缩文件
		//system("\"C:\\Program Files\\WinRAR\\Rar.exe\" a cacm.rar cacm.all");
		endTime = clock();
		sumTime += (endTime - startTime);
		cout << endTime - startTime << "ms" << " ";
	}

	cout << endl;
	cout << "压缩时间：" << 1.0*sumTime / TEST_NUM / 1000.0<<"s"<< endl;
	//unzipHuffFile(filename + ".huffzip"); //解压缩文件

	compareFiles(filename, filename + ".huffzip"); //比较源文件和压缩文件，输出压缩率
	//compareFiles("cacm.all", "cacm.rar");

	system("pause");
*/
	return 0;
}
