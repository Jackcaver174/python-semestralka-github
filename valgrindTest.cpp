#ifndef __PROGTEST__

#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>
#include <map>

#endif


template<typename T>
struct Node {
    T value;
    Node *parent, *left, *right;

    int height;
    int subtree_size;
    int id_node;

    Node(T val, int _id_node) : value(val), parent(nullptr), left(nullptr), right(nullptr), height(1), subtree_size(1), id_node(_id_node) {}

};

template<typename T>
int calculateHeight(Node<T> *node) {
    return node == nullptr ? 0 : node->height;
}

template<typename T>
int calculateBalance(Node<T> *node) {
    return node == nullptr ? 0 : calculateHeight(node->left) - calculateHeight(node->right);
}

template<typename T>
Node<T> *rotateLeft(Node<T> *x) {
    Node<T> *y = x->right;
    Node<T> *B = y->left;

    y->left = x;
    x->right = B;

    if (B) B->parent = x;
    y->parent = x->parent;
    x->parent = y;

    x->subtree_size = 1 + (x->left ? x->left->subtree_size : 0) + (x->right ? x->right->subtree_size : 0);
    y->subtree_size = 1 + (y->left ? y->left->subtree_size : 0) + (y->right ? y->right->subtree_size : 0);

    x->height = 1 + std::max(calculateHeight(x->left), calculateHeight(x->right));
    y->height = 1 + std::max(calculateHeight(y->left), calculateHeight(y->right));

    return y;
}

template<typename T>
Node<T> *rotateRight(Node<T> *y) {
    Node<T> *x = y->left;
    Node<T> *B = x->right;

    x->right = y;
    y->left = B;

    if (B) B->parent = y;
    x->parent = y->parent;
    y->parent = x;

    y->subtree_size = 1 + (y->left ? y->left->subtree_size : 0) + (y->right ? y->right->subtree_size : 0);
    x->subtree_size = 1 + (x->left ? x->left->subtree_size : 0) + (x->right ? x->right->subtree_size : 0);

    y->height = 1 + std::max(calculateHeight(y->left), calculateHeight(y->right));
    x->height = 1 + std::max(calculateHeight(x->left), calculateHeight(x->right));

    return x;

}

template<typename T>
Node<T> *findInorderSuccessor(Node<T> *current) {
    Node<T> *tmp = current;
    while (tmp->left) tmp = tmp->left;
    return tmp;
}

//https://www.geeksforgeeks.org/insertion-in-an-avl-tree/
//code from this site is used here
template<typename T>
struct Tree {
    size_t size() const { return elementCounter; };

    bool empty() const { return size() == 0; };

    const T &findIndexConst(size_t index) const {
        return findByIndex(root, index)->value;
    }

    T &findIndex(size_t index) const {
        return findByIndex(root, index)->value;
    }

    Node<T> *findByIndex(Node<T> *current, size_t index) const {
        if (!current) return nullptr;

        size_t left_size = current->left ? current->left->subtree_size : 0;

        if (index == left_size) {
            return current;
        } else if (index < left_size) {
            return findByIndex(current->left, index);
        } else {
            return findByIndex(current->right, index - left_size - 1);
        }
    }

    T erase(size_t index) {
        T ans = findIndex(index);
        root = eraseAtIndex(root, index);
        return ans;
    }

    Node<T> *eraseAtIndex(Node<T> *node, int index) {
        if (!node) return nullptr;

        int left_size = node->left ? node->left->subtree_size : 0;

        if (index < left_size) {
            node->left = eraseAtIndex(node->left, index);
            if (node->left) node->left->parent = node;
        } else if (index > left_size) {
            node->right = eraseAtIndex(node->right, index - left_size - 1);
            if (node->right) node->right->parent = node;
        } else {
            if (!node->left || !node->right) {
                Node<T> *temp = node->left ? node->left : node->right;
                if (temp) temp->parent = node->parent;
                delete node;
                return temp;
            } else {
                /*
                Node<T> *successor = findInorderSuccessor(node->right);
                node->value = successor->value;
                node->right = eraseAtIndex(node->right, 0);
                 */


                // Node has two children
                Node<T>* successor = findInorderSuccessor(node->right);

                if (successor->parent != node) {
                    // Update successor's parent's child
                    successor->parent->left = successor->right;
                    if (successor->right) successor->right->parent = successor->parent;

                    // Successor takes node's right child
                    successor->right = node->right;
                    if (successor->right) successor->right->parent = successor;
                }

                // Successor takes node's left child
                successor->left = node->left;
                if (successor->left) successor->left->parent = successor;

                // Successor takes node's parent
                successor->parent = node->parent;
                if (node->parent) {
                    if (node->parent->left == node) node->parent->left = successor;
                    else node->parent->right = successor;
                } else {
                    // Update root if node is root
                    root = successor;
                }

                delete node;

                // Recompute subtree sizes and heights
                successor->subtree_size = 1 + (successor->left ? successor->left->subtree_size : 0) + (successor->right ? successor->right->subtree_size : 0);
                successor->height = 1 + std::max(calculateHeight(successor->left), calculateHeight(successor->right));

                return balance(successor);  // Balance from the successor’s new position

            }
        }

        node->subtree_size = 1 + (node->left ? node->left->subtree_size : 0) +
                             (node->right ? node->right->subtree_size : 0);
        node->height = 1 + std::max(calculateHeight(node->left), calculateHeight(node->right));

        return balance(node);
    }


