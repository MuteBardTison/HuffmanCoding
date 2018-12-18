// Zihan Qi 2018/12/17
// Huffman Coding Algorithm
// Pseudocode is taken from Introduction to Algorithms(Second Edition) Chapter 16.3.
#include <queue>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <set>
#include <cassert>

struct HuffmanTreeNode {
    char character;
    unsigned frequency;
    HuffmanTreeNode* left;
    HuffmanTreeNode* right;

    HuffmanTreeNode(char character, unsigned frequency, HuffmanTreeNode* left = nullptr, HuffmanTreeNode* right = nullptr) :
        character(character), frequency(frequency), left(left), right(right) {
    }

    struct Compare {
        bool operator()(HuffmanTreeNode* node_one, HuffmanTreeNode* node_two) const {
            return node_one->frequency > node_two->frequency;
        }
    };
};

void CreateFrequencyTable(const std::string& text, std::unordered_map<char, unsigned>& frequency_table) {
    for(const auto& character : text) {
        if(frequency_table.find(character) == frequency_table.end())
            frequency_table[character] = 1;
        else
            frequency_table[character] += 1;
    }
}

// HUFFMAN(C)
// 1 n <- |C|
// 2 Q <- C
// 3 for i <- 1 to n-1
// 4   do allocate a new node z
// 5   left[z] <- x <- EXTRACT-MIN(Q)
// 6   right[z] <- y <- EXTRACT-MIN(Q)
// 7   f[z] <- f[x] + f[y]
// 8   INSERT(Q, z)
// 9 return EXTRACT-MIN(Q)  //return the root of the tree
HuffmanTreeNode* BuildHuffmanTree(const std::string& text) {
    std::unordered_map<char, unsigned> frequency_table;
    CreateFrequencyTable(text, frequency_table);
    std::priority_queue<HuffmanTreeNode*, std::vector<HuffmanTreeNode*>, HuffmanTreeNode::Compare> min_heap;
    std::set<char> characters(text.begin(), text.end());
    for(const auto& character : characters) {
        min_heap.push(new HuffmanTreeNode(character, frequency_table[character]));
    }
    HuffmanTreeNode* root = nullptr;
    while(min_heap.size() > 1) {
        HuffmanTreeNode* left_node = min_heap.top();
        min_heap.pop();
        HuffmanTreeNode* right_node = min_heap.top();
        min_heap.pop();
        root = new HuffmanTreeNode(-1, left_node->frequency + right_node->frequency, left_node, right_node);
        min_heap.push(root);
    }
    return root;
}

void PrintTree(const std::string& prefix, HuffmanTreeNode* node, bool is_left) {
    if(node) {
        std::cout << prefix;
        std::cout << (is_left ? "├────" : "└────" );
        std::cout<< node->character << " " << node->frequency << std::endl;
        PrintTree(prefix + (is_left ? "│    " : "     "), node->left, true);
        PrintTree(prefix + (is_left ? "│    " : "     "), node->right, false);
    }
}

void CreateLookupTable(HuffmanTreeNode* node, std::unordered_map<char, std::string>& lookup_table, std::string code) {
    if(node) {
        if(node->character > -1) {
            lookup_table[node->character] = code;
        }
        CreateLookupTable(node->left, lookup_table, code + "0"); 
        CreateLookupTable(node->right, lookup_table, code + "1"); 
    }
}

void PrintLookupTable(const std::unordered_map<char, std::string>& lookup_table) {
    for(const auto& pair : lookup_table) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
}

std::string Encode(const std::string& text) {
    std::string encoded_text;
    HuffmanTreeNode* root = BuildHuffmanTree(text);
    std::unordered_map<char, std::string> lookup_table;
    CreateLookupTable(root, lookup_table, "");
    for(const auto& character : text) {
        encoded_text += lookup_table[character];
    }
    return encoded_text;
}

std::string Decode(const std::string encoded_text, HuffmanTreeNode* root) {
    std::string decoded_text;
    HuffmanTreeNode* current_node = root;
    for(const auto& character : encoded_text) {
        if(character == '0')
            current_node = current_node->left;
        else if(character == '1')
            current_node = current_node->right;
        if(current_node->character > -1) {
            decoded_text += current_node->character;
            current_node = root;
        }
    }
    return decoded_text;
}

int main() {
    const std::string text("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbccccccccccccddddddddddddddddeeeeeeeeefffff");
    HuffmanTreeNode* root = BuildHuffmanTree(text);
    PrintTree("", root, false);

    std::unordered_map<char, std::string> lookup_table;
    CreateLookupTable(root, lookup_table, "");
    PrintLookupTable(lookup_table);

    std::cout << Encode(text) << std::endl;
    std::cout << Decode(Encode(text), root) << std::endl;
    assert(Decode(Encode(text), root) == text);

    std::cout << "Before: " << text.size() * 8 << " Bits." << std::endl;
    std::cout << "After: " << Encode(text).size() << " Bits." << std::endl;
    std::cout << "Compression Ratio: " << static_cast<double>(Encode(text).size()) / (text.size() * 8) * 100 << "%" << std::endl;
    return 0;
}