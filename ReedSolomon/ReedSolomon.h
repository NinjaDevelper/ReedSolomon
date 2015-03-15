/*
   Copyright (c) 2015, utamaro
   All rights reserved.
   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
   3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef REEDSOLOMON_H_INCLUDED
#define REEDSOLOMON_H_INCLUDED

#include <vector>
#include <string>

using namespace std;

/**
 *  struct representing polynomial.
 */
typedef struct {
/**
 *  data bytes.
 */
    unsigned char *data;
/**
 *  number of polynomial power.
 */    int n;
} POLY;

/**
 *  Class for Reed-Solomon coding including interleaving.
 *  
 *  @note http://en.wikiversity.org/wiki/Reed%E2%80%93Solomon_codes_for_coders
 */
class ReedSolomon {
    private:
        unsigned char expTable[512];
        unsigned char logTable[256];


        /**
         * Multiply polynomial by polynomial.
         * Result will be stored in x.
         * 
         * @param x multiplication factor, also where result will be stored.
         * @param y multiplication factor
         * @return pointer reference of  variable x
         */
        POLY& multiplyPoly(POLY& x,POLY& y,unsigned char* tmp);

        /**
         * Evaluate polynomial by specified value.
         * 
         * @param x polynomial that will be evaluated.
         * @param y value that is substituteed.
         * @return evaluated value
         */
        unsigned char evaluatePoly(POLY& x,unsigned char y);

    public:
        /**
         * Constructor, where initialize tables that store respresenting values of a^x and log(a^x).
         * 
         */
        ReedSolomon();

        /**
         * Calculate syndrome.
         * 
         * @param msg polynomial that represnts message you received.
         * @param result syndrome that is calculated.
         * @return pointer reference of  variable result
         */
        POLY& calcSyndrome(POLY& msg, POLY& result);

        /**
         * Calculate generator polynomical. result.
         * n must be a number of parity bits+1 (=power of generator).
         * 
         * @param result syndrome that is calculated.
         * @return pointer reference of  variable result
         */
        POLY& calcGenerator(POLY& result);

        /**
         * Encoding by Reed-Solomon codes.
         * 
         * @param msgIn polynomial that represnts message to be coded.Parity bits of msgIn must be 0.
         *        msgIn.data[0:msgIn.n] will be destroyed.       
         * @param generator result 
         * @return pointer reference of generator.
         */
        unsigned char* encodeMessage(POLY& msgIn,POLY& generator);


        /**
         * Encoding by Reed-Solomon codes.
         * 
         * @param msg polynomial that represnts message to be coded.Parity bits of msgIn must be 0.
         *        msg.data[0:msgIn.n] will be destroyed.       
         * @param num number of parity bits.
         * @param pos error location in bytes in file.
         * @return pointer reference of msgIn.
         */
        POLY& correct(POLY& msg, int num,std::vector<int>& pos);


        /**
         * Encode file by Reed-Solomon codes with interleaving.
         * 
         * For interleaving, data will be considered as K x (interleaving length) matrix
         * (interleaving length= (file size % K)+1).
         * Then Reed Solomon coding is adapted to each column, and add parity bits to the end of 
         * column.
         * 
         * For example, RS(K=200,N=208) is used to 1,000,000 bytes data, 
         * which means interleaving length=5,000
         * so you can recover data even if you cannot download sequential 5,000*8 bytes.
         * 
         * @param origFile file name that you wan to encode.
         * @param encodedFile file name that you wan to output.
         * @param K  amount of original data.
         * @param N  total number of symbols after the erasure coding
         * @return 0 is success. others for error.
         */
        int encodeFile(string origFile,string encodedFile,int K,int N);

        /**
         * Decode file that coded by Reed-Solomon codes.
         * 
         * @param inFile file name that you wan to decode.
         * @param outFile file name that you wan to output.
         * @param K  amount of original data.
         * @param N  total number of symbols after the erasure coding
         * @param size file size of original file (not encoded file)
         * @param errLoc error location in bytes in file.
         * @return 0 is success. others for error.
         */
        int decodeFile(string inFile,string outFile,int K,int N,long size,vector<long>&errLoc);
};

#endif
