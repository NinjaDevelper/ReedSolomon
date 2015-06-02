/*
 * Copyright (c) 2015, Shinya Yagyu
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its 
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <map>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "ReedSolomon.hpp"

using namespace std;

#define multiply(x,y) (((x)==0 || (y)==0)? 0:expTable[logTable[(x)]+logTable[(y)]])
#define divide(x,y) ((y)==0 ? -1:( (x)==0? 0:expTable[logTable[(x)]+255-logTable[(y)]]))

ReedSolomon::ReedSolomon(){
    int i=0;
    unsigned int x=1;
    expTable[0]=1;
    for(i=1;i<255;i++){
        x<<=1;
        if(x>0xff) x^=0x11d;
        unsigned char c=x&0xff;
        expTable[i]=c;
        logTable[c]=i;
    }
    for(i=255;i<512;i++){
        expTable[i]=expTable[i-255];
    }
    logTable[expTable[255]]=255;
}


POLY& ReedSolomon::multiplyPoly(POLY& x,POLY& y,unsigned char* tmp){
    int i=0,j=0;
    int n=x.n+y.n-1;

    for(j=0;j<n;j++) tmp[j]=0;
    for(i=0;i<x.n;i++){
        for(j=0;j<y.n;j++){
            tmp[i+j]^=multiply(x.data[i],y.data[j]);
        }
    }
    for(j=0;j<n;j++) x.data[j]=tmp[j];
    x.n+=y.n-1;
    return x;
}

unsigned char ReedSolomon::evaluatePoly(POLY& x,unsigned char y){
    int i=0;
    unsigned char r=x.data[0];
    for(i=1;i<x.n;i++){
        r=multiply(r,y) ^ x.data[i];
    }
    return r;
}

POLY& ReedSolomon::calcGenerator(POLY& result){
    int i=0;
    int num=result.n-1;

    unsigned char *tmp=(unsigned char *)malloc(num+1);

    result.data[0]=1;
    result.n=1;
    unsigned char c[2]={0x1,0};
    POLY p={c,2};
    for(i=0;i<num;i++){
        c[1]=expTable[i];
        multiplyPoly(result,p,tmp);
    }
    free(tmp);
    return result;
}


unsigned char* ReedSolomon::encodeMessage(POLY& msgIn,POLY& generator){
    int i=0;
    int j=0;
    unsigned char coef=0;

    for(i=0;i<msgIn.n;i++){
        coef=msgIn.data[i];
        if(coef!=0){
            for(j=0;j<generator.n;j++){
                msgIn.data[i+j] ^= multiply(generator.data[j], coef);
            }
        }
    }
    return msgIn.data+msgIn.n;
}

POLY& ReedSolomon::calcSyndrome(POLY& msg, POLY& result){
    int i=0;
    for(i=0;i<result.n;i++){
        result.data[i] = evaluatePoly(msg, expTable[i]);
    }
    return result;
}


POLY& ReedSolomon::correct(POLY& msg, int num,vector<int>& pos){
    int i=0;
    int j=0;
    int errNum=pos.size();
    unsigned char *tmp=(unsigned char *)malloc(errNum<<1);

    //calculate syndrome
    unsigned char *synd=(unsigned char *)malloc(num);
    for(i=0;i<num;i++){
        synd[i] = evaluatePoly(msg, expTable[i]);
    }

    //calculate locator
    POLY locator={(unsigned char *)malloc(errNum+1),1};
    locator.data[0]=1;
    unsigned char c[2]={0,1};
    POLY p={c,2};
    for(i=0;i<errNum;i++){
        c[0] = expTable[msg.n-1-pos[i]];
        multiplyPoly(locator,p,tmp);
    }

    //calculate evaluator
    POLY evaluator={(unsigned char *)malloc(errNum<<1),errNum};
    for(i=0;i<errNum;i++){
        evaluator.data[i]=synd[errNum-i-1];
    }
    multiplyPoly(evaluator, locator,tmp);
    for(i=0;i<errNum;i++){
        evaluator.data[i]=evaluator.data[evaluator.n-errNum+i];
    }
    evaluator.n=errNum;

    //calculate derivation of locator
    for(j=0,i=((locator.n & 0x1) ==0? 0:1);i<locator.n;i+=2,j++){
        locator.data[j]=locator.data[i];
    }
    locator.n=((locator.n & 0x1) ==0? locator.n>>1:(locator.n-1)>>1);

    /* calculate X_j*evaluator(X_j-^1)/locator'(X_j^-1)=
     * evaluator(X_j-^1)/( X_j^-1 * locator'(X_j^-1))
     */
    for(i=0;i<errNum;i++){
        unsigned char minus_i = expTable[pos[i]+256-msg.n];
        unsigned char evaluator_minus_i = evaluatePoly(evaluator, minus_i);
        unsigned char locator_minus_i = evaluatePoly(locator, 
                                multiply(evaluator_minus_i,evaluator_minus_i));
        msg.data[pos[i]] ^= divide(evaluator_minus_i, 
                                   multiply(minus_i,locator_minus_i));
    }
    free(tmp);
    free(synd);
    free(locator.data);
    free(evaluator.data);
    return msg;
}


