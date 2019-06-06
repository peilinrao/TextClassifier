#include <iostream>
#include <string>

using namespace std;

class shape
{
  private:
    string type;
  public:
    shape(string input){type = input;}
    string getType(){return type;}
    virtual double getC()=0;
};

class circle : public shape
{
  private:
    int r_;
    double helper(){return 3.14*2;}
  public:
    circle(int two) : shape("circle"){r_ = two;}
    double getC() {return helper()*(double)r_;}
};


int main()
{
  circle b(15);
  cout<<b.getType()<<endl;
  cout<<b.getC()<<endl;;
  return 0;
}
