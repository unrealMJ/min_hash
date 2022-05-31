#include <iostream>
#include <fstream>
#include "CRC32.h"
#include <vector>
#include <set>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <array>
#include <algorithm>
#include <sstream>

#define buffer_size 8
#define doc_size 500
#define all_doc_num 1000
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

// 返回文件名中出现的第一个整数
int get_doc_num(string file_path)
{
	int num = 0;
	bool added_num = false;
	for(int i=0; i<file_path.length(); i++)
	{
		while(isdigit(file_path[i]))
		{
			num = num*10 + file_path[i++]-'0';
			added_num = true;
		}
		if(added_num)	return num;
	}
	return num;
}

//读取指定目录下的文档，逐个文档生成 shingle 集合并映射为整数集合
vector<set<unsigned int>> shingling(vector<string> file_names)
{
	vector<set<unsigned int>> shingle_sets(all_doc_num);
	for (int i = 0; i < file_names.size();++i)
	{
		ifstream istream(file_names[i]);
		char buffer[buffer_size];
		set<unsigned int> shingle;
		
		// cout << "Reading file: " << file_names[i] << endl;
		if (!istream.is_open())
		{ cout << "Could not open file!" << endl; exit(1);}
		
		for(int j=0; j < (doc_size - buffer_size + 1); j++)
		{
			istream.read(buffer, sizeof(char)*buffer_size);
			istream.seekg(sizeof(char)*(1-buffer_size), ios::cur);
			unsigned int hash_num = CRC32(buffer, sizeof(buffer));
			shingle.insert(hash_num);

			// cout << buffer << ' ';
		}
		shingle_sets[get_doc_num(file_names[i])-1] = shingle;
		istream.close();
		cout << file_names[i] << " shingle set size: " << shingle.size() << endl;
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
		int seed_base = i*9973;    //21 4748 3647
		unsigned int signature[signature_dim][doc_num];
		for(int j=0;j<signature_dim;j++)
		{
			fill_n(signature[j], doc_num, MAX_INT);   //初始化为最大unsigned int
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
					unsigned long long a_temp = a;
					unsigned long long b_temp = b;
					unsigned long long x = (*it);
					unsigned long long  h_num_temp = ((a_temp*x + b_temp) % p ) % (MAX_INT+1); //根据参数hi对应的a,b计算转换到的新行号hi(num)	
					unsigned int h_num = h_num_temp;
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
		sprintf(sig_file, "signature_mat/%d", i);
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

float cal_jaccard(set<unsigned int> shingle1, set<unsigned int> shingle2)
{
	int num_all=0, num11=0;
	set<unsigned int> temp;
	set_intersection(shingle1.begin(), shingle1.end(), shingle2.begin(), shingle2.end(), 
					inserter(temp, temp.begin()));
	num11 = temp.size();
	temp.clear();
	set_union(shingle1.begin(), shingle1.end(), shingle2.begin(), shingle2.end(), 
					inserter(temp, temp.begin()));
	num_all = temp.size();
	return num11*1.0/num_all;
}

void save_sim_matrix(vector<set<unsigned int>> shingles)
{
	ofstream csvFile("jaccard_sim.csv", ios::out|ios::trunc);
	float sim;
	if (!csvFile.is_open())
		{ cout << "Could not open csv file!" << endl; exit(1);}

	for(int i=0; i<shingles.size(); i++)
	{
		for(int j=0; j<shingles.size(); j++)
		{
			sim = cal_jaccard(shingles[i], shingles[j]);
			csvFile << sim << ",";
			cout << "(" << i << "," << j << ") similary: " << sim << endl;
		}
		csvFile << endl;
	}
}

vector<vector<float>> read_matrix()
{
	ifstream csv_file("jaccard_sim.csv");
	vector<vector<float>> sim_matrix;
	string line;
	if (!csv_file.is_open())
		{ cout << "Could not open csv file!" << endl; exit(1);}
	while(getline(csv_file, line))
	{
		istringstream sin(line);
		vector<float> data_line;
		string field;
		while(getline(sin, field, ','))
		{
			data_line.push_back((float)atof(field.c_str()));
		}
		sim_matrix.push_back(data_line);
	}
	return sim_matrix;
}


int main(int argc, char** argv)
{
	vector<string> file_names;
	vector<set<unsigned int>> shingles; //由 文档shingle的整数集合 组成的列表
	
	// 获取指定路径下的1000个文章
	getFiles("./data", file_names);
	cout << "shingling ..." << endl;
	shingles = shingling(file_names);

	// 计算jaccard similarity matrix
	// cout << "calculating jaccard similarity matrix ..." << endl;
	// save_sim_matrix(shingles);
	// 采用min hash生成签名矩阵
	cout << "\n min hash" << endl;
	gen_signatures(shingles);

}
