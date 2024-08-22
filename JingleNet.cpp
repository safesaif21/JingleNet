// a3.cpp

/////////////////////////////////////////////////////////////////////////
//
// Student Info
// ------------
//
// Name : Saif Ali
// Email: saa225@sfu.ca
//
//
// Statement of Originality
// ------------------------
//
// All the code and comments below are my own original work. For any non-
// original work, I have provided citations in the comments with enough
// detail so that someone can see the exact source and extent of the
// borrowed work.
//
// In addition, I have not shared this work with anyone else, and I have
// not seen solutions from other students, tutors, websites, books,
// etc.
//
/////////////////////////////////////////////////////////////////////////

//
// Do not #include any other files!
//
#include "Announcement.h"
#include "JingleNet_announcer.h"
#include "Queue_base.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

class Queue : public Queue_base<Announcement>{

    struct Node
    {
        Announcement data;
        Node *next;
    };

    Node *head = nullptr;
    Node *tail = nullptr;
    int numOfElements = 0;

public:

    int size() const override{

        return numOfElements;
    } 
    void enqueue(const Announcement &item) override{

        if(head == nullptr){

            head = new Node{item, nullptr};
            tail = head;
        }else{

            tail->next = new Node{item, nullptr};
            tail = tail->next;
        }
        numOfElements++;
    }   
    void dequeue() override{

        if(numOfElements == 0){

            throw runtime_error("dequeue: queue is empty");
        }
        if(head == tail){
            
            tail = nullptr;
        }
        Node *temp = head;
        head = head->next;
        delete temp;
        numOfElements--;
    }
    const Announcement& front() const override{

        if(numOfElements == 0){

            throw runtime_error("front: queue is empty");
        }
        return head->data;
    }
    ~Queue(){

        while(head != nullptr){
            dequeue();
        }
    }
};// class Queue

class JingleNet {

    Queue Santas;
    Queue Reindeer;
    Queue Elf2;
    Queue Elf1;
    Queue Snowman;
    Queue *queueArray[5] = {&Santas, &Reindeer, &Elf2, &Elf1, &Snowman}; //array of pointers to queues

public:

    void send(Announcement &a){

        string rank = to_string(a.get_rank());
        if(rank == "santa"){

            Santas.enqueue(a);
        }else if(rank == "reindeer"){

            Reindeer.enqueue(a);
        }else if(rank == "elf2"){

            Elf2.enqueue(a);
        }else if(rank == "elf1"){

            Elf1.enqueue(a);
        }else if(rank == "snowman"){

            Snowman.enqueue(a);
        }
    }
    void remove_all(string username){

        for(int i = 0; i < 5; i++){

            Queue *temp = queueArray[i];
            int size = temp->size();
            while(size != 0){

                if(temp->front().get_sender_name() == username){

                    temp->dequeue();
                }else{

                    temp->enqueue(temp->front());
                    temp->dequeue();
                }
                size--;
            }
        }
    }
    void promote(const string& username) {

        for(int i = 1; i < 5; i++) {

            int size = queueArray[i]->size();

            for(int j = 0; j < size; j++) {

                const Announcement& frontAnnouncement = queueArray[i]->front();
                const string oldRank = to_string(frontAnnouncement.get_rank());
                Rank newRank = Rank::SANTA;

                if(frontAnnouncement.get_sender_name() == username) {

                    if(oldRank == "reindeer"){

                        newRank = Rank::SANTA;
                    }else if(oldRank == "elf2"){

                        newRank = Rank::REINDEER;
                    }else if(oldRank == "elf1"){

                        newRank = Rank::ELF2;
                    }else if(oldRank == "snowman"){

                        newRank = Rank::ELF1;
                    }
                    queueArray[i-1]->enqueue(Announcement(frontAnnouncement.get_sender_name(), newRank, frontAnnouncement.get_text()));
                    queueArray[i]->dequeue();
                }
                else{
                    queueArray[i]->enqueue(frontAnnouncement);
                    queueArray[i]->dequeue();
                }
            }
        }
    }
    void announce(int n){

        for(int i = 0; i < 5; i++){

            int size = queueArray[i]->size();

            for(int j = 0; j < size; j++){

                if(n != 0){

                    jnet.announce(queueArray[i]->front());
                    queueArray[i]->dequeue();
                    n--;
                }  
            }  
        }
    }
};// class JingleNet

int main(int argc, char *argv[]){

    JingleNet jingle;

    if(argc != 2){//checks if there is only one argument

        return 1; 
    }
    string file = argv[1];
    ifstream infile(file);
    string line;
    int num_lines = 0;
    while(getline(infile, line)){//reads the file line by line

        num_lines++;
        if(line.substr(0, 4) == "SEND"){

            Announcement temp(line.substr(5));
            jingle.send(temp);
        }else if(line.substr(0, 8) == "ANNOUNCE"){
                
                int n = stoi(line.substr(9));//stoi() converts string to int
                jingle.announce(n);
        }else if(line.substr(0, 10) == "REMOVE_ALL"){

            jingle.remove_all(line.substr(11));
        }else if(line.substr(0, 21) == "PROMOTE_ANNOUNCEMENTS"){

            jingle.promote(line.substr(22));
        }
    }
}// main
