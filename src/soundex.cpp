// Copyright (c) 2015, James P. Howard, II <jh@jameshoward.us>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//
//     Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in
//     the documentation and/or other materials provided with the
//     distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// [[Rcpp::depends(BH)]]
#include <Rcpp.h>
#include <boost/algorithm/string.hpp>

using namespace Rcpp;
using namespace boost;
using namespace std;

#define cc         *i
#define NULLCHAR    (char)NULL
#define pc          lastChar

string soundex(string, int);
string refinedSoundex(string, int);

//' @rdname soundex
//' @name soundex
//' @title Soundex
//'
//' @description
//' The Soundex phonetic algorithms
//'
//' @param word string or vector of strings to encode
//' @param maxCodeLen  maximum length of the resulting encodings, in characters
//'
//' @details The function \code{soundex} phonentically encodes the given
//' string using the soundex algorithm.  The function \code{refinedSoundex}
//' uses Apache's refined soundex algorithm.  Both implementations are loosely
//' based on the Apache Commons Java editons.
//'
//' The variable \code{maxCodeLen} is the limit on how long the returned
//' soundex should be.
//'
//' @return soundex encoded character vector
//'
//' @references
//' Charles P. Bourne and Donald F. Ford, "A study of methods for
//' systematically abbreviating English words and names," \emph{Journal
//' of the ACM}, vol. 8, no. 4 (1961), p. 538-552.
//'
//' Howard B. Newcombe, James M. Kennedy, "Record linkage: making
//' maximum use of the discriminating power of identifying information,"
//' \emph{Communications of the ACM}, vol. 5, no. 11 (1962), p. 563-566.
//'
//' @family phonics
//'
//' @examples
//' soundex("wheel")
//' soundex(c("school", "benji"))
//'
//' @useDynLib phonics
//' @importFrom Rcpp evalCpp
//' @export
//[[Rcpp::export]]
CharacterVector soundex(CharacterVector word, int maxCodeLen = 4) {
    CharacterVector res;

    for(CharacterVector::iterator i = word.begin(); i != word.end(); i++)
        res.push_back(soundex((string)*i, maxCodeLen));

    return(res);
}

//' @rdname soundex
//' @useDynLib phonics
//' @importFrom Rcpp evalCpp
//' @export
//[[Rcpp::export]]
CharacterVector refinedSoundex(CharacterVector word, int maxCodeLen = 10) {
    CharacterVector res;

    for(CharacterVector::iterator i = word.begin(); i != word.end(); i++)
        res.push_back(refinedSoundex((string)*i, maxCodeLen));

    return(res);
}

string soundex(string x, int maxCodeLen) {
    const string SOUNDEX = "01230120022455012623010202";
    string::iterator i;
    string code = "";
    char lastCode = NULLCHAR;

    trim(x);
    to_upper(x);

    string word = x.substr();

    if(x.length() == 0)
        return("");
    if(x.length() == 1)
        return(x);

    for(i = x.begin(); i != x.end() && !isalpha(*i); i++);
    if(i == x.end())
        return "";

    code = *i;
    lastCode = SOUNDEX.at(*i - 'A');

    for(i++; i != x.end(); ++i) {
        char nextCode = SOUNDEX.at(*i - 'A');

        if(nextCode != '0' && nextCode != lastCode)
            code += (lastCode = nextCode);
        if(nextCode ==  '0' && *i != 'H' && *i != 'W')
            lastCode = '?';
    }

    //  "0"-pad string then truncate
    code += "0000";
    code = code.substr(0, maxCodeLen);

    return code;
}

string refinedSoundex(string x, int maxCodeLen) {
    const string SOUNDEX = "01360240043788015936020505";
    string::iterator i;
    string code = "";
    char lastCode = NULLCHAR;

    trim(x);
    to_upper(x);

    string word = x.substr();

    if(x.length() == 0)
        return("");
    if(x.length() == 1)
        return(x);

    for(i = x.begin(); i != x.end() && !isalpha(*i); i++);
    if(i == x.end())
        return "";

    code = *i;
    code += (lastCode = SOUNDEX.at(*i - 'A'));

    for(i++; i != x.end(); ++i) {
        char nextCode = SOUNDEX.at(*i - 'A');
        if(nextCode != lastCode)
            code += (lastCode = nextCode);
    }

    // Do not "0"-pad for refined
    code = code.substr(0, maxCodeLen);

    return code;
}
