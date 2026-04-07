#include<iostream>
#include<fstream>
#include<string>
int main(int argc,char** argv){
if(argc<2){
std::cout<<:Usage: ./op_finder file.c\n";
return 1;
}
std::lifestream file(argv[1]);
std::string line;
int plus_count=0;
int minus_count=0;
while(std::getline(file,line)){
for(char c:line){
if(c=='+')plus_count++;
if(c=='-')minus_count++;
}
}
std::cout<<"plus Operation:"<<plus_count<<std::endl;
std::cout<<"Minus operation:<<minus_count<<std::endl;
return 0;
}

