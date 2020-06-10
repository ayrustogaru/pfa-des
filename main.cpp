#include <bits/stdc++.h>
using namespace std;

#include "des.h"


static int fault[8][4][16] = {
          // S1
          { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
          0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
          4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
          15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },
          // S2
          { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
          3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
          0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
          13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },
          // S3
          { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
          13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
          13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
          1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },
          // S4
          { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
          13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
          10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
          3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },
          // S5
          { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
          14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
          4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
          11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },
          // S6
          { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
          10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
          9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
          4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },
          // S7
          { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
          13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
          1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
          6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },
          // S8 -- changes 0,0 from 13 to 0
          { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
          1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
          7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
          2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } };

/*
Function used to extract "k bits" of a "number" from "position p".
Note: p = 0 corresponds to the LSB.
*/
int bitExtracted(uint32_t number, int k, int p)
{
    return (((1 << k) - 1) & (number >> p));
}

/*
Checks if the left parts of the correct and wrong differs at only one word.
Done by using xor operation and comparing the words in the resulting string
with the corresponding S-box entry.
*/
bool checkProperty(uint32_t a, int sbox)
{
  // cout<<a<<endl;
  for(int j = 0; j<32; j+=4)
  {
    uint32_t aj = bitExtracted(a,4,28-j);
    if(j!=4*sbox)
    {
       //change 4 to WORD_SIZE
      if(aj!=0) return false; // -- fill not a correct xor, start again
    }
    else{
      //temp must be equal to correct sbox entry
      if(aj != s[sbox][0][0]) return false;
    }
  }
  return true;
}

/*
This function recovers the 56-bit key used in DES using two 48-bit round keys.
The round keys used here are of Round 16 and 15 respectively.
*/
string reverseKeySchedule(string key1, string key2)
{
	string key1Expanded = permute(key1, key_exp, 56);
	string key2Expanded = permute(key2, key_exp, 56);

  //Splitting the keys
	string left1 = key1Expanded.substr(0, 28);
	string right1 = key1Expanded.substr(28, 28);

	string left2 = key2Expanded.substr(0, 28);
	string right2 = key2Expanded.substr(28, 28);

  //Tracing back
	left2 = shift_left(left2,1);
	right2 = shift_left(right2,1);

	string last = left1+right1;
	string  onelasttime = left2+right2;

	return bin2hex(or_(last, onelasttime));
}

/*
This function is used for decrypting the ciphertext ONCE.
So, it is used to decrypt the ciphertext from round 16 and thus,
generate the ciphertext of round 15.
Function parameters:
  @pt - ciphertext that needs to be decrypted.
  @roundKey - the key pertainig to the round that needs to be decrypted.
*/
string decryptOnce(string pt, string roundKey)
{
  // Hexadecimal to binary
  pt = hex2bin(pt);

  // Splitting
  string left = pt.substr(0, 32);
  string right = pt.substr(32, 32);

  // Expansion D-box
  string right_expanded = permute(right, exp_d, 48);

  // XOR RoundKey[i] and right_expanded
  string x = xor_(roundKey, right_expanded);

  // S-boxes
  string op = "";
  for (int i = 0; i < 8; i++) {
    int row = 2 * int(x[i * 6] - '0') + int(x[i * 6 + 5] - '0');
    int col = 8 * int(x[i * 6 + 1] - '0') + 4 * int(x[i * 6 + 2] - '0') + 2 * int(x[i * 6 + 3] - '0') + int(x[i * 6 + 4] - '0');
    int val = s[i][row][col];
    op += char(val / 8 + '0');
    val = val % 8;
    op += char(val / 4 + '0');
    val = val % 4;
    op += char(val / 2 + '0');
    val = val % 2;
    op += char(val + '0');
  }
  // XOR left and op
  x = xor_(op, left);
  left = x;

  //Swapper
  // if (i != 15) {
    swap(left, right);
  // }

  //Invert Straight D-box permutaion
  left = permute(left, invper, 32);

  // Combination
  string combine = left + right;
  string cipher = bin2hex(combine);
  return cipher;
}

