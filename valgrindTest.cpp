#ifndef __PROGTEST__

#include <cassert>
#include <cstdarg>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <array>
#include <random>
#include <type_traits>

// We use std::set as a reference to check our implementation.
// It is not available in progtest :)
#include <set>

template<typename T>
struct Ref {
    size_t size() const { return _data.size(); }

    const T *find(const T &value) const {
        auto it = _data.find(value);
        if (it == _data.end()) return nullptr;
        return &*it;
    }

    bool insert(const T &value) { return _data.insert(value).second; }

    bool erase(const T &value) { return _data.erase(value); }

    auto begin() const { return _data.begin(); }

    auto end() const { return _data.end(); }

private:
    std::set<T> _data;
};

#endif


namespace config {
    // Enable to check that the tree is AVL balanced.
    inline constexpr bool CHECK_DEPTH = true;

    // Disable if your implementation does not have parent pointers
    inline constexpr bool PARENT_POINTERS = false;
}

template<typename T>
struct Node {
    T value;
    Node *parent, *left, *right;

    int height;

    Node(T val) : value(val), parent(nullptr), left(nullptr), right(nullptr), height(1) {}

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
//some code from this site is used here
template<typename T>
struct Tree {
    size_t size() const { return elementCounter; };

    Node<T> *recursiveFind(Node<T> *current, const T &value) const {
        if (!current || current->value == value)
            return current;

        if (value < current->value) {
            return recursiveFind(current->left, value);
        } else {
            return recursiveFind(current->right, value);
        }

    }

    const T *find(const T &value) const {
        return &recursiveFind(root, value)->value;
    };

    Node<T> *recursiveInsert(Node<T> *current, T value) {
        if (!current) {
            elementCounter++;
            return new Node<T>(value);
        }

        if (value < current->value) {
            current->left = recursiveInsert(current->left, value);
        } else if (value > current->value) {
            current->right = recursiveInsert(current->right, value);
        } else return current;

        current->height = 1 + std::max(calculateHeight(current->left), calculateHeight(current->right));

        int balance = calculateHeight(current->left) - calculateHeight(current->right);

        if (balance > 1 && value < current->left->value) {
            return rotateRight(current);
        }

        if (balance < -1 && value > current->right->value) {
            return rotateLeft(current);
        }

        if (balance > 1 && value > current->left->value) {
            current->left = rotateLeft(current->left);
            return rotateRight(current);
        }

        if (balance < -1 && value < current->right->value) {
            current->right = rotateRight(current->right);
            return rotateLeft(current);
        }

        return current;

    }

    bool insert(T value) {
        size_t tmp = elementCounter;
        root = recursiveInsert(root, value);
        return tmp == elementCounter;
    }

    Node<T> *recursiveDelete(Node<T> *current, const T &value) {
        if (!current) return current;

        if (value < current->value) current->left = recursiveDelete(current->left, value);
        else if (value > current->value) current->right = recursiveDelete(current->right, value);
        else {
            if ( !current->left || !current->right) {
                Node<T> * tmp = current->left ? current->left : current->right;

                if (!tmp) {
                    tmp = current;
                    current = nullptr;
                } else *current = *tmp;

                delete tmp;
                elementCounter--;

            } else {
                Node<T> * tmp = findInorderSuccessor(current->right);
                current->value = tmp->value;
                current->right = recursiveDelete(current->right, tmp->value);
            }
        }

        if (!current) return current;

        current->height = 1 + std::max(calculateHeight(current->left), calculateHeight(current->right));
        int balance = calculateBalance(current);

        if (balance > 1 && calculateBalance(current->left) >= 0) {
            return rotateRight(current);
        }

        if (balance > 1 && calculateBalance(current->left) < 0) {
            current->left = rotateLeft(current->left);
            return rotateRight(current);
        }

        if (balance < -1 && calculateBalance(current->right) <= 0) {
            return rotateLeft(current);
        }

        if (balance < -1 && calculateBalance(current->right) > 0) {
            current->right = rotateRight(current->right);
            return rotateLeft(current);
        }

        return current;

    }

