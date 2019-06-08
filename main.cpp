/**
 * @text_classifier
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdio.h>
#include <cmath>
#include <locale>
#include <unordered_map>
using namespace std;
//# initalize local variable
vector<string> stop_words;
vector<vector<string> > x_train;
vector<int> y_train;
vector<vector<string> > x_test;
vector<int> y_test;
int NUM_CLASSES = 14;
float lambda_mixture = 0.7;
unordered_map<string,float> word_mat[14];
float p_class[14];
float num_class[14];
int total = 0;
int num_of_all_words = 0;

void read_stop_words(string filename){
    string str;
    fstream myfile;
    myfile.open(filename);
    while(!myfile.eof()){
      getline(myfile, str,',');
      string newstr;
      for(int i = 0; i < str.length(); i++){
        if(str[i]!=' ' && str[i]!='\'' && str[i] != '\n'){
          newstr += str[i];
        }
      }
      stop_words.push_back(newstr);
    }
}

void load_dataset(string train_filename, string dev_filename){
  int count = 0;
  string str;
  fstream train_file;
  train_file.open(train_filename);
  fstream dev_file;
  dev_file.open(dev_filename);

  while(!train_file.eof()){
    vector<string> x_train_entry;
    string newstr;
    string number_string;
    string word;
    int double_flag = 0;

    getline(train_file, str,'\n');
    for(int i = 0; i < str.length(); i++){
      if(double_flag == 2){
        newstr+=std::tolower(str[i], std::locale());
      }else{
        if(i == 0 || i==1){
          if(isdigit(str[i])){
            number_string+=str[i];
          }
        }
        if(str[i]==','){
          double_flag += 1;
        }
      }
    }
    if(number_string.length()!=0){
      y_train.push_back(atoi(number_string.c_str()));
    }

    for(int i = 0; i < newstr.length(); i++){
      if(isalpha(newstr[i]) || isdigit(newstr[i]) || newstr[i] == '\''){
        word+=newstr[i];
      }else{
        if (word.length() != 0){
          int flag = 0;
          for(vector<string>::iterator iter = stop_words.begin(); iter != stop_words.end(); iter++){
            if(word == *iter){
              flag = 1;
              break;
            }
          }
          if(!flag){
            x_train_entry.push_back(word);
          }
          word = "";
        }
      }
    }
    x_train.push_back(x_train_entry);
  }

  while(!dev_file.eof()){
    vector<string> x_test_entry;
    string newstr;
    string number_string;
    string word;
    int double_flag = 0;

    getline(dev_file, str,'\n');
    for(int i = 0; i < str.length(); i++){
      if(double_flag == 2){
        newstr+=std::tolower(str[i], std::locale());
      }else{
        if(i == 0 || i==1){
          if(isdigit(str[i])){
            number_string+=str[i];
          }
        }
        if(str[i]==','){
          double_flag += 1;
        }
      }
    }
    if(number_string.length()!=0){
      y_test.push_back(atoi(number_string.c_str()));
    }

    for(int i = 0; i < newstr.length(); i++){
      if(isalpha(newstr[i]) || isdigit(newstr[i]) || newstr[i] == '\''){
        word+=newstr[i];
      }else{
        if (word.length() != 0){
          int flag = 0;
          for(vector<string>::iterator iter = stop_words.begin(); iter != stop_words.end(); iter++){
            if(word == *iter){
              flag = 1;
              break;
            }
          }
          if(!flag){
            x_test_entry.push_back(word);
          }
          word = "";
        }
      }
    }
    x_test.push_back(x_test_entry);
  }
}



void fit(vector<vector<string> > train_set,vector<int> train_label){
  for(int type = 1; type < 15; type++){
    for(int index = 0; index < train_label.size(); index++){
      if(train_label[index] == type){
        for(vector<string>::iterator iter = train_set[index].begin(); iter != train_set[index].end(); iter++){
          //word is *iter
          num_class[type-1] += 1;
          total += 1;
          p_class[type-1] += 1;

          int flag_present = 0;
          for (std::pair<std::string, float> element : word_mat[type-1]){
            if (element.first == (*iter) ){
              flag_present = 1;
              word_mat[type-1][*iter] += 1;
              num_of_all_words += 1;
              break;
            }
          }
          if(flag_present == 0){
            word_mat[type-1][*iter] = 1;
          }
        }
      }
    }
  }

  for(int i = 0; i < NUM_CLASSES; i++){
    p_class[i] = log(p_class[i]) - log(total);
  }
  cout<<total<<endl;
  cout<<num_of_all_words<<endl;
  for(int j = 0; j < NUM_CLASSES; j++){
    float sum = 0;
    for (std::pair<std::string, float> element : word_mat[j]){
        sum += element.second;
        //cout<<sum<<endl;
    }
    for (std::pair<std::string, float> element : word_mat[j]){
      word_mat[j][element.first] = log(element.second+1) - log(sum+num_of_all_words);
    }
  }

  for(int m = 0; m < 14; m++){
    cout << p_class[m];
    cout << " ";
  }
  // for(int j = 0; j < NUM_CLASSES; j++){
  //   for (std::pair<std::string, float> element : word_mat[j]){
  //     cout<<element.first<<" "<<element.second<<endl;
  //   }
  // }
}

float predict(vector<vector<string> > dev_set,vector<int> dev_label){
  cout<<"flag1"<<endl;
  float accuracy = 0.0;
  float alpha = 0.1;
  vector<int> result;
  for(int index = 0; index < dev_label.size(); index++){
    //cout<<"flag2"<<endl;
    vector<float> p_label;
    for(int i = 0; i< NUM_CLASSES; i++){
      //cout<<"flag3"<<endl;
      p_label.push_back(p_class[i]);
      for(vector<string>::iterator iter = dev_set[index].begin(); iter != dev_set[index].end(); iter++){
        int flag_present = 0;
        for (std::pair<std::string, float> element : word_mat[i]){
          if (element.first == (*iter) ){
            flag_present = 1;
            p_label[i] += element.second;
            break;
          }
        }
        if(flag_present == 0){
          p_label[i] -= log(num_class[i]+num_of_all_words);
        }
      }
    }
    result.push_back(std::distance(p_label.begin(), max_element(p_label.begin(), p_label.end())));
    // for(int m = 0; m < p_label.size(); m++){
    //   cout << p_label[m];
    //   cout << " ";
    // }
    // cout<<""<<endl;
  }
  float count_correct = 0;
  cout<<dev_label.size()<<endl;
  cout<<result.size()<<endl;
  for(int i =0; i < dev_label.size(); i++){
    if(result[i]+1 == dev_label[i])
      count_correct+=1;
  }

  return count_correct/dev_label.size();
}

int main(){
  read_stop_words("stop_words.csv");
  load_dataset("train_text.csv","dev_text.csv");
  for(vector<vector<string> >::iterator iter = x_test.begin(); iter != x_test.end(); iter++){
    for(vector<string>::iterator iternew = (*iter).begin(); iternew != (*iter).end(); iternew++){
      cout<<*iternew+" ";
    }
    cout<<"\n"<<endl;
  }
  for(vector<int>::iterator iter = y_test.begin(); iter != y_test.end(); iter++){
    cout<<*iter;
    cout<<" ";
  }
  fit(x_train, y_train);
  cout<<"The accuracy is: "<<predict(x_test, y_test)<<endl;


  return 0;
}
