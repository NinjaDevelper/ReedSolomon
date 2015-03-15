#dummy for generating documents.

def encodeFile(origFile,encodedFile,K,N):
    """
    Encode file by Reed-Solomon codes with interleaving.

    For interleaving, data will be considered as K x (interleaving length) matrix
    (interleaving length= (file size % K)+1).
    Then Reed Solomon coding is adapted to each column, and add parity bits to the end of 
    column.
     
    For example, RS(K=200,N=208) is used to 1,000,000 bytes data, 
    which means interleaving length=5,000
    so you can recover data even if you cannot download sequential 5,000*8 bytes.
    
    :param str origFile: file name that you wan to encode.
    :param str encodedFile: file name that you wan to output.
    :param int K:  amount of original data.
    :param int N:  total number of symbols after the erasure coding
    :return 0: is success. others for error.
    """
    pass


def decodeFile(inFile,outFile,K,N,size,errLoc):
    """
    Decode file that coded by Reed-Solomon codes.
    
    :param str inFile: file name that you wan to decode.
    :param str outFile: file name that you wan to output.
    :param int K:  amount of original data.
    :param int N:  total number of symbols after the erasure coding
    :param long size: file size of original file (not encoded file)
    :param list errLoc: error location in bytes in file.
    :return 0: is success. others for error.
    """
    pass