    Node<T> *balance(Node<T> *current) {
        current->height = 1 + std::max(calculateHeight(current->left), calculateHeight(current->right));

        int balance = calculateHeight(current->left) - calculateHeight(current->right);

        if (balance > 1) {
            if (calculateHeight(current->left->left) >= calculateHeight(current->left->right)) {
                return rotateRight(current);
            } else {
                current->left = rotateLeft(current->left);
                return rotateRight(current);
            }
        }

        if (balance < -1) {
            if (calculateHeight(current->right->right) >= calculateHeight(current->right->left)) {
                return rotateLeft(current);
            } else {
                current->right = rotateRight(current->right);
                return rotateLeft(current);
            }
        }

        return current;
    }

    void insertIndex(int index, T value, int id) {
        root = insertAtIndex(root, index, value, id);
    }

    Node<T> *insertAtIndex(Node<T> *node, int index, T value, int id) {
        if (!node) return new Node<T>(value, id);

        int left_size = node->left ? node->left->subtree_size : 0;

        if (index <= left_size) {
            node->left = insertAtIndex(node->left, index, value, id);
            node->left->parent = node;
        } else {
            node->right = insertAtIndex(node->right, index - left_size - 1, value, id);
            node->right->parent = node;
        }

        node->subtree_size =
                1 + (node->left ? node->left->subtree_size : 0) + (node->right ? node->right->subtree_size : 0);
        node->height = 1 + std::max(calculateHeight(node->left), calculateHeight(node->right));

        return balance(node);
    }

    struct TesterInterface {
        // using Node = ...
        static const Node<T> *root(const Tree *t) { return t->root; }

        // Parent of root must be nullptr, ignore if config::PARENT_POINTERS == false
        static const Node<T> *parent(const Node<T> *n) { return n->parent; }

        static const Node<T> *right(const Node<T> *n) { return n->right; }

        static const Node<T> *left(const Node<T> *n) { return n->left; }

        static const T &value(const Node<T> *n) { return n->value; }
    };


    Tree() : root(nullptr), elementCounter(0) {}

    ~Tree() {
        deleteWholeTree(root);
    }

    void deleteWholeTree(Node<T> *current) {
        if (!current) return;

        deleteWholeTree(current->left);
        deleteWholeTree(current->right);
        delete current;
    }


//private:
    Node<T> *root;
    size_t elementCounter;

};

template<typename T>
struct Array {
    bool empty() const { return size() == 0; };

    size_t size() const { return elementCount; };

    const T &operator[](size_t index) const {
        if (index > size())
            throw std::out_of_range("Inserting out of range.");

        return tree.findIndexConst(index);
    }

    T &operator[](size_t index) {
        if (index > size())
            throw std::out_of_range("Inserting out of range.");

        return tree.findIndex(index);
    }

    Node<T> * findPointer(size_t index) {
        return tree.findByIndex(tree.root, index);
    }


    void insert(size_t index, T value) {
        //std::cout << "Index:" << index << std::endl;
        //std::cout << "Size:" << size() << std::endl;

        if (index > size())
            throw std::out_of_range("Inserting out of range.");

        tree.insertIndex(index, value, id++);
        elementCount++;
    }

    T erase(size_t index) {
        if (index > size())
            throw std::out_of_range("Erasing out of range.");

        elementCount--;
        return tree.erase(index);
    }

    struct TesterInterface {
        // using Node = ...
        static const Node<T> *root(const Array *t) { return t->tree.root; }

        // Parent of root must be nullptr, ignore if config::PARENT_POINTERS == false
        static const Node<T> *parent(const Node<T> *n) { return n->parent; }

        static const Node<T> *right(const Node<T> *n) { return n->right; }

        static const Node<T> *left(const Node<T> *n) { return n->left; }

        static const T &value(const Node<T> *n) { return n->value; }
    };

    Array() : elementCount(0), id(0) {}

private:
    Tree<T> tree;
    size_t elementCount;
    int id;

};

