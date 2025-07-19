#include <assert.h>
#include <sstream>
#include "qna_tool.h"


class minheapnode{
public:
    int bookcode=0;        // Which book this paragraph belongs to
    int pagno=0;           // Page number within the book
    int parno=0;           // Paragraph number within the page
    long double frac=0;    // Relevance score (TF-IDF like calculation)
    
    // Constructor with parameters
    minheapnode(int b,int p,int pr,long double f){
        bookcode=b;
        pagno=p;
        parno=pr;
        frac=f;
    }
    minheapnode(){}        // Default constructor
};

class minheap{
public:
    // vector<minheapnode> arr;
    vector<minheapnode> heap;
    int k;
    minheap(int k){
        heap.resize(k);
        this->k=k;
    }
    minheap(){}

    //Some helper functions for in heap class

    void swap(minheapnode &a, minheapnode &b) {
        minheapnode temp = a;
        a = b;
        b = temp;
    }

    bool isEmpty() {
    return heap.empty();
}

    int getParentIndex(int i) {
    return (i - 1) / 2;
    }
  
    int getLeftChildIndex(int i) {
        return 2 * i + 1;
    }
    
    int getRightChildIndex(int i) {
        return 2 * i + 2;
    }
    

    // Maintains min-heap property after insertion by moving element up.
    void heapifyUp(int index) {
    if (index == 0) return; // base condition for termination of a recursive invocation of the fnc
        
        int parentIndex = getParentIndex(index);
        
        if (heap[parentIndex].frac > heap[index].frac) {
            swap(heap[parentIndex], heap[index]);
            heapifyUp(parentIndex);
        }
    }

   //Maintains min-heap property after deletion by moving element down.
  void heapifyDown(int index) {
        int leftChild = getLeftChildIndex(index);
        int rightChild = getRightChildIndex(index);
        
        if (leftChild >= heap.size()) return; // No children
        
        int minindex = index;
        
        if (heap[minindex].frac > heap[leftChild].frac) {
        minindex = leftChild;
        }
        
        if (rightChild < heap.size() && heap[minindex].frac > heap[rightChild].frac) {
        minindex = rightChild;
        }
        
        if (minindex != index) {
        swap(heap[minindex], heap[index]);
        heapifyDown(minindex);
        }
    }



    minheapnode minElem(){
    return heap[0];
    }

  minheapnode deleteMin() {
    minheapnode temp=heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    heapifyDown(0);
    return temp;
   }

  void insert(minheapnode val) {
        if (heap[0].frac<val.frac){
        heap[0]=val;
        heapifyDown(0);
    }
  }

// void printHeap (){
//     for (const auto &elem : heap) {
//       cout << elem.frac << " ";
//     }
//     cout << endl;
//   }

  void makeheapempty(){
    heap.clear();
  }

};


//Storage Hierarchy Classes
using namespace std;

// Node structure to represent a paragraph
class paranode{
    public:
    long double frac;
    paranode(){
        frac=0;
    }
};

// Node structure to represent a page
// Contains a vector of paragraphs
class pagenode{
    public:
    vector<paranode*> vec;
    pagenode(int paramax){
        vec.resize(paramax,nullptr);
    }
};

// Node structure to represent a book
// Contains a vector of pages
class booknode{
    public:
    vector<pagenode*> vec;
    booknode(int pagamax){
        vec.resize(pagamax,nullptr);
    }
};

// Node structure to represent a word
// Contains a linked list of occurrences
class hashnode{
    public:
    vector<booknode*> vec;
    hashnode(int bookmax){
        vec.resize(bookmax,nullptr);
    }
};


// Trie structure to store words and their occurrences

/*The trie here serves as a reverse index - for any word, you can quickly find:

How many times it appears in the corpus (corpuscount)
How many times it appears overall (csvcount)
Exact locations where it appears (book, page, paragraph)*/

/*
From dict.cpp (Trie Structure):
struct wordnode {
    int bookcode;     // Which book
    int pageno;       // Which page
    int parano;       // Which paragraph
    wordnode* next;   // Next occurrence
};

struct word {
    int corpuscount;  // Total occurrences in corpus
    int csvcount;     // Total occurrences in general English
    wordnode* head;   // Start of linked list (dummy node)
    wordnode* tail;   // End of linked list (dummy node)
};*/

class triehash{

