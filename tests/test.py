#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2015, utamaro
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os
import filecmp
import unittest
import reedSolomon

class TestReedSolomon(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_dencode(self):
        r=reedSolomon.encodeFile('data/rand32m.dat','encoded.dat',200,208)

        #embed sequential error
        FROM=500000
        COUNT=300
        buf=bytearray(1024)
        with open('encoded.dat','r+b') as fin:
            fin.seek(FROM)
            for i in range(0,COUNT):
                fin.write(buf)
        loc=[]
        for i in range(0,COUNT*1024):
            loc.append(i+FROM)

        reedSolomon.decodeFile('encoded.dat','recovered.dat',200,208,os.path.getsize('data/rand32m.dat'),loc)

        self.assertTrue(filecmp.cmp('data/rand32m.dat','recovered.dat'),'erasure recovery test after coding')

if __name__ == '__main__':
    unittest.main()
