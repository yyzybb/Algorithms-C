#include <vector>
#include <queue>
#include <string>
#include <string.h>

static int utf8_char_c(std::string const& str);

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

        bool AddPattern(std::string const& p) {
            if (p.empty()) return false;

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
            return true;
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

        void CreateGrah() {
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

        int Match(std::string const& source, void(*cb)(int, std::string const&))
        {
            Reset();
            int move_cnt = 0;
            for (std::size_t i = 0; i < source.size(); ++i, ++move_cnt) {
                state_ = GraphNext(state_, source[i]);
                if (state_->finally && cb) {
                    cb(i - state_->deep + 1, source.substr(i - state_->deep + 1, state_->deep));

                    // sub string.
                    Node* pos = state_->fail;
                    for (; pos->finally; pos = pos->fail) {
                        cb(i - pos->deep + 1, source.substr(i - pos->deep + 1, pos->deep));
                    }
                }
            }
            return move_cnt;
        }

        std::string Replace(std::string const& source)
        {
            Reset();
            int offset = 0;
            std::string result(source);
            for (std::size_t i = 0; i < source.size(); ++i) {
                state_ = GraphNext(state_, source[i]);
                if (state_->finally) {
                    std::string pattern = source.substr(i - state_->deep + 1, state_->deep);
                    std::string dst;
                    dst.resize(utf8_char_c(pattern));
                    memset(&dst[0], '*', dst.size());
                    result.replace(i - state_->deep + 1 + offset, pattern.size(), dst);
                    offset += (int)dst.size() - (int)pattern.size();
                    Reset();
                }
            }
            return result;
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

        Node* GraphNext(Node* state, char value) {
            return state->next[(unsigned char)value];
        }
};

static int utf8_char_c(std::string const& str)
{
    int c = 0;
    std::size_t i = 0;
    while (i < str.size()) {
        char ch = str[i];
        int c_len = 1;
        while (ch & 0x80) {
            ch = ch << 1;
            ++c_len;
        }

        if (c_len > 6)
            c_len = 1;

        ++c;
        i += c_len;
    }
    return c;
}

