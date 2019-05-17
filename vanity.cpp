#include <iostream>
#include <cstdint>
#include <string> 
#include <bitcoin/system.hpp>
#include <chrono>

/* Vanity Passphrase Example by Decker (q) 2019 */

using namespace std;
using namespace libbitcoin::system;

/* 
    https://github.com/libbitcoin/libbitcoin-system/wiki/Examples-from-Serialised-Data
    https://stackoverflow.com/questions/505021/get-bytes-from-stdstring-in-c
    http://calaganne.blogspot.com/2017/04/libbitcoin-bx-seed.html
    
*/

size_t findCaseInsensitive(std::string data, std::string toSearch, size_t pos = 0)
{
	// Convert complete given String to lower case
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	// Convert complete given Sub String to lower case
	std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);
	// Find sub string in given string
	return data.find(toSearch, pos);
}

void check_passphrase(const std::string& start_pattern, const std::string& end_pattern, const std::string& find_pattern) {
    
    std::string passphrase;

    uint64_t i;
    hash_digest hash;

    ec_secret privkey;
    ec_compressed pubkey;
    one_byte addr_prefix;
    std::string kmd_addr, btc_addr;
    data_chunk prefix_pubkey_checksum;

    auto start = chrono::steady_clock::now();    
    
    for (i=0; i<0x7FFFFFFF; i++) {
        
        // pattern for passphrase creation
        passphrase = start_pattern + std::to_string(i) + end_pattern;

        // uncomment this if you want random passphrase
        data_chunk my_entropy(32);
        pseudo_random_fill(my_entropy);
        wallet::word_list mnemonic_words = wallet::create_mnemonic(my_entropy);
        passphrase = join(mnemonic_words); 
        

        if ((i % 100000) == 0) { 
            
            auto end = chrono::steady_clock::now();
  
            cout << "[" << std::to_string(i) << "] " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
  
            start = chrono::steady_clock::now();

        }

        /*

        https://github.com/XDAEX/API/wiki/REST_sample_cpp

        auto msg_data_slice = libbitcoin::data_slice((const unsigned char *)msg.c_str(), (const unsigned char *)msg.c_str() + msg.size());
        auto sha256sum = libbitcoin::sha256_hash(msg_data_slice);
        auto sha256sum_hex = libbitcoin::encode_base16(sha256sum);

        */

        std::vector<char> passphrase_bytes(passphrase.begin(), passphrase.end());
        // passphrase_bytes.push_back('\0');
        auto passphrase_data_slice = data_slice((const uint8_t *)passphrase_bytes.data(),(const uint8_t *)(passphrase_bytes.data() + passphrase_bytes.size()));
        auto sha256sum = sha256_hash(passphrase_data_slice);
        sha256sum[0]  = sha256sum[0] & 248;
        sha256sum[31] = sha256sum[31] & 127;
        sha256sum[31] = sha256sum[31] | 64;
        auto sha256sum_hex = encode_base16(sha256sum);
        // sha256(passbegin0passend) - 96d112c61d41264bef50c9d9ac87d174b1d24d137b5eb05263b8c9ba469970bc
        //                             90d112c61d41264bef50c9d9ac87d174b1d24d137b5eb05263b8c9ba4699707c 
        
        // cout << "Privkey: " << sha256sum_hex << endl;
        decode_base16(privkey, sha256sum_hex);
        secret_to_public(pubkey, privkey);
        std::string pubkeyhex_str = encode_base16(pubkey);
        // cout << "Pubkey: " << pubkeyhex_str << endl;

        // Pubkeyhash: sha256 + hash160
        auto my_pubkeyhash = bitcoin_short_hash(pubkey);
        
        addr_prefix = { { 60 } };
        // Byte sequence = prefix + pubkey + checksum(4-bytes)
        prefix_pubkey_checksum = to_chunk(addr_prefix);
        extend_data(prefix_pubkey_checksum, my_pubkeyhash);
        append_checksum(prefix_pubkey_checksum);
        // Base58 encode byte sequence -> Bitcoin Address
        kmd_addr = encode_base58(prefix_pubkey_checksum);

        addr_prefix = { { 0 } };
        prefix_pubkey_checksum = to_chunk(addr_prefix);
        extend_data(prefix_pubkey_checksum, my_pubkeyhash);
        append_checksum(prefix_pubkey_checksum);
        btc_addr = encode_base58(prefix_pubkey_checksum);

        //cout << "BTC: " << btc_addr << endl;
        //cout << "KMD: " << kmd_addr << endl;

        size_t pos = findCaseInsensitive(kmd_addr, find_pattern);
        if( pos != std::string::npos) {
            // cout << "KMD: " << kmd_addr << "\tPassphrase: '" << passphrase << "'" << endl;
            cout << "KMD: " << kmd_addr.substr(0,pos) << "\x1B[33m" << kmd_addr.substr(pos, find_pattern.size()) << "\033[0m" << kmd_addr.substr(pos + find_pattern.size()) << "\tPassphrase: '" << passphrase << "'" << endl;
        }
        
        int len = 3;
        if (btc_addr.substr(1,len) == kmd_addr.substr(1,len)) {
            cout << "KMD: " << kmd_addr << ", BTC: " << btc_addr << "\tPassphrase: '" << passphrase << "'" << endl;
        }
    }

    return;
}

int main() 
{ 
    std::string start_pattern = "beginofyourpassphrase";
    std::string end_pattern = "endofyourpassphrase";
    
    check_passphrase(start_pattern, end_pattern, "komod");

    return 0; 
}