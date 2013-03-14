#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
extern "C" {
  #include <datrie/trie.h>
}

/// @class DATrie 是使用 libdatrie 封装的双队列trie树，
/// @brief 关于 datrie 请参考：http://linux.thai.net/~thep/datrie/datrie.html
///        Ubuntu 环境下可以使用 sudo apt-get install libdatrie-dev 安装依赖文件。
///        其他Linux环境，可以使用上面网址提供下载的libdatrie-x.x.tar.gz 编译安装。
class DATrie {
 public:
  DATrie()
    :trie_(NULL), alpha_map_(NULL) {
  }

  ~DATrie() {
    Reset();
  }

  bool Init() {
    Reset();
    alpha_map_ = alpha_map_new();
    if (!alpha_map_) {
      return false;
    }
    /* Set four char range to map */
    alpha_map_add_range(alpha_map_, 0x00000000, 0xFFFFFFFF);
    trie_ = trie_new(alpha_map_);
    return (trie_ != NULL);
  }

  /// @brief 从 trie_save() 保存的文件初始化datrie。
  bool InitFromDictFile(const char* dict_path) {
    Reset();
    trie_ = trie_new_from_file(dict_path);
    return (trie_ != NULL);
  }

  /// @brief 从文本文件载入词典（词按行分隔）初始化datrie。(strlen(word) <= 256) && ( 自动trim(word) )
  bool InitFromTxtFile(const char* txt_path) {
    if (!Init()) {
      return false;
    }
    FILE *fp;
    char buf[1024], *str;
    AlphaChar alpha_key[257];
    int length;
    fp = fopen(txt_path, "r");
    if (!fp) {
      return false;
    }
    while (fgets(buf, 1024, fp)) {
      str = trim(buf);
      length = strlen(str);
      if (length > 256) {
        return false;
      }
      conv_to_alpha(str, length, alpha_key, NULL);
      if (!trie_store(trie_, alpha_key, length)) {
        return false;
      }
    }
    fclose(fp);
    return true;
  }

  /// @brief 保存datrie的数据到文件。方便下次直接调用InitFromDictFile载入词典。
  bool SaveDictToFile(const char* file_path) {
    return (trie_save(trie_, file_path) == 0);
  }

  /// @brief 清除datrie。
  void Reset() {
    if (trie_ != NULL) {
      trie_free(trie_);
      trie_ = NULL;
    }
    if (alpha_map_ != NULL) {
      alpha_map_free(alpha_map_);
      alpha_map_ = NULL;
    }
  }

  /// @brief 往 datrie 添加一个词。(strlen(word) <= 256)
  bool Store(const char* word) {
    int word_len = strlen(word);
    if (word_len > 256) {
      return false;
    }
    AlphaChar alpha_key[257];
    conv_to_alpha(word, word_len, alpha_key, NULL);
    return trie_store(trie_, alpha_key, word_len);
  }

  /// @brief 从 datrie 里删除一个词。(strlen(word) <= 256)
  bool Delete(const char* word) {
    int word_len = strlen(word);
    if (word_len > 256) {
      return false;
    }
    AlphaChar alpha_key[257];
    conv_to_alpha(word, word_len, alpha_key, NULL);
    return trie_delete(trie_, alpha_key);
  }

  /// @brief 完全匹配一个词是否存在于datrie。(strlen(word) <= 256)
  ///        Store("foo"); Match("foo") = true
  bool Match(const char* word) {
    TrieState* state = trie_root(trie_);
    if (state == NULL) {
      return false;
    }
    const char* ptr = word;
    while (*ptr) {
      if (!trie_state_walk(state, (AlphaChar)(*ptr++))) {
        break;
      }
    }
    bool is_match = trie_state_is_terminal(state);
    trie_state_free(state);
    return is_match;
  }

  /// @brief 找出 datrie 里等于word前缀的词。(strlen(word) <= 256)
  ///        Store("foo"); Store("foobar"); Prefixes("foobarfoobar", result);
  ///        result = <"foo", "foobar">
  void Prefixes(const char* word, std::vector<std::string>& result) {
    result.clear();
    TrieState* state = trie_root(trie_);
    if (state == NULL) {
      return;
    }
    const char* ptr = word;
    while (*ptr) {
      if (!trie_state_walk(state, (AlphaChar)(*ptr++))) {
        break;
      }
      if (trie_state_is_terminal(state)) {
        result.push_back(std::string(word, ptr - word));
      }
    }
    trie_state_free(state);
  }