    bool erase(const T &value) {
        size_t tmp = elementCounter;
        root = recursiveDelete(root, value);
        return tmp == elementCounter;
    }

    void inorder() {
        inorderRecursive(root);
    }

    void preorder() {
        preorderRecursive(root);
    }

    void preorderRecursive(Node<T> *current) {
        if (!current) return;

        std::cout << current->value << std::endl;
        preorderRecursive(current->left);
        preorderRecursive(current->right);
    }

    void inorderRecursive(Node<T> *current) {
        if (!current) return;

        inorderRecursive(current->left);
        std::cout << current->value << std::endl;
        inorderRecursive(current->right);

    }

    // Needed to test the structure of the tree.
    // Replace Node with the real type of your nodes
    // and implementations with the ones matching
    // your attributes.
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

private:
    Node<T> *root;
    size_t elementCounter;

};


#ifndef __PROGTEST__

struct TestFailed : std::runtime_error {
    using std::runtime_error::runtime_error;
};

std::string fmt(const char *f, ...) {
    va_list args1;
    va_list args2;
    va_start(args1, f);
    va_copy(args2, args1);

    std::string buf(vsnprintf(nullptr, 0, f, args1), '\0');
    va_end(args1);

    vsnprintf(buf.data(), buf.size() + 1, f, args2);
    va_end(args2);

    return buf;
}

template<typename T>
struct Tester {
    Tester() = default;

    void size() const {
        size_t r = ref.size();
        size_t t = tested.size();
        if (r != t) throw TestFailed(fmt("Size: got %zu but expected %zu.", t, r));
    }

    void find(const T &x) const {
        auto r = ref.find(x);
        auto t = tested.find(x);
        bool found_r = r != nullptr;
        bool found_t = t != nullptr;

        if (found_r != found_t) _throw("Find mismatch", found_r);
        if (found_r && *t != x) throw TestFailed("Find: found different value");
    }

    void insert(const T &x, bool check_tree_ = false) {
        auto succ_r = ref.insert(x);
        auto succ_t = tested.insert(x);
        if (succ_r != succ_t) _throw("Insert mismatch", succ_r);
        size();
        if (check_tree_) check_tree();
    }

    void erase(const T &x, bool check_tree_ = false) {
        bool succ_r = ref.erase(x);
        auto succ_t = tested.erase(x);
        if (succ_r != succ_t) _throw("Erase mismatch", succ_r);
        size();
        if (check_tree_) check_tree();
    }

    struct NodeCheckResult {
        const T *min = nullptr;
        const T *max = nullptr;
        int depth = -1;
        size_t size = 0;
    };

    void check_tree() const {
        using TI = typename Tree<T>::TesterInterface;
        auto ref_it = ref.begin();
        bool check_value_failed = false;
        auto check_value = [&](const T &v) {
            if (check_value_failed) return;
            check_value_failed = (ref_it == ref.end() || *ref_it != v);
            if (!check_value_failed) ++ref_it;
        };

        auto r = check_node(TI::root(&tested), decltype(TI::root(&tested))(nullptr), check_value);
        size_t t_size = tested.size();

        if (t_size != r.size)
            throw TestFailed(
                    fmt("Check tree: size() reports %zu but expected %zu.", t_size, r.size));

        if (check_value_failed)
            throw TestFailed(
                    "Check tree: element mismatch");

        size();
    }

