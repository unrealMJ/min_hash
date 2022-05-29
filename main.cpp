#include <iostream>
#include <fstream>
#include "CRC32.h"
#include <vector>
#include <set>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <array>


#define buffer_size 8
#define doc_size 500

#define signature_dim 30
#define MAX_INT 4294967295  // 

// 获得指定目录下的所有文件路径
void getFiles(string path, vector<string> &files)
{
    DIR *dir;
    struct dirent *ptr;
    if ((dir = opendir(path.c_str())) == NULL)
	{
        perror("Open dir error...");
        return;
    }
 
    while ((ptr = readdir(dir)) != NULL)
	{
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
            continue;
        else if (ptr->d_type == 8)    ///file
        {
            string strFile;
            strFile = path;
            strFile += "/";
            strFile += ptr->d_name;
            files.push_back(strFile);
        } else {
            continue;
        }
    }
    closedir(dir);
}

int count1(char n)
{ 	// 使用分治法统计1的个数
	// https://cloud.tencent.com/developer/article/1176348
	n = (n&0x55555555) + ((n>>1)&0x55555555);  
    n = (n&0x33333333) + ((n>>2)&0x33333333);  
    n = (n&0x0f0f0f0f) + ((n>>4)&0x0f0f0f0f);  
    n = (n&0x00ff00ff) + ((n>>8)&0x00ff00ff);  
    n = (n&0x0000ffff) + ((n>>16)&0x0000ffff);
	return n;
}

//读取指定目录下的文档，逐个文档生成 shingle 集合并映射为整数集合
vector<set<unsigned int>> shingling(vector<string> file_names)
{
	vector<set<unsigned int>> shingle_sets;
	for (int i = 0; i < file_names.size();++i)
	{
		ifstream istream(file_names[i]);
		char buffer[buffer_size];
		set<unsigned int> shingle;
		
		cout << "Reading file: " << file_names[i] << endl;
		if (!istream.is_open())
		{ cout << "Could not open file!" << endl; exit(1);}
		
		for(int j=0; j < doc_size / (buffer_size); j++)
		{
			istream.read(buffer, sizeof(char)*buffer_size);
			istream.seekg(sizeof(char)*(1-buffer_size), ios::cur);
			unsigned int hash_num = CRC32(buffer, sizeof(buffer));
			shingle.insert(hash_num);

			cout << buffer << ' ';
		}
		shingle_sets.push_back(shingle);
		istream.close();
		cout << endl << endl;
	}
	return shingle_sets;
}

//生成1000个 维度为30*1000的签名矩阵
void gen_signatures(vector<set<unsigned int>> shingles)
{
	int sig_mat_num = 1000;
	int doc_num = shingles.size();
	unsigned long p = 4294967311;
	cout << "parm p is set to:" <<  p << endl;

	for(int i=0; i< sig_mat_num ;i++)
	{	
		cout << "\n generating signature mat:" << i << endl;
		int seed_base = i*9973; 
		unsigned int signature[signature_dim][doc_num];
		for(int i=0;i<signature_dim;i++)
		{
			fill_n(signature[i], doc_num, MAX_INT);   //初始化为最大unsigned int
		}

		for(int hi=0;hi<signature_dim;hi++)  //遍历所有哈希函数
		{
			srand(seed_base+1+hi);
			//确定哈希函数参数a,b
			unsigned int a = rand();
			unsigned int b = rand();
			cout << "hash function:" << hi << " a: " << a << " b: " << b << endl;
			for(int col=0; col<doc_num; col++)   //遍历所有文档（相当于遍历特征矩阵第num行）
			{
				set<unsigned int>::iterator it;
				for(it=shingles[col].begin(); it!=shingles[col].end(); it++)  //遍历set中每个数值
				{
					unsigned int h_num = ((a*(*it) + b) % p ) % (MAX_INT+1); //根据参数hi对应的a,b计算转换到的新行号hi(num)	
					if(h_num<signature[hi][col])
					{
						signature[hi][col] = h_num;
					}
				}
			}
		}
		if(i<2)
		{
			cout << "\n signature result: " << endl;
			for(int col=0; col<doc_num; col++) 
			{
				cout << "\n doc col:" <<col << endl;
				for(int hi=0;hi<signature_dim;hi++)
				{
					cout << " " << signature[hi][col] << " ";
				}
				cout << "\n" ;
			}
		}

		// (option )save signature to file
		char sig_file[20];
		sprintf(sig_file, "signatue_mat/%d", i);
		FILE* fp = fopen(sig_file, "w");
		if(!fp){
			cout << "fail to open file " << endl;
		}
		int item_num=signature_dim*doc_num;
		unsigned int* item_p = &signature[0][0];
		fwrite(item_p, sizeof(unsigned int), item_num, fp);
		fclose(fp);
	}

}


int main(int argc, char** argv)
{
	vector<string> file_names;
	vector<set<unsigned int>> shingles; //由 文档shingle的整数集合 组成的列表

	// 获取指定路径下的1000个文章
	getFiles("./data", file_names);
	shingles = shingling(file_names);
	cout << shingles.size() << ' ' << shingles[0].size();

	//采用min hash生成签名矩阵
	cout << "\n min hash" << endl;
	gen_signatures(shingles);


}
