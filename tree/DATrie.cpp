#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#include "DATrie.h"

DATrie::DATrie()
  :trie_(NULL), alpha_map_(NULL) {
}

DATrie::~DATrie() {
  Clear();
}

bool DATrie::Init() {
  return Reset();
}

/// @brief 从 trie_save() 保存的文件初始化datrie。
bool DATrie::InitFromDictFile(const char* dict_path) {
  Clear();
  trie_ = trie_new_from_file(dict_path);
  return (trie_ != NULL);
}

/// @brief 从文本文件载入词典（词按行分隔）初始化datrie。(strlen(word) <= 256) && ( 自动trim(word) )
bool DATrie::InitFromTxtFile(const char* txt_path) {
  if (!Reset()) {
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
bool DATrie::SaveDictToFile(const char* file_path) {
  return (trie_save(trie_, file_path) == 0);
}

/// @brief 清除datrie。
bool DATrie::Reset() {
  Clear();
  alpha_map_ = alpha_map_new();
  if (!alpha_map_) {
    return false;
  }
  /* Set four char range to map */
  alpha_map_add_range(alpha_map_, 0x00000000, 0xFFFFFFFF);
  trie_ = trie_new(alpha_map_);
  return (trie_ != NULL);
}

/// @brief 清除datrie。
void DATrie::Clear() {
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
bool DATrie::Store(const char* word) {
  int word_len = strlen(word);
  if (word_len > 256) {
    return false;
  }
  AlphaChar alpha_key[257];
  conv_to_alpha(word, word_len, alpha_key, NULL);
  return trie_store(trie_, alpha_key, word_len);
}

/// @brief 从 datrie 里删除一个词。(strlen(word) <= 256)
bool DATrie::Delete(const char* word) {
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
bool DATrie::Match(const char* word) {
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
void DATrie::Prefixes(const char* word, std::vector<std::string>& result) {
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
void DATrie::LongestPrefix(const char* word, std::string& result) {
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
bool DATrie::HasKeysWithPrefix(const char* word) {
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
void DATrie::Search(const char* word, std::vector<std::string>& result) {
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
///        Store("foo"); Store("bar"); Store("foobar"); SearchForwardShortest("foobar", result);
///        result = <"foo", "bar">
void DATrie::SearchForwardShortest(const char* word, std::vector<std::string>& result) {
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
///        Store("foo"); Store("bar"); Store("foobar"); SearchForwardLongest("foobar foobar", result);
///        result = <"foobar", "foobar">
void DATrie::SearchForwardLongest(const char* word, std::vector<std::string>& result) {
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

char* DATrie::trim(char* str) {
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

void DATrie::conv_to_alpha(const char* in, int in_len, AlphaChar* out, int* out_len) {
  int i;
  for (i = 0; i < in_len; i++) {
    out[i] = (AlphaChar) in[i];
  }
  out[in_len] = (AlphaChar) 0;
  if (out_len) {
    *out_len = in_len;
  }
}