    template<typename Node, typename F>
    NodeCheckResult check_node(const Node *n, const Node *p, F &check_value) const {
        if (!n) return {};

        using TI = typename Tree<T>::TesterInterface;
        if constexpr (config::PARENT_POINTERS) {
            if (TI::parent(n) != p) throw TestFailed("Parent mismatch.");
        }

        auto l = check_node(TI::left(n), n, check_value);
        check_value(TI::value(n));
        auto r = check_node(TI::right(n), n, check_value);

        if (l.max && !(*l.max < TI::value(n)))
            throw TestFailed("Max of left subtree is too big.");
        if (r.min && !(TI::value(n) < *r.min))
            throw TestFailed("Min of right subtree is too small.");

        if (config::CHECK_DEPTH && abs(l.depth - r.depth) > 1)
            throw TestFailed(fmt(
                    "Tree is not avl balanced: left depth %i and right depth %i.",
                    l.depth, r.depth
            ));

        return {
                l.min ? l.min : &TI::value(n),
                r.max ? r.max : &TI::value(n),
                std::max(l.depth, r.depth) + 1, 1 + l.size + r.size
        };
    }

    static void _throw(const char *msg, bool s) {
        throw TestFailed(fmt("%s: ref %s.", msg, s ? "succeeded" : "failed"));
    }

    Tree<T> tested;
    Ref<T> ref;
};


void test_insert() {
    Tester<int> t;

    for (int i = 0; i < 10; i++) t.insert(i, true);
    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
    for (int i = -10; i < 20; i++) t.find(i);
}

void test_erase() {
    Tester<int> t;

    for (int i = 0; i < 10; i++) t.insert((1 + i * 7) % 17, true);
    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = 3; i < 22; i += 2) t.erase(i, true);
    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = 0; i < 10; i++) t.insert((1 + i * 13) % 17 - 8, true);
    for (int i = -10; i < 20; i++) t.find(i);

    for (int i = -4; i < 10; i++) t.erase(i, true);
    for (int i = -10; i < 20; i++) t.find(i);
}

enum RandomTestFlags : unsigned {
    SEQ = 1, NO_ERASE = 2, CHECK_TREE = 4
};

void test_random(size_t size, unsigned flags = 0) {
    Tester<size_t> t;
    std::mt19937 my_rand(24707 + size);

    bool seq = flags & SEQ;
    bool erase = !(flags & NO_ERASE);
    bool check_tree = flags & CHECK_TREE;

    for (size_t i = 0; i < size; i++)
        t.insert(seq ? 2 * i : my_rand() % (3 * size), check_tree);

    t.check_tree();

    for (size_t i = 0; i < 3 * size + 1; i++) t.find(i);

    for (size_t i = 0; i < 30 * size; i++)
        switch (my_rand() % 5) {
            case 1:
                t.insert(my_rand() % (3 * size), check_tree);
                break;
            case 2:
                if (erase) t.erase(my_rand() % (3 * size), check_tree);
                break;
            default:
                t.find(my_rand() % (3 * size));
        }

    t.check_tree();
}

void my_test() {

    Tree<int> tree;
    std::vector<int> array({2, 5, 7, 8, 1, 0, 1, 2, 99, 1000, 324, 1343});
    for (int i = 0; i < 2000; ++i) {
        tree.insert(i);
    }

    auto ref = tree.find(1);
    std::cout << *ref << std::endl;
    tree.erase(1);
    std::cout << *ref << std::endl;



}

int main() {

    my_test();

    try {
        std::cout << "Insert test..." << std::endl;
        test_insert();

        std::cout << "Erase test..." << std::endl;
        test_erase();

        std::cout << "Tiny random test..." << std::endl;
        test_random(20, CHECK_TREE);

        std::cout << "Small random test..." << std::endl;
        test_random(200, CHECK_TREE);

        std::cout << "Big random test..." << std::endl;
        test_random(50'000);

        std::cout << "Big sequential test..." << std::endl;
        test_random(50'000, SEQ);

        std::cout << "All tests passed." << std::endl;
    } catch (const TestFailed &e) {
        std::cout << "Test failed: " << e.what() << std::endl;
    }
    /*
     */
}

#endif



