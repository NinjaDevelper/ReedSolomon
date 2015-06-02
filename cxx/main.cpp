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
#include <unistd.h>
#include <fcntl.h>
#include "ReedSolomon.hpp"

#define FROM 500000
#define COUNT 300


int main(int argc,char *argv[]){
    ReedSolomon rs;
 
    int fin=open(argv[2],O_RDONLY);
    long size=lseek(fin,0,SEEK_END);
    lseek(fin,0,SEEK_SET);
    close(fin);

    if(argv[1][0]=='e'){
        rs.encodeFile(argv[2],"encoded.dat",200,208);
    }
    if(argv[1][0]=='m'){
        if(size>500000+1024*300){
            //embed sequential error
	    fin=open("encoded.dat",O_RDWR);
	    lseek(fin,FROM,SEEK_END);
	    char buf[1024]={};
	    for(int i=0;i<COUNT;i++){
	        write(fin,buf,1024);
	    }
	    close(fin);
        }
    }
    if(argv[1][0]=='d'){
	vector<long> loc;
	for(int i=0;i<COUNT*1024;i++){
	    loc.push_back(i+500000);
    	}
        rs.decodeFile("encoded.dat","recovered.dat",200,208,size,loc);
    }
    return 0;
}
