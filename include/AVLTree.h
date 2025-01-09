#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

#ifndef NODE_H
#define NODE_H

// Node class for AVL Tree
template <typename T>
class Node {
public:
    T data;
    Node<T>* left;
    Node<T>* right;
    int height;

    Node(const T& data) : data(data), left(nullptr), right(nullptr), height(0) {}

    ~Node() = default;

    // Returns a string representation of the node
    std::string toString() const {
        std::stringstream ss;
        ss << "(" << data.toString();
        
        if (left != nullptr) {
            ss << " L:" << left->toString();
        }
        if (right != nullptr) {
            ss << " R:" << right->toString();
        }
        ss << ")";
        
        return ss.str();
    }
    // Updates the height of the node by getting the max height of the left and right children
    void updateHeight() {
        int leftHeight = (left == nullptr) ? -1 : left->height;
        int rightHeight = (right == nullptr) ? -1 : right->height;
        height = 1 + std::max(leftHeight, rightHeight);
    }
    // Returns the balance factor of the node by getting the difference in height of the left and right children
    int getBalance() const {
        int leftHeight = (left == nullptr) ? -1 : left->height;
        int rightHeight = (right == nullptr) ? -1 : right->height;
        return leftHeight - rightHeight;
    }
    // Comparison operators for ordering
    bool operator<(const Node& other) const {
        return data < other.data;
    }

    bool operator>(const Node& other) const {
        return data > other.data;
    }

    bool operator==(const Node& other) const {
        return data == other.data;
    }

    bool operator!=(const Node& other) const {
        return !(*this == other);
    }

    bool operator<=(const Node& other) const {
        return !(*this > other);
    }

    bool operator>=(const Node& other) const {
        return !(*this < other);
    }
};
#endif

#ifndef AVLTREE_H
#define AVLTREE_H
// AVL Tree class
template <typename T>
class AVLTree {
public:
    Node<T>* root;
    int size;

    AVLTree() : root(nullptr), size(0) {}

    ~AVLTree() {
        destroyTree(root);
    }
    // Prints the tree to the console for debugging
    void printTree() {
        if (root != nullptr) {
            std::cout << root->toString() << std::endl;
        } else {
            std::cout << "Empty tree" << std::endl;
        }
    }

private:
// Helper functions
    // Does a post order traversal to deallocate memory
    void destroyTree(Node<T>* node) {
        if (node == nullptr) return;

        destroyTree(node->left);
        node->left = nullptr; // Prevent dangling pointer

        destroyTree(node->right);
        node->right = nullptr; // Prevent dangling pointer

        delete node;
    }
    // Does a single rotation to the right
    Node<T>* rightRotate(Node<T>* y) {
        Node<T>* x = y->left;
        Node<T>* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->updateHeight();
        x->updateHeight();

        return x;
    }
    // Does a single rotation to the left
    Node<T>* leftRotate(Node<T>* x) {
        Node<T>* y = x->right;
        Node<T>* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->updateHeight();
        y->updateHeight();

        return y;
    }
    // Inserts a node into the tree
    Node<T>* insert(Node<T>* node, const T& data) {
        if (node == nullptr) {
            size++;
            return new Node<T>(data);
        }
        // Find the correct position to insert the node
        if (data < node->data) {
            node->left = insert(node->left, data);
        } else if (data > node->data) {
            node->right = insert(node->right, data);
        } else {
            return node; // No duplicates allowed
        }
        // Update the height of the current node
        node->updateHeight();
        // Get the balance factor
        int balance = node->getBalance();
        // If needed balance the tree
        // Left Left Case
        if (balance > 1 && data < node->left->data) {
            return rightRotate(node);
        }

        // Right Right Case
        if (balance < -1 && data > node->right->data) {
            return leftRotate(node);
        }

        // Left Right Case
        if (balance > 1 && data > node->left->data) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Left Case
        if (balance < -1 && data < node->right->data) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }
    // Helper function for remove node. Returns the smallest node in the right subtree
    Node<T>* getSmallestNode(Node<T>* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }
    // Removes a node from the tree
    Node<T>* remove(Node<T>* node, const T& data) {
        if (node == nullptr) return node;
        // Find the node to delete
        if (data < node->data) {
            node->left = remove(node->left, data);
        } else if (data > node->data) {
            node->right = remove(node->right, data);
        } else {
            // Found the node to delete
            if (node->left == nullptr || node->right == nullptr) {
                // Case 1 & 2: No child or one child
                Node<T>* temp = node->left ? node->left : node->right;
                
                if (temp == nullptr) {
                    // No child case
                    temp = node;
                    node = nullptr;
                } else {
                    // One child case
                    *node = *temp; // Copy the contents
                }
                
                delete temp;
                size--;
            } else {
                // Case 3: Two children
                // Get inorder successor (smallest in right subtree)
                Node<T>* temp = getSmallestNode(node->right);
                
                // Copy the inorder successor's data to this node
                node->data = temp->data;
                
                // Delete the inorder successor
                node->right = remove(node->right, temp->data);
            }
        }

        if (node == nullptr) return node;

        // Update height of current node
        node->updateHeight();

        // Get the balance factor
        int balance = node->getBalance();

        // Perform rotations if needed
        if (balance > 1) {
            int leftBalance = node->left ? node->left->getBalance() : 0;
            if (leftBalance >= 0) {
                return rightRotate(node);
            }
            if (leftBalance < 0 && node->left) {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
        }

        if (balance < -1) {
            int rightBalance = node->right ? node->right->getBalance() : 0;
            if (rightBalance <= 0) {
                return leftRotate(node);
            }
            if (rightBalance > 0 && node->right) {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }
        }

        return node;
    }
    // Converts the tree to a vector by doing an inorder traversal
    void toVector(Node<T>* node, std::vector<T>& vec) {
        if (node == nullptr) return;
        toVector(node->left, vec);
        vec.push_back(node->data);
        toVector(node->right, vec);
    }

public:
// Public functions
    // Inserts a data into the tree
    void insert(const T& data) {
        root = insert(root, data);
    }
    // Removes a data from the tree
    void remove(const T& data) {
        root = remove(root, data);
    }
    // Finds a node in the tree
    bool find(const T& data) {
        Node<T>* curr = root;
        while (curr != nullptr) {
            if (curr->data == data) {
                return true;
            } else if (data < curr->data) {
                curr = curr->left;
            } else {
                curr = curr->right;
            }
        }
        return false;
    }
    // Converts the tree to a vector
    std::vector<T> toVector() {
        std::vector<T> vec;
        toVector(root, vec);
        return vec;
    }
    // Checks if the tree is empty
    bool isEmpty() const {
        return size == 0;
    }
    // Gets the size of the tree
    int getSize() const {
        return size;
    }
};
#endif