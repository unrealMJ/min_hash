import os
import matplotlib.pyplot as plt
import numpy as np


def draw(filename, b, r):

    f = open(filename)
    lines = f.readlines()
    similarity = []
    probability = []
    for line in lines:
        if line[0] == 'p':
            continue
        line = line.split("%")
        prob = line[0].split("=")
        prob[1] = prob[1].strip()
        simi = line[1].split("=")
        simi[1] = simi[1].strip()
        similarity.append(float(simi[1])/100)
        probability.append(float(prob[1])/100)


    x = np.linspace(0, 10, 1000)
    prediction = 1 - (1 - x**r)**b


    plt.xlim((0, 1))
    plt.ylim((0, 1))
    plt.xlabel("Jaccard Similarity")
    plt.ylabel("Probability")
    plt.xticks(np.linspace(0, 1, 10))
    plt.yticks(np.linspace(0, 1, 10))
    l1 = plt.plot(x, prediction, color='red', label='theoretical')
    l2 = plt.scatter(similarity, probability, s=10, color='blue', label='experimental')
    plt.legend(loc='upper left')
    plt.savefig('./figure/' + str(b) + '_' + str(r) + '.jpg')
    plt.cla()

if __name__ == '__main__':
    path = "./lsh_result"
    files = os.listdir(path)
    for file in files:
        b_r = file.split("_")
        b = int(b_r[0])
        r = int(b_r[1].split(".")[0])
        draw(path+'/'+file, b, r)