#define BUFSIZE 1024

long getSize(int fin){
    long size=lseek(fin,0,SEEK_END);
    lseek(fin,0,SEEK_SET);
    return size;
}


int ReedSolomon::decodeFile(string inFile,string outFile,int K,int N,
                            long size,vector<long>&errLoc){
    int i=0;
    int k=0;

    int fin=open(inFile.c_str(),O_RDWR);
    if(fin<0){
        return 1;
    }
    long inSize=getSize(fin);

    //use MAP_PRIVATE not to copy mem to file.
    unsigned char *buf = 
        (unsigned char*)mmap(NULL, inSize, PROT_READ|PROT_WRITE, 
                               MAP_PRIVATE, fin, 0);

    int interleave=ceil((double)size/K);
    map<int, vector<int> > errors;

    //translate location in file to column/row number of 
    //K x interleave length matrix.
    for(unsigned int i=0;i<errLoc.size();i++){
        long loc=errLoc.at(i);
        errors[loc%interleave].push_back(loc/interleave);
    }
    //it error does not exist in data area but in parity area, ignore it.
    for(i=0;i<interleave;i++){
        bool existData=false;
        for(unsigned int k=0;k<errors[i].size();k++){
            if(errors[i].at(k)<K) existData=true;
        }
        if(!existData) errors[i].clear();
    }
    POLY msgIn={(unsigned char*)malloc(N),N}; 

    //collect data in row , and execute correction.
    unsigned char c=0;
    long loc=0;
    int e=0;
    for(k=0;k<interleave;k++){
        if(errors[k].size()==0) continue;
        for(i=0;i<K;i++){
            loc=(long)i*interleave+k;
            if(loc<size){
                c=buf[loc];
            }else{
                c=0;
            }
            msgIn.data[i]=c;
        }
        for(i=0;i<N-K;i++){
            loc=size+(long)i*interleave+k;
            c=buf[loc];
            msgIn.data[K+i]=c;
        }
        correct(msgIn,N-K,errors[k]);
        for(unsigned int i=0;i<errors[k].size();i++){
            e=errors[k].at(i);
            if(e<K)
                buf[e*interleave+k]=msgIn.data[e];
        }
    }

    //copy only data area to destination file.
    FILE *fp=fopen(outFile.c_str(),"wb");
    fwrite(buf,1,size,fp);
    fclose(fp);

    close(fin);
    munmap(buf,inSize);
    free(msgIn.data);

    return 0;
}

int ReedSolomon::encodeFile(string origFile,string encodedFile,int K,int N){
    int i=0;
    int k=0;

    int fin=open(origFile.c_str(),O_RDONLY);
    if(fin<0){
        return 1;
    }
    int fout=open(encodedFile.c_str(),O_RDWR|O_CREAT, S_IRUSR | S_IWUSR);
    long size=getSize(fin);

    int interleave=ceil((double)size/K);
    long writeSize=size+interleave*(N-K);

    //write a last byte to ensure the output file size 
    lseek(fout,writeSize-1,SEEK_SET);
    write(fout,"",1);
    lseek(fout,0,SEEK_SET);

    unsigned char *inBuf =
        (unsigned char*) mmap(NULL, size, PROT_READ, MAP_SHARED, fin, 0);
    unsigned char *outBuf = 
        (unsigned char*)mmap(NULL, writeSize, PROT_WRITE, MAP_SHARED,
                               fout, 0);

    POLY generator={(unsigned char*)malloc(N-K+1),N-K+1};
    calcGenerator(generator);
    POLY msgIn={(unsigned char*)calloc(N,1),K}; 

    //collect data in row , encode, and write parities.
    unsigned char c=0;
    long loc=0;
    unsigned char *parity=NULL;
    for(k=0;k<interleave;k++){
        for(i=0;i<K;i++){
            loc=(long)i*interleave+k;
            if(loc<size){
                c=inBuf[loc];
                outBuf[loc]=c;
            }else{
                c=0;
            }
            msgIn.data[i]=c;
        }
        parity=encodeMessage(msgIn,generator);
        for(i=0;i<N-K;i++){
            loc=size+(long)i*interleave+k;
            outBuf[loc]=parity[i];
            parity[i]=0;
        }
    }
    munmap(inBuf,size);
    munmap(outBuf,writeSize);
    close(fin);
    close(fout);
    free(generator.data);
    free(msgIn.data);
    return 0;
}