template<typename T>
struct Queue {

    bool empty() const { return array.empty();}

    size_t size() const { return array.size();};

    struct Ref {
        Node<T> * pointer;
        int id_ref;

        Ref(Node<T> * _pointer, int _id_ref) :pointer(_pointer), id_ref(_id_ref) {}

    };

    Ref push_last(T x) {
        array.insert(array.size(), x);

        Node<T> * pointer = array.findPointer(array.size() - 1);
        seen.insert(pointer->id_node);
        //std::cout << "Adding id: " << pointer->id_node << std::endl;

        return {pointer, pointer->id_node};
        //return TODO
    }

    T pop_first() {
        Node<T> * pointer = array.findPointer(0);
        T ans = pointer->value;

        //std::cout << "Removing id: " << pointer->id_node << std::endl;
        seen.erase(pointer->id_node);

        //T ans = array[0];
        array.erase(0);
        return ans;
    }

    size_t position(const Ref &it) const {
        if ( !seen.contains(it.id_ref) ) {
            std::cout << "trying to remove not existing element" << std::endl;
        }


        Node<T> * current = it.pointer;
        size_t index = current->left ? current->left->subtree_size : 0;

        while (current->parent) {
            if (current == current->parent->right) index += 1 + (current->parent->left ? current->parent->left->subtree_size : 0);
            current = current->parent;
        }
        return index;
    };


    void jump_ahead(const Ref &it, size_t positions) {
        //printQueue();
        size_t pos = position(it);
        std::cout << "Element: " << it.pointer->value << " has position: " << pos << std::endl;
        size_t newPos = std::max(0, (int)pos - (int)positions);
        std::cout << "New position: " << newPos << std::endl;
        T element = array[pos];

        array.erase(pos);
        array.insert(newPos, element);
        //printQueue();

    }

    void printQueue() {
        for (int i = 0; i < 10; ++i)
            std::cout << array[std::min(i, 10)] << ", ";
        std::cout << std::endl;
    }


    Queue() : id (0) {}

private:
    Array<T> array;
    std::unordered_set<int> seen;
    int id;

};

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template<typename T>
auto quote(const T &t) { return t; }

std::string quote(const std::string &s) {
    std::string ret = "\"";
    for (char c: s) if (c != '\n') ret += c; else ret += "\\n";
    return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)

////////////////// End of dark magic ////////////////////////


void test1(int &ok, int &fail) {
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);

    constexpr int RUN = 10, TOT = 105;

    for (int i = 0; i < TOT; i++) {
        Q.push_last(i % RUN);
        CHECK(Q.empty(), false);
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++) {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 1 - i);
    }

    CHECK(Q.empty(), true);
}

void test2(int &ok, int &fail) {
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);
    std::vector<decltype(Q.push_last(0))> refs;

    constexpr int RUN = 10, TOT = 105;

    for (int i = 0; i < TOT; i++) {
        refs.push_back(Q.push_last(i % RUN));
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[0], 15);
    Q.jump_ahead(refs[3], 0);

    CHECK(Q.size(), TOT);
    //for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[8], 100);
    Q.jump_ahead(refs[9], 100);
    Q.printQueue();
    Q.jump_ahead(refs[7], 1);

    static_assert(RUN == 10 && TOT >= 30);
    for (int i: {9, 8, 0, 1, 2, 3, 4, 5, 7, 6})
        CHECK(Q.pop_first(), i);

    for (int i = 0; i < TOT * 2 / 3; i++) {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 11 - i);
    }

    CHECK(Q.empty(), false);
}

template<int TOT>
void test_speed(int &ok, int &fail) {
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);
    std::vector<decltype(Q.push_last(0))> refs;

    for (int i = 0; i < TOT; i++) {
        refs.push_back(Q.push_last(i));
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++) {
        CHECK(Q.position(refs[i]), i);
        Q.jump_ahead(refs[i], i);
    }

    for (int i = 0; i < TOT; i++) CHECK(Q.position(refs[i]), TOT - 1 - i);

    for (int i = 0; i < TOT; i++) {
        CHECK(Q.pop_first(), TOT - 1 - i);
        CHECK(Q.size(), TOT - 1 - i);
    }

    CHECK(Q.empty(), true);
}

void my_test() {
    Queue<int> q;
    std::vector<decltype(q.push_last(0))> refs;


    for (int i = 1; i < 10; ++i) {
        refs.push_back(q.push_last(i));
    }


}

int main() {
    //my_test();
    int ok = 0, fail = 0;
    if (!fail) test1(ok, fail);
    if (!fail) test2(ok, fail);
    if (!fail) test_speed<1'000>(ok, fail);

    if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
    else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif



