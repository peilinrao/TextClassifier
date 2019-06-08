# TextClassifier.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Dhruv Agarwal (dhruva2@illinois.edu) on 02/21/2019
import math
"""
You should only modify code within this file -- the unrevised staff files will be used for all other
files and classes when code is run, so be careful to not modify anything else.
"""
class TextClassifier(object):
    def __init__(self):
        """Implementation of Naive Bayes for multiclass classification

        :param lambda_mixture - (Extra Credit) This param controls the proportion of contribution of Bigram
        and Unigram model in the mixture model. Hard Code the value you find to be most suitable for your model
        """
        self.lambda_mixture = 1
        # word_mat has words for col and label in row
        # It shows the log P(Word|Type=Ci)
        self.word_mat = []
        self.feature_mat = []
        self.p_class = []
        self.num_of_all_words = 0
        self.num_class = []
        self.ec_mat = []
        self.num_class_bi = []
        self.num_of_all_words_bi = 0
        self.total = 0


    def fit(self, train_set, train_label):
        """
        :param train_set - List of list of words corresponding with each text
            example: suppose I had two emails 'i like pie' and 'i like cake' in my training set
            Then train_set := [['i','like','pie'], ['i','like','cake']]

        :param train_label - List of labels corresponding with train_set
            example: Suppose I had two texts, first one was class 0 and second one was class 1.
            Then train_label := [0,1]
        """

        # TODO: Write your code here
        # Unique words in the train_set

        # Initialize the word_mat and p_class
        length = 14
        self.feature_mat = [["" for col in range(20)] for row in range(length)]
        self.word_mat = [{"":0.0} for row in range(length)]
        self.ec_mat = [{("",""):0.0} for row in range(length)]
        self.p_class = [0.0 for i in range(length)]
        self.num_class = [0.0 for i in range(length)]
        self.num_class_bi = [0.0 for i in range(length)]


        for type in range(1, length+1):
            # Filter the dataset for the sentences of desired type
            for index in range(len(train_label)):
                if train_label[index] == type:
                    for word in train_set[index]:
                        self.num_class[type-1] += 1
                        self.total += 1
                        self.p_class[type-1] += 1
                        if word not in self.word_mat[type-1]:
                            self.word_mat[type-1][word] = 1
                        else:
                            self.word_mat[type-1][word] += 1
                            self.num_of_all_words += 1

                        for word_other in train_set[index]:
                            self.num_class_bi[type-1] += 1
                            if (word, word_other) not in self.ec_mat[type-1]:
                                self.ec_mat[type-1][(word,word_other)] = 1
                            else:
                                self.ec_mat[type-1][(word,word_other)] += 1
                                self.num_of_all_words_bi += 1
        # Deal with the p_class mat
        for i in range(len(self.p_class)):
            self.p_class[i] = math.log(self.p_class[i]) - math.log(self.total)

        # Save feature_mat
        print(self.total)
        print(self.num_of_all_words)
        print(self.p_class)
        A = self.word_mat.copy()
        for type in range(length):
            self.feature_mat[type] = sorted(A[type], key=A[type].get, reverse=True)[:20]

        # Palace smoothing for word_mat
        for i in range(len(self.word_mat)):
            sum = 0.0
            for j in (self.word_mat[i]):
                sum += self.word_mat[i][j]
            for j in (self.word_mat[i]):
                self.word_mat[i][j] = math.log(self.word_mat[i][j]+1) - math.log(sum+self.num_of_all_words)

        temp = self.ec_mat.copy()
        for i in range(len(self.word_mat)):
            sum_bi = 0.0
            for j in self.ec_mat[i]:
                sum_bi += self.ec_mat[i][j]
            for (m,n) in self.ec_mat[i]:
                if self.ec_mat[i][(n,m)] > 0:
                    temp[i][(m,n)] = math.log(self.ec_mat[i][(m,n)] + self.ec_mat[i][(n,m)] + 1) - math.log(sum_bi+self.num_of_all_words_bi)
                else:
                    temp[i][(m,n)] = math.log(self.ec_mat[i][(m,n)]+1) - math.log(sum_bi+self.num_of_all_words_bi)
        self.ec_mat = temp.copy()

        pass

    def predict(self, dev_set, dev_label,lambda_mix=0.7):
        """
        :param dev_set: List of list of words corresponding with each text in dev set that we are testing on
              It follows the same format as train_set
        :param dev_label : List of class labels corresponding to each text
        :param lambda_mix : Will be supplied the value you hard code for self.lambda_mixture if you attempt extra credit

        :return:
                accuracy(float): average accuracy value for dev dataset
                result (list) : predicted class for each text
        """

        accuracy = 0.0
        result = []
        alpha = 0.1
        for sentence in dev_set:
            p_label = [0.0 for i in range(len(self.word_mat))]
            b_label = [0.0 for i in range(len(self.word_mat))]
            for type_i in range(14):
                # Comment out the next line to use MAP, otherwise it will be ML
                p_label[type_i] = self.p_class[type_i]
                for word in sentence:
                    if word in self.word_mat[type_i]:
                        p_label[type_i] += self.word_mat[type_i][word]
                    else:
                        p_label[type_i] += - math.log(self.num_class[type_i]+self.num_of_all_words)
                b_label[type_i] = self.p_class[type_i]
                for word_i in range(len(sentence)):
                    if word_i == 0:
                        if sentence[word_i] in self.word_mat[type_i]:
                            b_label[type_i] += self.word_mat[type_i][sentence[word_i]]
                        else:
                            b_label[type_i] += -math.log(self.num_class[type_i]+self.num_of_all_words)
                    elif (sentence[word_i], sentence[word_i-1]) in self.ec_mat[type_i]:
                        b_label[type_i] += self.ec_mat[type_i][(sentence[word_i],sentence[word_i-1])]
                        if sentence[word_i-1] in self.word_mat[type_i]:
                            b_label[type_i] -= self.word_mat[type_i][sentence[word_i-1]]
                        else:
                            b_label[type_i] -= -math.log(self.num_class[type_i]+self.num_of_all_words)
                    else:
                        b_label[type_i] += -math.log(self.num_class_bi[type_i]+self.num_of_all_words_bi*alpha)

                        if sentence[word_i-1] in self.word_mat[type_i]:
                            b_label[type_i] -= self.word_mat[type_i][sentence[word_i-1]]
                        else:
                            b_label[type_i] -= -math.log(self.num_class[type_i]+self.num_of_all_words)
            print(p_label)
            t_label = [0.0 for i in range(len(self.word_mat))]
            for type_i in range(len(self.word_mat)):
                t_label[type_i] = p_label[type_i] * self.lambda_mixture + b_label[type_i] * (1-self.lambda_mixture)

            result.append(1+t_label.index(max(t_label)))
        # print("result:")
        # print(result)
        # print()
        # print("dev_label")
        # print(dev_label)
        # print()

        confusion_mat = [[0.0 for i in range(14)] for j in range(14)]
        # class is from 1-14, I shift it to 0-13
        for i in range(len(dev_label)):
            confusion_mat[dev_label[i]-1][result[i]-1] += 1
        for i in range(len(confusion_mat)):
            sum = 0.0
            for j in range(len(confusion_mat[i])):
                sum += confusion_mat[i][j]
            for j in range(len(confusion_mat[i])):
                confusion_mat[i][j]/=sum

        # print("Confusion Matrix:")
        # print(confusion_mat)
        # print()
        #
        # print("Feature words:")
        # print(self.feature_mat)
        # print()

        count_correct = 0.0
        for i in range(len(result)):
            if result[i] == dev_label[i]:
                count_correct+=1

        accuracy = count_correct/len(result)
        return accuracy,result
