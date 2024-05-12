// Copyright (c) 2015-2019, James P. Howard, II <jh@jameshoward.us>
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

std::string soundex_single(std::string x, int maxCodeLen) {
    const std::string SOUNDEX = "01230120022455012623010202";
    std::string::iterator i;
    std::string code = "";
    char lastCode = (char)NULL;

    boost::trim(x);
    boost::to_upper(x);

    for(i = x.begin(); i != x.end() && !isalpha(*i); ++i);
    if(i == x.end())
        return "";
    if(x.length() == 1) {
        x += "0000";
        x = x.substr(0, maxCodeLen);
        return(x);
    }

    code = *i;
    lastCode = SOUNDEX.at(*i - 'A');

    for(++i; i != x.end(); ++i) {
        char currCode = *i - 'A';
        if(currCode < 0 || currCode > 25)
            break;

        char nextCode = SOUNDEX.at(currCode);
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

std::string refinedSoundex_single(std::string x, int maxCodeLen) {
    const std::string SOUNDEX = "01360240043788015936020505";
    std::string::iterator i;
    std::string code = "";
    char lastCode = (char)NULL;

    boost::trim(x);
    boost::to_upper(x);

    for(i = x.begin(); i != x.end() && !isalpha(*i); ++i);
    if(i == x.end())
        return "";
    if(x.length() == 1)
        return(x);

    code = *i;
    code += (lastCode = SOUNDEX.at(*i - 'A'));

    for(++i; i != x.end(); ++i) {
        char currCode = *i - 'A';
        if(currCode < 0 || currCode > 25)
            break;

        char nextCode = SOUNDEX.at(currCode);
        if(nextCode != lastCode)
            code += (lastCode = nextCode);
    }

    // Do not "0"-pad for refined
    code = code.substr(0, maxCodeLen);

    return code;
}

//' @useDynLib phonics
//' @importFrom Rcpp evalCpp
//[[Rcpp::export]]
Rcpp::CharacterVector soundex_internal(Rcpp::CharacterVector word, int maxCodeLen = 4) {
    unsigned int input_size = word.size();
    Rcpp::CharacterVector res(input_size);

    for(unsigned int i = 0; i < input_size; i++){
        if((i % 10000) == 0){
            Rcpp::checkUserInterrupt();
        }
        if(word[i] == NA_STRING){
            res[i] = NA_STRING;
        } else {
            res[i] = soundex_single(Rcpp::as<std::string>(word[i]), maxCodeLen);
        }
    }

    return res;
}

//' @useDynLib phonics
//' @importFrom Rcpp evalCpp
//[[Rcpp::export]]
Rcpp::CharacterVector refinedSoundex_internal(Rcpp::CharacterVector word, int maxCodeLen = 10) {
    unsigned int input_size = word.size();
    Rcpp::CharacterVector res(input_size);

    for(unsigned int i = 0; i < input_size; i++){
        if((i % 10000) == 0){
            Rcpp::checkUserInterrupt();
        }
        if(word[i] == NA_STRING){
            res[i] = NA_STRING;
        } else {
            res[i] = refinedSoundex_single(Rcpp::as<std::string>(word[i]), maxCodeLen);
        }
    }

    return res;
}
