// Do NOT add any other includes
#include "dict.h"


//Converting a string to lowercase
string tolowerr(string s){
    string d="";
    for (char i:s){
        int k=i-'A';
        if (k>=0 && k<=25) i=i+32;
        d+=i;
    }
    return d;
}

//Breaking a sentence into words
//and returning a vector of words
vector<string> getword(string sentence){
    int n=sentence.length();
    vector<string> v;
    string s="";
    string sp=" .,-:!\"\'()?[];@";
    for (char i:sentence){
        bool flag=false;
        for (char c:sp){
            if (c==i){
                flag=true;
                if (s!="") v.push_back(s);
                s="";
                break;
            }
        }
        if (!flag)  s+=i;
    }
    if (s!="") v.push_back(s);
    return v;

}



Dict::Dict(){}

Dict::~Dict(){
    sr.~trie();    
}

void Dict::insert_sentence(int book_code, int page, int paragraph, int sentence_no, string sentence) {
    vector<string> v = getword(sentence);    // Step 1: Break into words
    for (string i : v) {                     // Step 2: Process each word
        i = tolowerr(i);                     // Step 3: Convert to lowercase
        sr.insert(book_code, page, paragraph, i);  // Step 4: Add to trie
    }
}

/* sr.insert() (this is in the trie implementation):

Creates/updates a word entry in trie
Adds location (book_code, page, paragraph) to word's occurrence list
Updates frequency counts (corpuscount, csvcount)

corpuscount: Total number of times word appears in entire corpus
csvcount: Number of different paragraphs/documents containing this word

Example
word* truth_data = {
    corpuscount: 245,        // "truth" appears 245 times total
    csvcount: 89,           // Found in 89 different paragraphs
    head -> wordnode -> wordnode -> ... -> tail  // List of all locations
}

Result: Every word is now indexed with its exact location*/

word* Dict::get_word_count(string wd){
    return sr.search(wd); //Trie searches for the word and returns pointer to word data structure
}
void Dict::dump_dictionary(string filename){
    // sr.dump(filename); 
    return;
}
bool Dict::is_unwanted(string i){
    return sr.uwords->search(i);  // Checks if the word is in the unwanted words trie
    // Returns true if it is unwanted, false otherwise
}