    public:
        hashnode* root;
        minheap minhp;
        int bkmax;
        int pgmax;
        int prmax;

    // Constructor to initialize the triehash with book, page, paragraph limits and heap size
    triehash(int bookmax,int pagemax,int paramax,int k){
        root=new hashnode(bookmax);
        bkmax=bookmax;
        pgmax=pagemax;
        prmax=paramax;
        minhp=minheap(k);
    }

   //Let's take example of freedom name word class so it is the list of all occurrences of the word "freedom" in the corpus
    //This function inserts the wordlist into the triehash
    //It iterates through the linked list of occurrences and inserts each occurrence into the triehash
    //It updates the frequency counts (corpuscount, csvcount) for each occurrence
    //The triehash is used to store the frequency counts and occurrences of words in the corpus
    void insert(word* wordlist){

        if (wordlist==nullptr) return;
        if (wordlist->head==nullptr) return;

        /*head -> [Book1,Page5,Para2] -> [Book1,Page12,Para1] -> [Book2,Page8,Para3] -> ... -> tail
                        ^
                     temp starts here*/
        wordnode* temp=wordlist->head->next;

        //High corpus frequency + Low general frequency = High relevance
        long double frac=(static_cast<long double>(wordlist->corpuscount+1))/(wordlist->csvcount+1);
        while (temp!=wordlist->tail){

            int bkcode=temp->bookcode;
            int pgno=temp->pageno;
            int parno=temp->parano;

            // Check if book  exists, create if not
                if (!root->vec[bkcode]) 
                    root->vec[bkcode]=new booknode(pgmax);

                // Check if page  exists in book , create if not  
                if (!root->vec[bkcode]->vec[pgno]) 
                    root->vec[bkcode]->vec[pgno]=new pagenode(prmax);

                // Check if paragraph  exists in page , create if not
                if (!root->vec[bkcode]->vec[pgno]->vec[parno]) 
                    root->vec[bkcode]->vec[pgno]->vec[parno]=new paranode();

            // Update the fraction for this paragraph
            root->vec[bkcode]->vec[pgno]->vec[parno]->frac+=frac;
            temp=temp->next;
        }
    }

    void heapinsert(){
        for (int i=0;i<bkmax;i++){
            if (root->vec[i]){
                for (int j=0;j<pgmax;j++){
                    if (root->vec[i]->vec[j]){
                        for (int k=0;k<prmax;k++){
                            if (root->vec[i]->vec[j]->vec[k]){
                                minhp.insert(minheapnode(i,j,k,root->vec[i]->vec[j]->vec[k]->frac));
                            }
                        }
                    }
                }
            }
        }
    }


  Node* givelargestk(int k){
    Node* temp=nullptr;
    int count = 0;
    
    while(count < k && minhp.heap.size() > 0){
        minheapnode m = minhp.deleteMin();
        
        // Skip invalid book codes (0 doesn't exist)
        if(m.bookcode == 0) continue;
        
        if (temp==nullptr){
            temp=new Node(m.bookcode,m.pagno,m.parno,0,0);
        }
        else {
            Node* n=new Node(m.bookcode,m.pagno,m.parno,0,0);
            temp->left=n;
            n->right=temp;
            temp=n;
        }
        count++;
    }
    return temp;
}


};

string tolower(string s){
    string d="";
    for (char i:s){
        int k=i-'A';
        if (k>=0 && k<=25) i=i+32;
        d+=i;
    }
    return d;
}

vector<string> getmyword(string sentence){
    int n=sentence.length();
    vector<string> v;
    string s="";
    string sp=" .,-:!\"\'()?[];@";
    for (char i:sentence){
        bool flag=false;
        for (char c:sp){
            if (c==i){
                flag=true;
                if (s!="") {
                    s=tolower(s);
                    v.push_back(s);
                }
                s="";
                break;
            }
        }
        if (!flag)  s+=i;
    }
    if (s!=""){
        s=tolower(s);
        v.push_back(s);
    }
    return v;

}



QNA_tool::QNA_tool(){
    // Implement your function here
    Dict dict;
    bmax=0;
    pgmaxx=0;
    prmaxx=0;
    
}

QNA_tool::~QNA_tool(){
}

void QNA_tool::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence){
    dict.insert_sentence(book_code, page, paragraph, sentence_no,sentence);
    pgmaxx=max(page,pgmaxx);
    bmax=max(bmax,book_code);
    prmaxx=max(prmaxx,paragraph);
    // searchengine.insert_sentence(book_code,page,paragraph,sentence_no,sentence);
    return;
}

