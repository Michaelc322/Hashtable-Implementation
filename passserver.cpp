#include "passserver.h"

// Xian: in a shell, type "man crypt", and the MD5 can be found (which uses 22 characters of the password), and the usage will be shown as well.
// Note that it takes c_str, char [], not string directly as the input arguments. 
string PassServer::encrypt(const string & str) {
        char salt[] = "$1$########";
        char * password = new char [35];
        strcpy(password, crypt(str.c_str(), salt));

        string pword(password);
        string delim = "$";
        int pos = pword.find(delim, 0);
        pos = pword.find(delim, pos+1);
        pos = pword.find(delim, pos+1);
        pword.erase(0, pos+1);
        return pword;
	/*char salt[] = "$1$########";
	string ret = crypt(str.c_str(), salt);
	return ret.substr(1, 22);*/
}

PassServer::PassServer(size_t size) : h(size) {}

PassServer::~PassServer()
{
    h.clear();
}


bool PassServer::load(const char *filename)
{
    return h.load(filename);
}

bool PassServer::write_to_file(const char *filename) const
{
    return h.write_to_file(filename);
}

bool PassServer::addUser(std::pair<string, string> & kv)
{
    if(!h.contains(kv.first)){
       std::pair<string, string> encryptpair(kv.first, encrypt(kv.second));
       h.insert(encryptpair);
       return true;
    }

    return false;
}


bool PassServer::addUser(std::pair<string, string> && kv)
{
    if(!h.contains(kv.first)) {
        std::pair<string, string> encryptpair(kv.first, encrypt(kv.second));
        h.insert(std::move(encryptpair));
        return true;
    }
    
    return false;
}


bool PassServer::removeUser(const string & k)
{
    return h.remove(k);
}

bool PassServer::changePassword(const std::pair<string, string> &p, const string & newpassword){

   if(p.second == newpassword){
       return false;
   }

   if(!h.contains(p.first)){
       return false;
   }   

   std:pair<string, string> oldPair(p.first, encrypt(p.second));
   if(h.match(oldPair)){
       std::pair<string, string> encryptpair(p.first, encrypt(newpassword));
       return h.insert(encryptpair);
   }
   else{
       return false;
   }
}

bool PassServer::find(const string & user) const
{
    return h.contains(user);
}

void PassServer::dump() const{
    h.dump();
}

size_t PassServer::size() const{
    return h.size();
}
