
// compile:
// g++ test_datrie.cpp DATrie.cpp -o test_datrie -ldatrie

#include "DATrie.h"

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
  trie.Prefixes("barfoofoobar", prefixes);
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
  trie4.SearchForwardLongest("foobar, 赶集网，啥都有哦", search_result);
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

