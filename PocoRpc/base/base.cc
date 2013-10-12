#include "base.h"
#include "pystring.h"
#include "popen.h"
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <Poco/Exception.h>
#include <Poco/DigestStream.h>
#include <Poco/SHA1Engine.h>
#include <Poco/MD5Engine.h>
#include <Poco/Format.h>
#include <Poco/Crypto/Cipher.h>
#include <Poco/Crypto/CipherKey.h>
#include <Poco/Crypto/CipherFactory.h>

std::string int2string(int32 v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

std::string int2string(int64 v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

int32 str2int32(const std::string& str) {
  long v;
  char* pEnd;
  v = strtol(str.c_str(), &pEnd, 0);
  return v;
}

int64 str2int64(const std::string& str) {
  int64 v;
  char* pEnd;
  v = strtoll(str.c_str(), &pEnd, 0);
  return v;
}

std::string Sha1String(const std::string& input_str) {
  Poco::SHA1Engine sha1_eng;
  Poco::DigestOutputStream digest_out(sha1_eng);
  digest_out << input_str;
  digest_out.close();
  return Poco::DigestEngine::digestToHex(sha1_eng.digest());
}

std::string Md51String(const std::string& input_str) {
  Poco::MD5Engine md5_eng;
  Poco::DigestOutputStream digest_out(md5_eng);
  digest_out << input_str;
  digest_out.close();
  return Poco::DigestEngine::digestToHex(md5_eng.digest());
}

const static std::string EncryptSalt("9($J>#_g^a>Wf&xdCNp1K8YFx5DqQ{UmgW^7L_0+C6,UEf{>JXnwF[^{iIGAm7sXlufjGVzeUy{Rb852bB8%u`=xqVqviN6-P_<R&4[XuW*g-'J}[x}]QK'4as5UrZO%");
const static std::string Password("vDU7$_(BKen,uK5FmaJoF>x?=GkSa%5;EsSkGaH6k/14xk+Ox04$6=zb]{{/N6$a");

std::string EncryptString(const std::string& val) {
  
  Poco::Crypto::CipherKey key("aes256", Password, EncryptSalt);
  Poco::Crypto::Cipher::Ptr pCipher = Poco::Crypto::CipherFactory::defaultFactory().createCipher(key);
  return pCipher->encryptString(val, Poco::Crypto::Cipher::ENC_BINHEX_NO_LF);
}

std::string DecryptString(const std::string& val) {
  Poco::Crypto::CipherKey key("aes256", Password, EncryptSalt);
  Poco::Crypto::Cipher::Ptr pCipher = Poco::Crypto::CipherFactory::defaultFactory().createCipher(key);
  return pCipher->decryptString(val, Poco::Crypto::Cipher::ENC_BINHEX_NO_LF);
}

bool tryDecryptString(const std::string& val, std::string* plain_txt) {
  try {
    *plain_txt = DecryptString(val);
    return true;
  } catch (...) {
    return false;
  }
}

std::string ReadFromFile(const std::string& file_path, bool strip) {
  std::fstream fs(file_path.c_str(), std::ios::in | std::ios::binary);
  std::stringstream ss;
  char buf[1024];
  ::memset(buf, 0, sizeof(buf));
  
  while (!fs.eof()) {
    fs.read(buf, sizeof (buf));
    ss << buf;
    ::memset(buf, 0, sizeof(buf));
  }
  fs.close();
  if (strip) {
    return pystring::strip(ss.str());
  } else {
    return ss.str();
  }
}

bool WriteToFile(const std::string& file_path, const std::string& value) {
  try {
    std::fstream fs(file_path.c_str(), std::ios::out | std::ios::binary);
    fs << value;
    fs.close();
  } catch (std::exception ex) {
    return false;
  }
  return true;
}

std::string CatFile(const std::string& file_path, bool strip) {
  std::string cmd = Poco::format("cat %s", file_path);
  scoped_ptr<com::Popen> p(new com::Popen(cmd));
  p->run();
  std::stringstream ss;
  com::StringList::iterator it = p->output_lines()->begin();
  for (; it != p->output_lines()->end(); ++it) {
    ss << *it;
  }
  if (strip) {
    return pystring::strip(ss.str());
  } else {
    return ss.str();
  }
}

