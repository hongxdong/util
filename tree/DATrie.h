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
  DATrie();

  ~DATrie();

  bool Init();

  /// @brief 从 trie_save() 保存的文件初始化datrie。
  bool InitFromDictFile(const char* dict_path);

  /// @brief 从文本文件载入词典（词按行分隔）初始化datrie。(strlen(word) <= 256) && ( 自动trim(word) )
  bool InitFromTxtFile(const char* txt_path);

  /// @brief 保存datrie的数据到文件。方便下次直接调用InitFromDictFile载入词典。
  bool SaveDictToFile(const char* file_path);

  /// @brief 重置datrie。
  bool Reset();

  /// @brief 清除datrie。
  void Clear();

  /// @brief 往 datrie 添加一个词。(strlen(word) <= 256)
  bool Store(const char* word);

  /// @brief 从 datrie 里删除一个词。(strlen(word) <= 256)
  bool Delete(const char* word);

  /// @brief 完全匹配一个词是否存在于datrie。(strlen(word) <= 256)
  ///        Store("foo"); Match("foo") = true
  bool Match(const char* word);

  /// @brief 找出 datrie 里等于word前缀的词。(strlen(word) <= 256)
  ///        Store("foo"); Store("foobar"); Prefixes("foobarfoobar", result);
  ///        result = <"foo", "foobar">
  void Prefixes(const char* word, std::vector<std::string>& result);

  /// @brief 找出 datrie 里等于word的最长前缀的词。(strlen(word) <= 256)
  ///        Store("foo"); Store("foobar"); Prefixes("foobarfoobar", result);
  ///        result = "foobar"
  void LongestPrefix(const char* word, std::string& result);

  /// @brief datrie里的词含有前缀为word的词。
  ///        Store("foo"); Store("foobar"); HasKeysWithPrefix("fo") = true
  bool HasKeysWithPrefix(const char* word);

  /// @brief 正向匹配word在datrie里出现的所有的词。
  ///        Store("foo"); Store("bar"); Store("foobar"); Search("foobar", result);
  ///        result = <"foo", "foobar", "bar">
  void Search(const char* word, std::vector<std::string>& result);

  /// @brief 正向最小匹配word在datrie里出现的所有的词，会跳过已匹配的词。
  ///        Store("foo"); Store("bar"); Store("foobar"); SearchForwardShortest("foobar", result);
  ///        result = <"foo", "bar">
  void SearchForwardShortest(const char* word, std::vector<std::string>& result);

  /// @brief 正向最大匹配word在datrie里出现的所有的词，会跳过已匹配的词。
  ///        Store("foo"); Store("bar"); Store("foobar"); SearchForwardLongest("foobar foobar", result);
  ///        result = <"foobar", "foobar">
  void SearchForwardLongest(const char* word, std::vector<std::string>& result);

 private:
  char* trim(char* str);

  void conv_to_alpha(const char* in, int in_len, AlphaChar* out, int* out_len);

 private:
  Trie* trie_;
  AlphaMap* alpha_map_;
};

