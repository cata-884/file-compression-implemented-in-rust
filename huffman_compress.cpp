#include<bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
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
    uint8_t currentByte = 0;
    int bitCount = 0;

    for (char bit : bits) {
        currentByte = (currentByte << 1) | (bit == '1' ? 1 : 0);
        bitCount++;

        if (bitCount == 8) {
            bytes.push_back(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }

    if (bitCount > 0) {
        currentByte <<= (8 - bitCount);
        bytes.push_back(currentByte);
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
    std::string alphabet;
    std::vector<int> freq;
    for (const auto& pair : freqMap) {
        alphabet += pair.first;
        freq.push_back(pair.second);
    }
    std::vector<std::string> codes = huffmanCodes(alphabet, freq);
    std::unordered_map<char, std::string> encoder;

    for (size_t i = 0; i < alphabet.size(); i++) {
        encoder[alphabet[i]] = codes[i];
    }

    char newPath[256];
    strcpy(newPath, argv[1]);
    strcat(newPath, "_compressed");

    int fdOut = open(newPath, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fdOut == -1){
        perror("eroare la deschiderea fisierului de output\n");
        return 1;
    }
    uint32_t uniqueChars = alphabet.size();
    write(fdOut, &uniqueChars, sizeof(uniqueChars));
    for (size_t i = 0; i < alphabet.size(); i++) {
        write(fdOut, &alphabet[i], sizeof(char));
        write(fdOut, &freq[i], sizeof(int));
    }
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Eroare la redeschiderea fisierului pentru compresie");
        close(fdOut);
        return 1;
    }
    std::string compressedBits;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            char c = buffer[i];
            compressedBits += encoder[c];
        }
    }
    close(fd);
    auto compressedBytes = bitsToBytes(compressedBits);

    uint32_t compressedSize = compressedBytes.size();
    write(fdOut, &compressedSize, sizeof(compressedSize));
    write(fdOut, compressedBytes.data(), compressedBytes.size());

    close(fdOut);
    std::cout << "Compresie completa:\n";
    std::cout << " - Fisier original: " << totalChars << " caractere\n";
    std::cout << " - Caractere unice: " << alphabet.size() << "\n";
    std::cout << " - Date comprimate: " << compressedBytes.size() << " bytes\n";
    std::cout << " - Fisier output: " << newPath << "\n";
    return 0;
}
