#include <iostream>
#include <vector>
#include <queue>

class AC_Automan
{
    static const int char_size = 256;

    private:
        struct Node
        {
            bool finally;
            int deep;
            Node* fail;
            Node* next[char_size];
            bool ref[char_size];

            Node() : finally(false), deep(0), fail(NULL), next{}, ref{} {}
        };

        Node root_;
        Node* state_;

    public:
        AC_Automan() : state_(&root_) {
            root_.fail = &root_;
        }

        ~AC_Automan() {
            Clear();
        }

        void Clear() {
            std::queue<Node*> Q;
            for (int i = 0; i < char_size; ++i) {
                Node *child = root_.next[i];
                root_.next[i] = NULL;
                if (root_.ref[i]) continue;
                if (!child) continue;
                Q.push(child);
            }

            while (!Q.empty()) {
                Node* nd = Q.front();
                Q.pop();

                for (int i = 0; i < char_size; ++i) {
                    Node *child = nd->next[i];
                    if (!child) continue;

                    if (!nd->ref[i]) {
                        Q.push(child);
                    }
                }

                delete nd;
            }
        }

        void AddPattern(std::string const& p) {
            if (p.empty()) return ;

            Node* pos = &root_;
            for (std::size_t i = 0; i < p.size(); ++i) {
                unsigned char next_index = p[i];
                if (!pos->next[next_index]) {
                    pos->next[next_index] = new Node{};
                    pos->next[next_index]->deep = pos->deep + 1;
                }

                pos = pos->next[next_index];
            }
            pos->finally = true;
        }

        void CreateFailPointer() {
            std::queue<Node*> Q;
            for (int i = 0; i < char_size; ++i) {
                if (root_.next[i]) {
                    Q.push(root_.next[i]);
                    root_.next[i]->fail = &root_;
                }
            }

            while (!Q.empty()) {
                Node* nd = Q.front();
                Q.pop();

                for (int i = 0; i < char_size; ++i) {
                    Node *child = nd->next[i];
                    if (child) {
                        Q.push(child);
                        child->fail = Next(nd->fail, (char)i);
                    }
                }
            }
        }

        void CreateMatrix() {
            std::queue<Node*> Q;
            Q.push(&root_);

            while (!Q.empty()) {
                Node* nd = Q.front();
                Q.pop();

                for (int i = 0; i < char_size; ++i) {
                    Node *child = nd->next[i];
                    if (child) {
                        Q.push(child);
                    } else {
                        nd->next[i] = Next(nd->fail, (char)i);
                        nd->ref[i] = true;
                    }
                }
            }
        }

        void Reset() {
            state_ = &root_;
        }

        void solve(std::string const& source, void(*cb)(int, int, std::string const&))
        {
            Reset();
            for (int i = 0; i < source.size(); ++i) {
                state_ = Next(state_, source[i]);
                if (state_->finally) {
                    cb(i - state_->deep + 1, state_->deep, source);

                    // sub string.
                    Node* pos = state_->fail;
                    for (; pos->finally; pos = pos->fail) {
                        cb(i - pos->deep + 1, pos->deep, source);
                    }
                }
            }
        }

    private:
        Node* Next(Node* state, char value) {
            unsigned char i = value;
            if (state->next[i]) {
                return state->next[i];
            } else if (state == &root_) {
                return &root_;
            }

            return Next(state->fail, value);
        }
};

void dump(int pos, int n, std::string const& source) {
    std::cout << "match [" << pos << "]. keyword: [" << source.substr(pos, n) << "]" << std::endl;
}

int main()
{
    const char* patterns[] = {
        "she",
        "he",
        "say",
        "shr",
        "her"
    };

    std::string source = "yasherhs";
    AC_Automan ac;
    for (int i = 0; i < sizeof(patterns) / sizeof(const char*); ++i)
        ac.AddPattern(patterns[i]);
    ac.CreateFailPointer();
    ac.CreateMatrix();
    ac.solve(source, dump);
    ac.Clear();
    return 0;
}