  /// @brief 找出 datrie 里等于word的最长前缀的词。(strlen(word) <= 256)
  ///        Store("foo"); Store("foobar"); Prefixes("foobarfoobar", result);
  ///        result = "foobar"
  void LongestPrefix(const char* word, std::string& result) {
    result.clear();
    TrieState* state = trie_root(trie_);
    if (state == NULL) {
      return;
    }
    const char* ptr = word;
    const char* end_ptr = NULL;
    while (*ptr) {
      if (!trie_state_walk(state, (AlphaChar)(*ptr++))) {
        break;
      }
      if (trie_state_is_terminal(state)) {
        end_ptr = ptr;
      }
    }
    if (end_ptr != NULL) {
      result.assign(word, end_ptr - word);
    }
    trie_state_free(state);
  }

  /// @brief datrie里的词含有前缀为word的词。
  ///        Store("foo"); Store("foobar"); HasKeysWithPrefix("fo") = true
  bool HasKeysWithPrefix(const char* word) {
    TrieState* state = trie_root(trie_);
    if (state == NULL) {
      return false;
    }
    bool has_key_with_prefix = true;
    const char* ptr = word;
    while (*ptr) {
      if (!trie_state_walk(state, (AlphaChar)(*ptr++))) {
        has_key_with_prefix = false;
        break;
      }
    }
    trie_state_free(state);
    return has_key_with_prefix;
  }

  /// @brief 正向匹配word在datrie里出现的所有的词。
  ///        Store("foo"); Store("bar"); Store("foobar"); Search("foobar", result);
  ///        result = <"foo", "foobar", "bar">
  void Search(const char* word, std::vector<std::string>& result) {
    result.clear();
    const char* begin_ptr = word;
    const char* end_ptr   = NULL;
    while (*begin_ptr) {
      TrieState* state = trie_root(trie_);
      if (state == NULL) {
        return;
      }
      end_ptr = begin_ptr;
      while (*end_ptr) {
        if (!trie_state_walk(state, (AlphaChar)(*end_ptr++))) {
          break;
        }
        if (trie_state_is_terminal(state)) {
          result.push_back(std::string(begin_ptr, end_ptr - begin_ptr));
        }
      }
      trie_state_free(state);
      ++begin_ptr;
    }
  }

  /// @brief 正向最小匹配word在datrie里出现的所有的词，会跳过已匹配的词。
  ///        Store("foo"); Store("bar"); Store("foobar"); Search("foobar", result);
  ///        result = <"foo", "bar">
  void SearchForwardShortest(const char* word, std::vector<std::string>& result) {
    result.clear();
    const char* begin_ptr = word;
    const char* end_ptr   = NULL;
    while (*begin_ptr) {
      TrieState* state = trie_root(trie_);
      if (state == NULL) {
        return;
      }
      end_ptr = begin_ptr;
      while (*end_ptr) {
        if (!trie_state_walk(state, (AlphaChar)(*end_ptr++))) {
          break;
        }
        if (trie_state_is_terminal(state)) {
          result.push_back(std::string(begin_ptr, end_ptr - begin_ptr));
          begin_ptr += end_ptr - begin_ptr - 1;
          break;
        }
      }
      trie_state_free(state);
      ++begin_ptr;
    }
  }

  /// @brief 正向最大匹配word在datrie里出现的所有的词，会跳过已匹配的词。
  ///        Store("foo"); Store("bar"); Store("foobar"); Search("foobar foobar", result);
  ///        result = <"foobar", "foobar">
  void SearchForwardLongest(const char* word, std::vector<std::string>& result) {
    result.clear();
    const char* begin_ptr = word;
    const char* end_ptr   = NULL;
    while (*begin_ptr) {
      TrieState* state = trie_root(trie_);
      if (state == NULL) {
        return;
      }
      end_ptr = begin_ptr;
      const char* stop_ptr = NULL;
      while (*end_ptr) {
        if (!trie_state_walk(state, (AlphaChar)(*end_ptr++))) {
          break;
        }
        if (trie_state_is_terminal(state)) {
          stop_ptr = end_ptr;
        }
      }
      if (stop_ptr != NULL) {
        result.push_back(std::string(begin_ptr, stop_ptr - begin_ptr));
        begin_ptr += stop_ptr - begin_ptr - 1;
      }
      trie_state_free(state);
      ++begin_ptr;
    }
  }