/*
Core function that runs the DES algorithm and then attacks it.
Firstly, recovers the round 16 key using PFA.
Then, it uses the recovered key to retrieve the round 15 key.
Using these as inputs to reverseKeySchedule(), it finds the 56-bit key.
Function Parameters:
  @key - The 64 bit key used to run the DES cipher.
*/
vector<string> automateRecovery(string key)
{
  //Store the round 16,15 and the master keys.
  vector<string> recoveredKey(3, "");
  int numTrials = 0;
  cout<<endl<<"Recovering Round 16 key..."<<endl;
  //Round 16 key recovery
  for(int i = 0; i<8; ++i)
  {
    // cout<<"Attacking S-box "<<i<<" and entry is "<<fault[i][0][0]<<endl;
    // Overwrite 0,0 element of S-box
    fault[i][0][0] = 0; // change o,o to FAULT_ROW adn FAULT_COLUMN
    do {
      //Run the faultless and faulty DES encryption.
      string pt = genRandomHexString(16);
      string correct_cipher = des(pt,key,s);
      string wrong_cipher = des(pt,key,fault);

      //Reverse the Final Permutation
      correct_cipher = bin2hex(permute(hex2bin(correct_cipher), initial_perm, 64));
      wrong_cipher = bin2hex(permute(hex2bin(wrong_cipher), initial_perm, 64));

      //Get the right part of the cipher
      string yr = hex2bin(correct_cipher).substr(32,32);
      string yr_ = hex2bin(wrong_cipher).substr(32,32);
      uint64_t right = strtouint64(correct_cipher);
      uint64_t wrong = strtouint64(wrong_cipher);

      //a = xor(yl,yl_) --> xor the left parts
      uint32_t a = (right^wrong)>>32;
      numTrials++;
      //check if the right parts of the cipher are equal and validate the
      //xor of the left parts.
      if(correct_cipher.compare(wrong_cipher) && !(yr.compare(yr_)) && checkProperty(a, i))
      {
        //Expand the right part of the cipher to 48-bits
        string yrExpanded = permute(yr, exp_d, 48);
        //Add the 6 bits (ith word of yrExpanded) to recover the key
        recoveredKey[0] += yrExpanded.substr(6*i,6);
        cout<<"Using S-box "<<i<<":\t"<<"Key bits recovered: "<<
          yrExpanded.substr(6*i,6)<<"\tTrials:"<<numTrials<<endl;
        break;
      }
    } while(true);
    numTrials = 0;
    //Reset the S-box to correct entry
    fault[i][0][0] = s[i][0][0];
  }
  cout<<endl<<"Recovering Round 15 key..."<<endl;
  //Round 15 key recovery
  for(int i = 0; i<8; ++i)
  {
    // cout<<"Attacking S-box "<<i<<" and entry is "<<fault[i][0][0]<<endl;
    // Overwrite 0,0 element of S-box
    fault[i][0][0] = 0; // change o,o to FAULT_ROW and FAULT_COLUMN
    do {
      //Run the faultless and faulty DES encryption.
      string pt = genRandomHexString(16);
      string correct_cipher = des(pt,key,s);
      string wrong_cipher = des(pt,key,fault);

      //Check if the ciphers are different
      if(correct_cipher.compare(wrong_cipher))
      {
        //Reverse the Final Permutation
        correct_cipher = bin2hex(permute(hex2bin(correct_cipher), initial_perm, 64));
        wrong_cipher = bin2hex(permute(hex2bin(wrong_cipher), initial_perm, 64));

        /*Decrypt the ciphertext once using Round 15 key to generate ciphertext
        of Round 14*/
        correct_cipher = decryptOnce(correct_cipher,recoveredKey[0]);
        wrong_cipher = decryptOnce(wrong_cipher, recoveredKey[0]);

        //Get the right part of the cipher
        string yr = hex2bin(correct_cipher).substr(32,32);
        string yr_ = hex2bin(wrong_cipher).substr(32,32);
        uint64_t right = strtouint64(correct_cipher);
        uint64_t wrong = strtouint64(wrong_cipher);

        //a = xor(yl,yl_) --> xor the left parts
        uint32_t a = (right^wrong)>>32;

        numTrials++;
        /*Check if the right parts of the Round 15 cipher are equal and
        validate the xor of the left parts.*/
        if(correct_cipher.compare(wrong_cipher) && !(yr.compare(yr_)) && checkProperty(a, i))
        {
          //Expand the right part of the cipher to 48-bits
          string yrExpanded = permute(yr, exp_d, 48);
          //Add the 6 bits (ith word of yrExpanded) to recover the key
          recoveredKey[1] += yrExpanded.substr(6*i,6);
          cout<<"Using S-box 8:\t"<<"Key bits recovered: "<<
            yrExpanded.substr(6*i,6)<<"\tTrials:"<<numTrials<<endl;
          break;
        }
      }
    } while(true);
    numTrials = 0;
    //Reset the S-box to correct entry
    fault[i][0][0] = s[i][0][0];
  }
  cout<<endl<<"Recovering 56-bit key..."<<endl;
  //recover the 56-bit key using round 16 and 15 keys
  recoveredKey[2] = reverseKeySchedule(recoveredKey[0], recoveredKey[1]);
  return recoveredKey;

}

int main()
{
  //declare variables
  srand(time(NULL));
  int n = 16;
  string pt, key;

  key = "0BF222122231343D";

  //filestreams for plaintext and ciphertext storage
  ofstream ptout, ctout;
  ptout.open("plaintext.txt");
  ctout.open("ciphertext.txt");

  //Binarize the key
  key = hex2bin(key);

  //Recover the key
  keygen(key);
  printRoundKeys();
  vector<string> recoveredKey = automateRecovery(key);
  cout<<"Round 16 Key: "<<bin2hex(recoveredKey[0])<<endl;
  cout<<"Round 15 Key: "<<bin2hex(recoveredKey[1])<<endl;
  cout<<"Extracted Key: "<<recoveredKey[2]<<endl;
  cout<<"Original Key:  "<<bin2hex(key56)<<endl;
  ptout.close();
  ctout.close();
}
