#include <vector>
#include <queue>
#include <string>
#include <stdint.h>
#include <string.h>

class SkipAC
{
    static const int char_size = 256;
    static const int max_char_bytes = 6;

    private:
        struct Node;

        //struct State
        union State
        {
            Node* ptr;
            int skip : 3;

            State() = default;
            explicit State(Node* p) : ptr(p) {}

            inline State& operator=(Node *p) {
                ptr = p;
                skip = 0;
                return *this;
            }

            inline operator Node*() const {
                return (Node*)((long long)ptr & ~(long long)0x7);
            }

            inline Node* operator->() const {
                return (Node*)(*this);
            }
        };
        struct Node
        {
            Node* fail;
            int finally;    // if finally node, the value is char count of pattern.
            int deep;
            int follow_bytes;
            bool ref[char_size];
            State next[char_size];

            Node()
                : fail(NULL), finally(0), deep(0), follow_bytes(0), ref{}, next{} {}
        };

        Node* v_root_array_;
        Node* v_root_[max_char_bytes + 1];
        Node root_;
        State state_;

    public:
        SkipAC() : state_{&root_} {
            root_.fail = &root_;
            v_root_[0] = &root_;
            v_root_array_ = new Node[max_char_bytes];
            for (int i = 1; i < max_char_bytes + 1; ++i) {
                v_root_[i] = &v_root_array_[i - 1];
                v_root_[i]->fail = v_root_[i];
                for (int j = 0; j < char_size; ++j) {
                    v_root_[i]->next[j] = v_root_[i - 1];
                    v_root_[i]->ref[j] = true;
                }
            }
        }

        ~SkipAC() {
            Clear();
            delete[] v_root_array_;
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
            int n = utf8_char_c(p);
            if (n <= 0) return false;

            int follow_bytes = 0;
            for (std::size_t i = 0; i < p.size(); ++i) {
                if (!follow_bytes) {
                    follow_bytes = utf8_leader(p[i]);
                }
                unsigned char next_index = p[i];
                if (!pos->next[next_index]) {
                    pos->next[next_index] = new Node{};
                    pos->next[next_index]->deep = pos->deep + 1;
                    pos->next[next_index]->follow_bytes = follow_bytes - 1;
                }

                -- follow_bytes;
                pos = pos->next[next_index];
            }
            pos->finally = n;
            return true;
        }

        void CreateFailPointer() {
            std::queue<Node*> Q;
            for (int i = 0; i < char_size; ++i) {
                if (root_.next[i]) {
                    Q.push(root_.next[i]);
                    root_.next[i]->fail = v_root_[root_.next[i]->follow_bytes];
                }
            }

            while (!Q.empty()) {
                Node* nd = Q.front();
                Q.pop();

                for (int i = 0; i < char_size; ++i) {
                    Node *child = nd->next[i];
                    if (child) {
                        Q.push(child);
                        child->fail = NextState(nd->fail, (char)i);
                        if (child->fail == &root_) {
                            child->fail = v_root_[child->follow_bytes];
                        }
                    }
                }
            }
        }

        void CreateGrah() {
            // 构建Trie图
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
                        nd->next[i] = NextState(nd->fail, (char)i);
                        nd->ref[i] = true;
                    }
                }
            }

            // 优化、加权Trie图
            Q.push(&root_);
            while (!Q.empty()) {
                Node* nd = Q.front();
                Q.pop();

                for (int i = 0; i < char_size; ++i) {
                    Node *child = nd->next[i];
                    if (child >= &v_root_array_[0] && child <= &v_root_array_[max_char_bytes - 1]) {
                        // 指向了虚拟节点, 需要调整至Root并加权
                        nd->next[i] = &root_;
                        nd->next[i].skip = (child - &v_root_array_[0] + 1);
                    } else if (child == &root_) {
                        int follow_bytes = utf8_leader(i);
                        if (follow_bytes > 0)
                            nd->next[i].skip = follow_bytes - 1;
                    }

                    if (!nd->ref[i])
                        Q.push(child);
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
            for (register std::size_t i = 0; i < source.size(); ++i, ++move_cnt) {
                state_ = GraphNextState(state_, source[i]);
                if (state_->finally && cb) {
                    cb(i - state_->deep + 1, source.substr(i - state_->deep + 1, state_->deep));

                    // sub string.
                    Node* pos = state_->fail;
                    for (; pos->finally; pos = pos->fail) {
                        cb(i - pos->deep + 1, source.substr(i - pos->deep + 1, pos->deep));
                    }
                }

                i += state_.skip;
            }
            return move_cnt;
        }

        std::string Replace(std::string const& source)
        {
            Reset();
            int offset = 0;
            std::string result(source);
            for (std::size_t i = 0; i < source.size(); ++i) {
                state_ = NextState(state_, source[i]);
                if (state_->finally) {
                    std::string dst;
                    dst.resize(state_->finally);
                    memset(&dst[0], '*', dst.size());
                    result.replace(i - state_->deep + 1 + offset, state_->deep, dst);
                    offset += (int)dst.size() - (int)state_->deep;
                    Reset();
                }
            }
            return result;
        }

    private:
        State NextState(Node* state, char value) {
            unsigned char i = value;
            if (state->next[i]) {
                return state->next[i];
            } else if (state == &root_) {
                return State{&root_};
            }

            return NextState(state->fail, value);
        }

        State GraphNextState(Node* state, char value) {
            return state->next[(unsigned char)value];
        }

        // -------------- UTF-8 functions --------------
    private:
        // Returns: if c is a utf-8 charset leader byte, return the bytes count of this char.
        //      else return 0.
        int utf8_leader(char c) {
            int zero_index = __builtin_clz(~(unsigned)((unsigned char)c << 24));
            if (zero_index == 0) {
                return 1;
            } else if (zero_index == 1) {
                return 0;
            } else {
                return zero_index;
            }
        }
        // Returns: if string is not utf-8 charset, return -1.
        //      else return the charset count of string.
        int utf8_char_c(std::string const& str)
        {
            int c = 0;
            std::size_t i = 0;
            while (i < str.size()) {
                int leader_c = utf8_leader(str[i]);
                if (!leader_c) return -1;
                for (int j = 1; j < leader_c; ++j) {
                    if (utf8_leader(str[i + j])) return -1;
                }

                ++c;
                i += leader_c;
            }
            return c;
        }

};