 private:
  char* trim(char* str) {
    #define is_space(ch) ((ch) == ' ' || (ch) == '\r' || (ch) == '\t' || (ch) == '\n')
    char *ptr = str, *end;
    while (*ptr) {
        if (is_space(*ptr)) {
            ptr++;
        } else {
            break;
        }
    }
    end = str + strlen(str) - 1;
    while (end >= ptr) {
        if (is_space(*end)) {
            *end = '\0';
            end--;
        } else {
            break;
        }
    }
    return ptr;
  }

  void conv_to_alpha(const char* in, int in_len, AlphaChar* out, int* out_len) {
    int i;
    for (i = 0; i < in_len; i++) {
        out[i] = (AlphaChar) in[i];
    }
    out[in_len] = (AlphaChar) 0;
    if (out_len) {
      *out_len = in_len;
    }
  }

 private:
  Trie* trie_;
  AlphaMap* alpha_map_;
};


int main(void) {
  DATrie trie;
  trie.Init();

  // Store, Match
  printf("add 'foobar'   : %d\n", trie.Store("foobar"));
  printf("add 'foo'      : %d\n", trie.Store("foo"));
  printf("match 'foobar' : %d\n", trie.Match("foobar"));
  printf("match 'foo'    : %d\n", trie.Match("foo"));
  printf("match 'foob'   : %d\n", trie.Match("foob"));
  printf("match 'bar'    : %d\n", trie.Match("bar"));
  printf("\n");

  // Prefixes
  std::vector<std::string> prefixes;
  trie.Prefixes("foobarfoobar", prefixes);
  for (size_t i = 0; i < prefixes.size(); ++i) {
    printf("prefix         : %s\n", prefixes[i].c_str());
  }
  trie.Prefixes("barfoo", prefixes);
  for (size_t i = 0; i < prefixes.size(); ++i) {
    printf("prefix         : %s\n", prefixes[i].c_str());
  }
  printf("\n");

  // LongestPrefix
  std::string longest_prefix;
  trie.LongestPrefix("foobarfoobar", longest_prefix);
  printf("LongestPrefix  : %s\n", longest_prefix.c_str());
  trie.LongestPrefix("barbar", longest_prefix);
  printf("LongestPrefix  : %s\n", longest_prefix.c_str());
  printf("\n");

  printf("HasKeysWithPrefix : %d\n", trie.HasKeysWithPrefix("fo"));
  printf("HasKeysWithPrefix : %d\n", trie.HasKeysWithPrefix("foobar"));
  printf("HasKeysWithPrefix : %d\n", trie.HasKeysWithPrefix("foobartest"));
  printf("HasKeysWithPrefix : %d\n", trie.HasKeysWithPrefix("bar"));
  printf("HasKeysWithPrefix : %d\n", trie.HasKeysWithPrefix("fob"));
  printf("\n");

  printf("SaveDictToFile: %d\n", trie.SaveDictToFile("./foo.dict"));

  DATrie trie2;
  printf("InitFromDictFile: %d\n", trie2.InitFromDictFile("./foo.dict"));
  trie2.LongestPrefix("foobarfoobar", longest_prefix);
  printf("LongestPrefix  : %s\n", longest_prefix.c_str());
  printf("\n");

  DATrie trie3;
  printf("InitFromTxtFile: %d\n", trie3.InitFromTxtFile("./foo.txt"));
  trie3.LongestPrefix("赶集网，啥都有。", longest_prefix);
  printf("LongestPrefix  : %s\n", longest_prefix.c_str());
  printf("\n");

  DATrie trie4;
  trie4.Init();
  trie4.Store("foo");
  trie4.Store("bar");
  trie4.Store("foobar");
  trie4.Store("赶集网");
  trie4.Store("啥都有");
  trie4.Store("赶集网，啥都有");
  std::vector<std::string> search_result;
  trie4.Search("foobar 赶集网，啥都有", search_result);
  for (size_t i = 0; i < search_result.size(); ++i) {
    printf("search    : %s\n", search_result[i].c_str());
  }
  printf("\n");
  trie4.SearchForwardShortest("foobar 赶集网，啥都有", search_result);
  for (size_t i = 0; i < search_result.size(); ++i) {
    printf("search_forward_shortest    : %s\n", search_result[i].c_str());
  }
  printf("\n");
  trie4.SearchForwardLongest("foobar赶集网，啥都有哦", search_result);
  for (size_t i = 0; i < search_result.size(); ++i) {
    printf("search_forward_longest    : %s\n", search_result[i].c_str());
  }
  printf("\n");
  trie4.SearchForwardLongest("foobar foobar", search_result);
  for (size_t i = 0; i < search_result.size(); ++i) {
    printf("search_forward_longest    : %s\n", search_result[i].c_str());
  }

  return 0;
}

