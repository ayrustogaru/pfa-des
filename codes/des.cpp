#include <bits/stdc++.h>
using namespace std;
#include "des.h"
#define ALPHA_SIZE 16

string key56 = "";
vector<string> rk(16); //RoundKeys in hexadecimal
// hexadecimal to binary conversion
string hex2bin(string s)
{
	unordered_map<char, string> mp;
	mp['0'] = "0000";
	mp['1'] = "0001";
	mp['2'] = "0010";
	mp['3'] = "0011";
	mp['4'] = "0100";
	mp['5'] = "0101";
	mp['6'] = "0110";
	mp['7'] = "0111";
	mp['8'] = "1000";
	mp['9'] = "1001";
	mp['A'] = "1010";
	mp['B'] = "1011";
	mp['C'] = "1100";
	mp['D'] = "1101";
	mp['E'] = "1110";
	mp['F'] = "1111";
	string bin = "";
	for (int i = 0; i < s.size(); i++) {
		bin += mp[s[i]];
	}
	return bin;
}

// binary to hexadecimal conversion
string bin2hex(string s)
{
	unordered_map<string, string> mp;
	mp["0000"] = "0";
	mp["0001"] = "1";
	mp["0010"] = "2";
	mp["0011"] = "3";
	mp["0100"] = "4";
	mp["0101"] = "5";
	mp["0110"] = "6";
	mp["0111"] = "7";
	mp["1000"] = "8";
	mp["1001"] = "9";
	mp["1010"] = "A";
	mp["1011"] = "B";
	mp["1100"] = "C";
	mp["1101"] = "D";
	mp["1110"] = "E";
	mp["1111"] = "F";
	string hex = "";
	for (int i = 0; i < s.length(); i += 4) {
		string ch = "";
		ch += s[i];
		ch += s[i + 1];
		ch += s[i + 2];
		ch += s[i + 3];
		hex += mp[ch];
	}
	return hex;
}

//function to permute a string
string permute(string k, int* arr, int n)
{
	string per = "";
    for (int i = 0; i < n; i++) {
	if(arr[i]!=-1)
        	per += k[arr[i] - 1];
	else
		per += "0";
    }
    return per;
}

//function to shift-left a string
string shift_left(string k, int shifts)
{
	string s = "";
	for (int i = 0; i < shifts; i++) {
		for (int j = 1; j < 28; j++) {
			s += k[j];
		}
		s += k[0];
		k = s;
		s = "";
	}
	return k;
}

//function to OR two strings
string or_(string a, string b)
{
    string ans = "";
    for (int i = 0; i < a.size(); i++) {
        if ((a[i] =='0') && (b[i] == '0')) {
            ans += "0";
        }
        else {
            ans += "1";
        }
    }
    return ans;
}

//function to XOR two strings
string xor_(string a, string b)
{
	string ans = "";
	for (int i = 0; i < a.size(); i++) {
		if (a[i] == b[i]) {
			ans += "0";
		}
		else {
			ans += "1";
		}
	}
	return ans;
}

/*
Round function ind DES cipher system
Parameters:
@pt - Plaintext
@rkb - vector of 16 round keys
@s - S-box that needs to be used in the round function
returns a vector of hexadecimal strings which correspont to round keys
*/
string encrypt(string pt, vector<string> rkb, int s[8][4][16])
{

	// Hexadecimal to binary
	pt = hex2bin(pt);
	// Initial Permutation
	pt = permute(pt, initial_perm, 64);

	// Splitting
	string left = pt.substr(0, 32);
	string right = pt.substr(32, 32);

	for (int i = 0; i < 16; i++) {
		// Expansion D-box
		string right_expanded = permute(right, exp_d, 48);

		// XOR RoundKey[i] and right_expanded
		string x = xor_(rkb[i], right_expanded);

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
		// Straight D-box
		if(i<15)
			op = permute(op, per, 32);

		// XOR left and op
		x = xor_(op, left);

		left = x;
		// Swapper
		if (i != 15) {
			swap(left, right);
		}
	}

	// Combination
	string combine = left + right;
	//Final Permutation
	string cipher = bin2hex(permute(combine, final_perm, 64));
	// string cipher = bin2hex(combine);
	return cipher;
}

/*
Funcion to generate the 48-bit round keys from the 64-bit master key
Parameters:
@key - 64-bit master key
returns a vector of hexadecimal strings which correspont to round keys
*/
vector<string> keygen(string key)
{
	// getting 56 bit key from 64 bit by dropping the parity bits
	key = permute(key, keyp, 56); // key without parity

	key56 = key;
	// Splitting
	string left = key.substr(0, 28);
	string right = key.substr(28, 28);

	vector<string> rkb(16); // rkb for RoundKeys in binary
	for (int i = 0; i < 16; i++) {
		// Shifting
		left = shift_left(left, shift_table[i]);
		right = shift_left(right, shift_table[i]);

		// Combining
		string combine = left + right;

		// Key Compression
		string RoundKey = permute(combine, key_comp, 48);

		rkb[i] = RoundKey;
	}
	for(int i = 0; i<16; ++i)
	{
		rk[i] = bin2hex(rkb[i]);// Hexadecimal Keys
	}
	return rkb;
}

string des(string pt, string key, int s[8][4][16]){

	vector<string> rkb(16); //RoundKeys in binary
	rkb = keygen(key);

	string cipher = encrypt(pt, rkb, s);
	return cipher;
}


void printRoundKeys()
{
	for(int i = 0; i<16; ++i)
	{
		printf("Round %02d Key: ", i);
		cout<<rk[i]<<endl;
	}
}
/*
Function for random hexadecimal plaintext generation
Parameters: @n - size of the Plaintext
returns plaintext of size n
*/
string genRandomHexString(int n)
{
    char alphabet[ALPHA_SIZE] = { '0','1','2','3','4','5','6','7','8',
                          '9','A','B','C','D','E','F'};

    string res = "";
    for (int i = 0; i < n; i++)
        res = res + alphabet[rand() % ALPHA_SIZE];

    return res;
}

//function for converting string to uint64_t
uint64_t strtouint64(string str){
  return strtoull(str.c_str(), NULL, 16);
}

//function for converting uint64_t to string
string uint64tostr(uint64_t hex){
  string str = "";
  ostringstream oss;
  oss<<hex;
  str += oss.str();
  return str;
}
