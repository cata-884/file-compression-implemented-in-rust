#include<bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
//implementarea original e in C++ si de acolo ma inspir pentru Rust
class Node{
public:
    int data;
    Node *left, *right;
    Node(int x){
        data = x;
        left = nullptr;
        right = nullptr;
    }
};
class Compare{
public:
    bool operator() (Node* a, Node* b){
        return a->data > b->data;
    }    
};

void preOrder(Node* root, std::vector<std::string>& ans, std::string curr){
    if(root == nullptr) return;
    if(root->left == nullptr && root->right == nullptr){
        ans.push_back(curr);
        return ;
    }
    preOrder(root->left, ans, curr+'0');
    preOrder(root->right, ans, curr+'1');
}

std::vector<std::string> huffmanCodes(std::string s, std::vector<int> freq){
    int n = s.size();
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;
    for(int i=0; i<n; i++){
        Node* temp = new Node(freq[i]);
        pq.push(temp);
    }
    while(pq.size()>=2){
        Node* l = pq.top();
        pq.pop();
        Node* r = pq.top();
        pq.pop();
        Node* newNode = new Node(l->data + r->data);
        newNode->left = l;
        newNode->right = r;
        pq.push(newNode);
    }
    Node* root = pq.top();
    std::vector<std::string> ans;
    preOrder(root, ans, "");
    return ans;
}

std::vector<uint8_t> bitsToBytes(const std::string& bits) {
    std::vector<uint8_t> bytes;
    uint8_t current_byte = 0;
    int bit_count = 0;

    for (char bit : bits) {
        current_byte = (current_byte << 1) | (bit == '1' ? 1 : 0);
        bit_count++;

        if (bit_count == 8) {
            bytes.push_back(current_byte);
            current_byte = 0;
            bit_count = 0;
        }
    }

    if (bit_count > 0) {
        current_byte <<= (8 - bit_count);
        bytes.push_back(current_byte);
    }

    return bytes;
}
int main(int argc, char* argv[]){ //argv contine cale spre fisier
    if(argc!= 2){
        std::cout<<"nu a fost primit un fisier\n";
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului");
        return 1;
    }
    std::cout << "fisierul s-a deschis cu succes. FD: " << fd << std::endl;
    std::unordered_map<char, int> freqMap;
    char buffer[1024];
    ssize_t bytesRead;
    int totalChars = 0;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            freqMap[buffer[i]]++;
            totalChars++;
        }
    }
    close(fd);
    if(totalChars == 0){
        std::cout<<"Fisierul este gol \n";
        return 1;
    }
    std::string alfabet;
    std::vector<int> freq;
    for (const auto& pair : freqMap) {
        alfabet += pair.first;
        freq.push_back(pair.second);
    }
    std::vector<std::string> codes = huffmanCodes(alfabet, freq);
    std::unordered_map<char, std::string> encoder;

    for (size_t i = 0; i < alfabet.size(); i++) {
        encoder[alfabet[i]] = codes[i];
    }

    char new_path[256];
    strcpy(new_path, argv[1]);
    strcat(new_path, "_compressed");

    int fd_out = open(new_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd_out == -1){
        perror("eroare la deschiderea fisierului de output\n");
        return 1;
    }
    uint32_t unique_chars = alfabet.size();
    write(fd_out, &unique_chars, sizeof(unique_chars));
    for (size_t i = 0; i < alfabet.size(); i++) {
        write(fd_out, &alfabet[i], sizeof(char));
        write(fd_out, &freq[i], sizeof(int));
    }
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Eroare la redeschiderea fisierului pentru compresie");
        close(fd_out);
        return 1;
    }
    std::string compressed_bits;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            char c = buffer[i];
            compressed_bits += encoder[c];
        }
    }
    close(fd);
    auto compressed_bytes = bitsToBytes(compressed_bits);

    // Write compressed data size and data
    uint32_t compressed_size = compressed_bytes.size();
    write(fd_out, &compressed_size, sizeof(compressed_size));
    write(fd_out, compressed_bytes.data(), compressed_bytes.size());

    close(fd_out);
    std::cout << "Compresie completă:\n";
    std::cout << " - Fisier original: " << totalChars << " caractere\n";
    std::cout << " - Caractere unice: " << alfabet.size() << "\n";
    std::cout << " - Date comprimate: " << compressed_bytes.size() << " bytes\n";
    std::cout << " - Fisier output: " << new_path << "\n";
    return 0;
}
