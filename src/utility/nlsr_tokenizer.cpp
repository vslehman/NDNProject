#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <algorithm>

#include "nlsr_tokenizer.hpp"

namespace nlsr
{

  using namespace std;
  using namespace boost;

  void
  nlsrTokenizer::makeToken()
  {
    char_separator<char> sep(m_seps.c_str());
    tokenizer< char_separator<char> >tokens(m_originalString, sep);
    tokenizer< char_separator<char> >::iterator tok_iter = tokens.begin();
    for ( ; tok_iter != tokens.end(); ++tok_iter)
    {
      string oneToken(*tok_iter);
      trim(oneToken);
      if(!oneToken.empty())
      {
        insertToken(oneToken);
      }
    }
    m_firstToken=m_vTokenList[0];
    makeRestOfTheLine();
  }

  void
  nlsrTokenizer::insertToken(const string& token)
  {
    m_tokenList.push_back(token);
    m_vTokenList.push_back(token);
  }

  int
  nlsrTokenizer::getTokenPosition(string& token)
  {
    int pos=-1;
    int i=0;
    for(std::list<string>::iterator it=m_tokenList.begin();
        it!=m_tokenList.end(); it++)
    {
      if( (*it) == token )
      {
        break;
      }
      i++;
    }
    if( i < m_tokenList.size() )
    {
      pos=i;
    }
    return pos;
  }

  string
  nlsrTokenizer::getTokenString(int from , int to)
  {
    string returnString="";
    if((from>=0 && to<m_tokenList.size()) &&
        (to>=from && to <m_tokenList.size()))
    {
      for(int i=from; i<=to; i++)
      {
        returnString+=m_seps;
        returnString+=m_vTokenList[i];
      }
    }
    trim(returnString);
    return returnString;
  }

  string
  nlsrTokenizer::getTokenString(int from)
  {
    return getTokenString(from,m_tokenList.size()-1);
  }

  static bool
  tokenCompare(string& s1, string& s2)
  {
    return s1==s2;
  }

  void
  nlsrTokenizer::makeRestOfTheLine()
  {
    m_restOfTheLine=getTokenString(1);
  }

  bool
  nlsrTokenizer::doesTokenExist(string token)
  {
    std::list<string >::iterator it = std::find_if( m_tokenList.begin(),
                                      m_tokenList.end(),
                                      bind(&tokenCompare, _1 , token));
    if( it != m_tokenList.end() )
    {
      return true;
    }
    return false;
  }

}//namespace nlsr