Node* QNA_tool::get_top_k_para(string question, int k) {
    // Implement your function here
    vector<string> qvec=getmyword(question);
    triehash* updatehash=new triehash(bmax+3,pgmaxx+3,prmaxx+3,k);
    for (string i:qvec){
        updatehash->insert(dict.get_word_count(i));
    }
    updatehash->heapinsert();
    return updatehash->givelargestk(k);
}

Node* QNA_tool::get_top_k_modified_para(string question,int k){
    vector<string> qvec=getmyword(question);
    triehash* updatehash=new triehash(bmax+3,pgmaxx+3,prmaxx+3,k);
    for (string i:qvec){
        if (!dict.is_unwanted(i)){
        updatehash->insert(dict.get_word_count(i));
        }
    }
    updatehash->heapinsert();
    return updatehash->givelargestk(k);
}

void QNA_tool::query(string question, string filename){
    // Implement your function here
    std::cout << "Q: " << question << std::endl;
    std::cout << "A: " << "Analyzing Gandhi's works..." << std::endl;
    Node* root = get_top_k_modified_para(question, 5);
    
    
    query_llm(filename, root, 5, "", question);
    return;
}

std::string QNA_tool::get_paragraph(int book_code, int page, int paragraph){

    cout << "Book_code: " << book_code << " Page: " << page << " Paragraph: " << paragraph << endl;
    
    // Define the folder path (should match the one in main function)
    std::string folder_path = "gandhi_texts/";
    
    std::string filename = folder_path + "mahatma-gandhi-collected-works-volume-";
    filename += to_string(book_code);
    filename += ".txt";

    std::ifstream inputFile(filename);

    std::string tuple;
    std::string sentence;

    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the input file " << filename << "." << std::endl;
        exit(1);
    }

    std::string res = "";

    while (std::getline(inputFile, tuple, ')') && std::getline(inputFile, sentence)) {
        // Get a line in the sentence
        tuple += ')';

        int metadata[5];
        std::istringstream iss(tuple);

        // Temporary variables for parsing
        std::string token;

        // Ignore the first character (the opening parenthesis)
        iss.ignore(1);

        // Parse and convert the elements to integers
        int idx = 0;
        while (std::getline(iss, token, ',')) {
            // Trim leading and trailing white spaces
            size_t start = token.find_first_not_of(" ");
            size_t end = token.find_last_not_of(" ");
            if (start != std::string::npos && end != std::string::npos) {
                token = token.substr(start, end - start + 1);
            }
            
            // Check if the element is a number or a string
            if (token[0] == '\'') {
                // Remove the single quotes and convert to integer
                int num = std::stoi(token.substr(1, token.length() - 2));
                metadata[idx] = num;
            } else {
                // Convert the element to integer
                int num = std::stoi(token);
                metadata[idx] = num;
            }
            idx++;
        }

        if(
            (metadata[0] == book_code) &&
            (metadata[1] == page) &&
            (metadata[2] == paragraph)
        ){
            res += sentence;
        }
    }

    inputFile.close();
    return res;
}

void QNA_tool::query_llm(string filename, Node* root, int k, string API_KEY, string question){

    // first write the k paragraphs into different files
    Node* traverse = root;
    int num_paragraph = 0;

    while(num_paragraph < k){
        assert(traverse != nullptr);
        string p_file = "paragraph_";
        p_file += to_string(num_paragraph);
        p_file += ".txt";
        // delete the file if it exists
        remove(p_file.c_str());
        ofstream outfile(p_file);
        string paragraph = get_paragraph(traverse->book_code, traverse->page, traverse->paragraph);
        assert(paragraph != "$I$N$V$A$L$I$D$");
        outfile << paragraph;
        outfile.close();
        traverse = traverse->right;
        num_paragraph++;
    }

    // write the query to query.txt
    ofstream outfile("query.txt");
    outfile << "These are the excerpts from Mahatma Gandhi's books.\nOn the basis of this, ";
    outfile << question;
    outfile.close();
 
   
    string command = "python ";
    command += filename;
    command += " ";
    command += to_string(k);
    command += " ";
    command += "query.txt";

    system(command.c_str());
    return;
}
// #undef int