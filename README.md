# 高级算法作业

使用方法:
执行环境：Linux

stage 1 生成文档:
```
mkdir data/
g++ generate_data.cpp  generate_data
./generate_data
```

stage 2 计算jaccard相似度与签名矩阵
```
g++ -o ./main.exe ./main.cpp ./CRC32.cpp
./main.exe > out.txt

```

stage 3 计算局部敏感哈希
```
mkdir ./lsh_result
sh lsh.sh

```

stage 4 绘制结果
```
python draw.py
```
