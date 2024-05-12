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

//' @useDynLib phonics
//' @importFrom Rcpp evalCpp
// [[Rcpp::depends(BH)]]
#include <Rcpp.h>
#include <boost/algorithm/string.hpp>

#define cc         *i
#define nc         *(i + 1)
#define nnc        *(i + 2)
#define pc          lastChar
#define NULLCHAR    (char)NULL

bool is(std::string x, char c) {
    return (c != NULLCHAR && x.find_first_of(c) != std::string::npos);
}

char at(std::string x, int i) {

    try {
        return x.at(i);
    } catch(const std::out_of_range& e) {
        return NULLCHAR;
    }
}

std::string substr(std::string x, int i, int n) {

    try {
        return x.substr(i, n);
    } catch(const std::out_of_range& e) {
        return "";
    }
}

std::string metaphone_single(std::string x, int maxCodeLen, bool traditional) {
    std::string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string soft = "EIY";
    std::string vowels = "AEIOU";

    std::string::iterator i;
    std::string word = x, meta = "";
    char lastChar = NULLCHAR;

    boost::trim(word);
    boost::to_upper(word);

    /*
     * First, we will handle a few special cases.  The Metaphone of the
     * null string is, itself, the null string.  The Metaphone of a
     * single character is itself, capitalized, as appropriate.
     */
    for(i = word.begin(); i != word.end() && !isalpha(*i); ++i);
    if(i == word.end())
        return "";
    if(word.length() == 1)
        return(word);

    switch (cc) {
    case 'A':
        meta += nc == 'E' ? nc : cc;
        i += 1;
        break;
    case 'G':
    case 'K':
    case 'P':
        if (nc == 'N') {
            meta += nc;
            i += 2;
        }
        break;
    case 'W':
        if (nc == 'R') {
            meta += nc;
            i += 2;
        } else if (nc == 'H' || is(vowels, nc)) {
            meta += 'W';
            i += 2;
        }
        break;
    case 'X':
        meta += 'S';
        i += 1;
        break;
    case 'E':
    case 'I':
    case 'O':
    case 'U':
        meta += cc;
        ++i;
        break;
    }

    while(meta.length() < maxCodeLen && i != word.end()) {
        if(cc != 'C' && pc == cc)
            i++;
        else {
            switch(cc) {
            case 'B':
                if (pc != 'M')
                    meta += cc;
                break;
            case 'C':
                if (is(soft, nc)) {
                    if (nc == 'I' && nnc == 'A') {
                        meta += 'X';
                    } else if (pc != 'S') {
                        meta += 'S';
                    }
                } else if (nc == 'H') {
                    meta += !traditional && (nnc == 'R' || pc == 'S') ? 'K' : 'X';
                    i++;
                } else {
                    meta += 'K';
                }
                break;
            case 'D':
                if (nc == 'G' && is(soft, nnc)) {
                    meta += 'J';
                    i++;
                } else {
                    meta += 'T';
                }
                break;
            case 'G':
                if (nc == 'H') {
                    if(!(is("BDH", at(word, std::distance(word.begin(), i) - 3)) ||
                         at(word, std::distance(word.begin(), i) - 4) == 'H')) {
                        meta += 'F';
                        i++;
                    }
                } else if(nc == 'N') {
                    if (is(alpha, nnc) && substr(word, std::distance(word.begin(), i) + 1, 3) != "NED") {
                        meta += 'K';
                    }
                } else if(is(soft, nc) && pc != 'G') {
                    meta += 'J';
                } else {
                    meta += 'K';
                }
                break;
            case 'H':
                if(is(vowels, nc) && !is("CGPST", pc))
                    meta += cc;
                break;
            case 'K':
                if (pc != 'C') {
                    meta += 'K';
                }
                break;
            case 'P':
                meta += nc == 'H' ? 'F' : cc;
                break;
            case 'Q':
                meta += 'K';
                break;
            case 'S':
                if(nc == 'I' && is("AO", nnc)) {
                    meta += 'X';
                } else if(nc == 'H') {
                    meta += 'X';
                    i += 1;
                } else if(!traditional && substr(word, std::distance(word.begin(), i) + 1, 3) == "CHW") {
                    meta += 'X';
                    i += 2;
                } else {
                    meta += 'S';
                }
                break;
            case 'T':
                if(nc == 'I' && is("AO", nnc)) {
                    meta += 'X';
                } else if(nc == 'H') {
                    meta += '0';
                    i += 1;
                } else if(substr(word, std::distance(word.begin(), i) + 1, 2) != "CH") {
                    meta += 'T';
                }
                break;
            case 'V':
                meta += 'F';
                break;
            case 'W':
            case 'Y':
                if(is(vowels, nc))
                    meta += cc;
                break;
            case 'X':
                meta += "KS";
                break;
            case 'Z':
                meta += 'S';
                break;
            case 'F':
            case 'J':
            case 'L':
            case 'M':
            case 'N':
            case 'R':
                meta += cc;
                break;
            default:
                break;
            }
            pc = cc;
            i++;
        }
    }
    return meta;
}

//[[Rcpp::export]]
Rcpp::CharacterVector metaphone_internal(Rcpp::CharacterVector word, int maxCodeLen = 10) {

    unsigned int input_size = word.size();
    Rcpp::CharacterVector res(input_size);

    for(unsigned int i = 0; i < input_size; i++){
        if((i % 10000) == 0){
            Rcpp::checkUserInterrupt();
        }
        if(word[i] == NA_STRING){
            res[i] = NA_STRING;
        } else {
            res[i] = metaphone_single(Rcpp::as<std::string>(word[i]), maxCodeLen, true);
        }
    }

    return res;
}
