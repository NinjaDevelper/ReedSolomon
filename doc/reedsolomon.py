#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2015, Shinya Yagyu
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


#dummy for generating documents.

def encode_file(orig_file,encoded_file,k,n):
    """
    Encode file by Reed-Solomon codes with interleaving.

    For interleaving, data will be considered as K x (interleaving length) matrix
    (interleaving length= (file size % K)+1).
    Then Reed Solomon coding is adapted to each column, and add parity bits to 
    the end of column.
     
    For example, RS(K=200,N=208) is used to 1,000,000 bytes data, 
    which means interleaving length=5,000
    so you can recover data even if you cannot download sequential 5,000*8 bytes.
    
    :param str orig_file: file name that you wan to encode.
    :param str encoded_file: file name that you wan to output.
    :param int k:  amount of original data.
    :param int n:  total number of symbols after the erasure coding
    :return: 0 is success. others for error.
    """
    pass


def decode_file(in_file,out_file,k,n,size,err_loc):
    """
    Decode file that coded by Reed-Solomon codes.
    
    :param str in_file: file name that you wan to decode.
    :param str out_file: file name that you wan to output.
    :param int k:  amount of original data.
    :param int n:  total number of symbols after the erasure coding
    :param long size: file size of original file (not encoded file)
    :param list err_loc: error location in bytes in file.
    :return: 0 is success. others for error.
    """
    pass


