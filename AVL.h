#ifndef AVL_H
#define AVL_H

#include <iostream>
#include <queue>
#include <stdexcept>
#include <initializer_list>

template <typename T>
class AVL {
    struct Node {
        Node() : left{}, right{}, height{1}, value{} {}
        explicit Node(const T& val) : left{}, right{}, height{1}, value{val} {}
        Node* left;
        Node* right;
        int height;
        T value;
    };
public:    
    AVL() : m_root{} , m_size{} {}
    AVL(const std::initializer_list<T>& list) : AVL() {
        for (const auto& el : list) {
            insert(el);
        }
    }
    AVL(const AVL& other) : AVL() {
        auto copy = [this](const T& value) { this->insert(value); };
        other.levelorderTraverse(copy);
    }
    AVL(AVL&& other) noexcept : AVL() {
        swap(other);
    }
    AVL& operator=(const AVL& rhs) {
        auto tmp = rhs;
        swap(tmp);
        return *this;
    }
    AVL& operator=(AVL&& rhs) noexcept {
        if (&rhs == this) {
            return *this;
        }
        swap(rhs);
        rhs.clear();
        return *this;
    }
    void insert(const T& val) { 
        doInsert(m_root ,val);
        ++m_size;
    }
    void remove(const T& val) { 
        m_root = removeNode(m_root, val); 
        --m_size; 
    }
    void clear() {
        doClear(m_root);
        m_root = nullptr;
        m_size = 0;
    }
    void swap(AVL& other) noexcept {
        std::swap(m_size, other.m_size);
        std::swap(m_root, other.m_root);
    }
    template <typename F>
    void inorderTraverse(F f) const {
        doInorder(f, m_root);
    }
    template <typename F>
    void preorderTraverse(F f) const {
        doPreorder(f, m_root);
    }
    template <typename F>
    void postorderTraverse(F f) const {
        doPostorder(f, m_root);
    }
    template <typename F>
    void levelorderTraverse(F f) const {
        doLevelorder(f, m_root);
    }
    const T& max() const {
        if (empty()) {
            throw std::logic_error("AVL is empty. Fail to get max value");
        }
        return getMax(m_root)->value;
    }
    const T& min() const {
        if (empty()) {
            throw std::logic_error("AVL is empty. Fail to get min value");
        }
        return getMin(m_root)->value;
    }
    const T& successorOf(const T& value) {
        Node* res = getSuccessor(m_root, value);
        return res ? res->value : value; // If not successor return same value
    }
    const T& predecessorOf(const T& value) {
        Node* res = getPredecessor(m_root, value);
        return res ? res->value : value; // If not successor return same value
    }
    bool search(const T& value) { return getNode(value, m_root); }
    constexpr size_t size() const { return m_size; }
    constexpr bool empty() const { return !m_size; }
    void printAllOrders() const {
        auto print = [](const T& v) { std::cout << v << " "; };
        std::cout << "inorder" << std::endl;
        inorderTraverse(print);
        std::cout << std::endl;
        std::cout << "preorder" << std::endl;
        preorderTraverse(print);
        std::cout << std::endl;
        std::cout << "postorder" << std::endl;
        postorderTraverse(print);
        std::cout << std::endl;
        std::cout << "levelorder" << std::endl;
        levelorderTraverse(print);
        std::cout << std::endl;
    }
    ~AVL() { clear(); }
private:
    Node* leftRotate(Node* node) {
        if (!node) {
            return node;
        }
        Node* nodeRight = node->right;
        Node* nodeRightLeft = nodeRight->left;
        nodeRight->left = node;
        node->right = nodeRightLeft;
        updateNodeHeight(node);
        updateNodeHeight(node->right);
        if (node == m_root) {
            m_root = nodeRight;
        }
        return nodeRight;
    }
    Node* rightRotate(Node* node) {
        if (!node) {
            return node;
        }
        Node* nodeLeft = node->left;
        Node* nodeLeftRight = nodeLeft->right;
        nodeLeft->right = node;
        node->left = nodeLeftRight;
        updateNodeHeight(node);
        updateNodeHeight(nodeLeft);
        if (node == m_root) {
            m_root = nodeLeft;
        }
        return nodeLeft;
    }
    Node* makeTreeBalanced(Node* node, int balance, const T& value) {
        if (balance > 1 && value < node->left->value) {
            return rightRotate(node);
        }
        if (balance < -1 && value > node->right->value) {
            return leftRotate(node);
        }
        if (balance > 1 && value > node->left->value) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && value < node->right->value) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }
    Node* doInsert(Node* node, const T& value) {
        if (!node) {
            node = new Node(value);
            if (empty()) {
                m_root = node;
            }
            return node;
        }
        if (value > node->value) {
            node->right = doInsert(node->right, value);
        }
        else if (value < node->value) {
            node->left = doInsert(node->left, value);
        }
        else {
            --m_size;
            return node;
        }
        updateNodeHeight(node);
        int balance = getBalance(node);
        if (abs(balance) > 1) {
            return makeTreeBalanced(node, balance, value);
        }
        return node;
    }
    Node* removeNode(Node* node, const T& value) {
        if (!node) {
            return nullptr;
        }
        if (value > node->value) {
            node->right = removeNode(node->right, value);
        }
        else if (value < node->value) {
            node->left = removeNode(node->left, value);
        }
        else {
            if (!node->left) {
                Node* tmp = node;
                node = node->right;
                delete tmp; 
            }
            else if (!node->right) {
                Node* tmp = node;
                node = node->left;
                delete tmp;
            }
            else {
                Node* succesor = getMin(node->right);
                node->value = succesor->value;
                node->right = removeNode(node->right, succesor->value);
            }
        }
        updateNodeHeight(node);
        int balance = getBalance(node);
        if (balance > 1 && getBalance(node->left) >= 0) {
            return rightRotate(node);
        }
        if (balance < -1 && getBalance(node->right) <= 0) {
            return leftRotate(node);
        }
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }
    static int getBalance(Node* node) {
        if (!node) {
            return 0;
        }
        return heightOfNode(node->left) - heightOfNode(node->right);
    }
    static int heightOfNode(Node* node) { return node ? node->height : 0; }
    static void updateNodeHeight(Node* node) {
        if (!node) {
            return;
        }
        node->height = std::max(heightOfNode(node->left), heightOfNode(node->right)) + 1;
    }
    template <typename F>
    static void doInorder(F f, Node* node) {
        if (!node) {
            return;
        }
        doInorder(f, node->left);
        f(node->value);
        doInorder(f, node->right);
    }
    template <typename F>
    static void doPreorder(F f, Node* node) {
        if (!node) {
            return;
        }
        f(node->value);
        doPreorder(f, node->left);
        doPreorder(f, node->right);
    }
    template <typename F>
    static void doPostorder(F f, Node* node) {
        if (!node) {
            return;
        }
        doPostorder(f, node->left);
        doPostorder(f, node->right);
        f(node->value);
    }
    template <typename F>
    static void doLevelorder(F f, Node* node) { 
        if (!node) {
            return;
        }
        std::queue<Node*> queue;
        queue.push(node);
        while (!queue.empty()) {
            auto tmp = queue.front();
            queue.pop();
            f(tmp->value);
            if (tmp->left) {
                queue.push(tmp->left);
            }
            if (tmp->right) {
                queue.push(tmp->right);
            }
        }
    }
    static Node* getMax(Node* node) {
        if (!node) {
            return nullptr;
        }
        while (node->right) {
            node = node->right;
        }
        return node;
    }
    static Node* getMin(Node* node) {
        if (!node) {
            return nullptr;
        }
        while (node->left) {
            node = node->left;
        }
        return node;
    }
    static Node* getNode(const T& value, Node* node) {
        while (node && node->value != value) {
            if (value > node->value) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }
        return node;
    }
      static Node* getSuccessor(Node* root, const T& value) {
        if (!root) {
            return root;
        }
        Node* node = getNode(value, root);
        if (!node) {
            return node;
        }
        if (node->right) {
            return getMin(node->right);
        }
        Node* successor = nullptr;
        while (root->value != value) {
            if (value > root->value) {
                root = root->right;
            }
            else {
                successor = root;
                root = root->left;
            }
        }
        return root ? successor : root;
    }
    static Node* getPredecessor(Node* root, const T& value) {
        if (!root) {
            return root;
        }
        Node* node = getNode(value, root);
        if (!node) {
            return node;
        }
        if (node->left) {
            return getMax(node->left);
        }
        Node* predecessor = nullptr;
        while (root->value != value) {
            if (value > root->value) {
                predecessor = root;
                root = root->right;
            }
            else {
                root = root->left;
            }
        }
        return predecessor;
    }
    static void doClear(Node* node) {
         if (!node) {
            return;
        }
        doClear(node->left);
        doClear(node->right);
        delete node;
    }
    Node* m_root;
    size_t m_size;    
};

#endif
