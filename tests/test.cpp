/*
   Copyright (c) 2015, utamaro
   All rights reserved.
   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
   3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <tap.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>

#include "ReedSolomon.h"

#define BUFFER_SIZE 65535
int compareChunk(string fname1,string fname2){
    int i=0;
    FILE *fp1=NULL,*fp2=NULL;
    fp1=fopen(fname1.c_str(),"rb");
    if(fp1==NULL){
	printf("file %s not found\n",fname1.c_str());
        return 1;
    }
    fp2=fopen(fname2.c_str(),"rb");
    if(fp2==NULL){
        fclose(fp1);
	printf("file %s not found\n",fname2.c_str());
        return 1;
    }
    int bytes_read1=0;
    int bytes_read2=0;
    unsigned char buffer1[BUFFER_SIZE];
    unsigned char buffer2[BUFFER_SIZE];
    int isOK=1;
    while(isOK && (bytes_read1 = fread(buffer1, sizeof(unsigned char), BUFFER_SIZE, fp1)) !=0){
        bytes_read2 = fread(buffer2, sizeof(unsigned char), BUFFER_SIZE, fp2);
        if(bytes_read1!=bytes_read2){
	    printf("size differ\n");
	    return 1;
         }
        for(i=0;i<bytes_read1;i++){
            if(buffer1[i]!=buffer2[i]){
		printf("data differ\n");
		return 1;
            }
        }
    }
    fclose(fp1);
    fclose(fp2);
    return 0;
}

int checkSyndromes(string  encodedFile,int K,int N,long size){
    int i=0;
    int k=0;
    ReedSolomon rs;

    int fin=open(encodedFile.c_str(),O_RDONLY);
    if(fin==0){
        return 1;
    }
    int interleave=ceil((double)size/K);

    long size_=lseek(fin,0,SEEK_END);
    lseek(fin,0,SEEK_SET);

    unsigned char *inBuf =
        (unsigned char*) mmap(NULL, size_, PROT_READ, MAP_SHARED, fin, 0);

    POLY msgIn={(unsigned char*)malloc(N),N}; 
    POLY syndrome={(unsigned char*)malloc(N-K),N-K};

    for(k=0;k<interleave;k++){
        for(i=0;i<K;i++){
            unsigned char c=0;
            long loc=(long)i*interleave+k;
            if(loc<size){
                c=inBuf[loc];
            }
            msgIn.data[i]=c;
        }
        for(i=0;i<N-K;i++){
            long loc=size+(long)i*interleave+k;
            unsigned char c=inBuf[loc];
            msgIn.data[K+i]=c;
        }
        for(i=0;i<N-K;i++) syndrome.data[i]=0;
        rs.calcSyndrome(msgIn,syndrome);
        for(i=0;i<N-K;i++){
            if(syndrome.data[i]!=0){
                printf("syndrom %d %d is not 0\n",k,i);
                return 1;
            }
        }
    }
    munmap(inBuf,size);
    close(fin);
    free(msgIn.data);
    free(syndrome.data);
    return 0;
}

int main () {
    ReedSolomon rs;
    int i=0;
    unsigned char c[26]={0x40, 0xd2, 0x75, 0x47, 0x76, 0x17, 0x32, 0x06,
        0x27, 0x26, 0x96, 0xc6, 0xc6, 0x96, 0x70, 0xec};
    POLY msgIn={c,16};
 
    unsigned char gen[11];
    POLY generator={gen,11};
    rs.calcGenerator(generator);
    unsigned char expected1[11]={0x1,0xd8,0xc2,0x9f,0x6f,0xc7,0x5e,0x5f,0x71,0x9d,0xc1};
    cmp_mem(gen,expected1,11,"generator polynomial test");

    unsigned char *parity =rs.encodeMessage(msgIn,generator);
    unsigned char expected2[10]={0xbc,0x2a,0x90,0x13,0x6b,0xaf,0xef,0xfd,0x4b,0xe0};
    cmp_mem(parity,expected2,10,"parity bits test");

    unsigned char c_[16]={0x40, 0xd2, 0x75, 0x47, 0x76, 0x17, 0x32, 0x06,
        0x27, 0x26, 0x96, 0xc6, 0xc6, 0x96, 0x70, 0xec};
    for(i=0;i<16;i++) c[i]=c_[i];
    msgIn.n=26;
    unsigned char synd[10];
    POLY syndrome={synd,10};
    rs.calcSyndrome(msgIn,syndrome);
    unsigned char expected3[10]={};
    cmp_mem(synd,expected3,10,"syndrome of no error message");

    c[0]=0;
    rs.calcSyndrome(msgIn,syndrome);
    unsigned char expected4[10]={64, 192, 93, 231, 52, 92, 228, 49, 83, 245};
    cmp_mem(synd,expected4,10,"syndrome of error message");

    vector<int> errs;
    errs.push_back(0);
    errs.push_back(8);
    rs.correct(msgIn,8,errs);
    unsigned char expected5[26]={0x40, 0xd2, 0x75, 0x47, 0x76, 0x17, 0x32, 0x06,
        0x27, 0x26, 0x96, 0xc6, 0xc6, 0x96, 0x70, 0xec,
        0xbc,0x2a,0x90,0x13,0x6b,0xaf,0xef,0xfd,0x4b,0xe0};
    cmp_mem(msgIn.data,expected5,26,"corrected message");

    int fin=open("data/rand32m.dat",O_RDONLY);
    long size=lseek(fin,0,SEEK_END);
    lseek(fin,0,SEEK_SET);
    close(fin);

    int r2=rs.encodeFile("notexist.dat","encoded.dat",200,208);
    ok(r2==1,"encode test to non-exist file");

    rs.encodeFile("data/rand32m.dat","encoded.dat",200,208);
    ok(checkSyndromes("encoded.dat",200,208,size)==0,"syndromes test of a coded file");

    #define FROM 500000
    #define COUNT 300
    //embed sequential error
    fin=open("encoded.dat",O_RDWR);
    lseek(fin,FROM,SEEK_SET);
    char buf[1024]={};
    for(int i=0;i<COUNT;i++){
        write(fin,buf,1024);
    }
    close(fin);
    vector<long> loc;
    for(int i=0;i<COUNT*1024;i++){
        loc.push_back(i+500000);
    }

    int r=rs.decodeFile("notexist.dat","recovered.dat",200,208,size,loc);
    ok(r==1,"erasure test to non-exist file");

    rs.decodeFile("encoded.dat","recovered.dat",200,208,size,loc);
    ok(compareChunk("data/rand32m.dat","recovered.dat")==0,"erasure test");
    
    done_testing();
}
