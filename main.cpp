//      main.cpp
//      
//      Copyright 2010 Unknown <mohammed.azmy@energy>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#include <iostream>
#include "gc.hpp"
#include <vector>

using namespace std;
using namespace gc;

class MyObject : public Object{
    protected:
        string name;
    public:
        MyObject(const char* name){
            this->name = name;
            cout << "Construct object: " << this->name << endl;
        };
        virtual ~MyObject(){
            cout << "Destruct object: " << this->name << endl;
        };
        virtual void printName(){
            cout << "I am MyObject of " <<  this->name << ": " << this << endl;
        }
};

class Child : public MyObject{
    private:
        $<MyObject> link;
        
    public:
        Child(const char* name) : MyObject(name){
            link = new MyObject("This is the parent link");
        };
        
        virtual void printName() {
            cout << "I am child, and my name is " << this->name << endl;
            cout << "    Parent: " ;
            link->printName();
        };
};


void funct($<MyObject> o){
    (*o).printName();
}


int main(int argc, char** argv)
{
    vector<$<MyObject> > v;
    #define C 10
    for (int i =0; i < C; i++){
        $<MyObject> r = new Child("Azmy");
        v.push_back(r);
    }
	
    for (int i = 0; i< C; i++)
        funct(v[i]);
    
	return 0;
}
