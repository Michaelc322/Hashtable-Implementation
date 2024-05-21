template <typename K, typename V>
unsigned long HashTable<K, V>::prime_below(unsigned long n)
{
  if (n > max_prime)
    {
      std::cerr << "** input too large for prime_below()\n";
      return 0;
    }
  if (n == max_prime)
    {
      return max_prime;
    }
  if (n <= 1)
    {
		std::cerr << "** input too small \n";
      return 0;
    }

  // now: 2 <= n < max_prime
  std::vector <unsigned long> v (n+1);
  setPrimes(v);
  while (n > 2)
    {
      if (v[n] == 1)
	return n;
      --n;
    }

  return 2;
}

template <typename K, typename V>
void HashTable<K, V>::setPrimes(vector<unsigned long> & vprimes)
{
  int i = 0;
  int j = 0;

  vprimes[0] = 0;
  vprimes[1] = 0;
  int n = vprimes.capacity();

  for (i = 2; i < n; ++i)
    vprimes[i] = 1;

  for( i = 2; i*i < n; ++i)
    {
      if (vprimes[i] == 1)
        for(j = i + i ; j < n; j += i)
          vprimes[j] = 0;
    }
}


template <typename K, typename V>
HashTable<K, V>::HashTable(size_t size)
{
    if(prime_below(size) == 0){
        theLists.resize(default_capacity);
        currentSize = 0;
        cout << "In total there are " << theLists.size() << " rows in the vector" << endl;
    }
    else{
        theLists.resize(prime_below(size));
        currentSize = 0;
        cout << "In total there are " << theLists.size() << " rows in the vector" << endl;

    }
}



template <typename K, typename V>
HashTable<K, V>::~HashTable()
{
    clear();
}


template<typename K, typename V>
bool HashTable<K, V>::contains(const K & k) const
{
    auto & whichList = theLists[myhash(k)];
    for (const auto &pair : whichList){
        if(pair.first == k){
            return true;
        }
    }
    return false;
}


template <typename K, typename V>
bool HashTable<K, V>::match(const std::pair<K, V> &kv) const {
    const K &key = kv.first;
    const V &value = kv.second;
    size_t index = myhash(key);
    for (const auto &pair : theLists[index]) {
        if (pair == kv) {
            return true; // Key-value pair found
        }
    }
    return false; // Key-value pair not found
}


template <typename K, typename V>
bool HashTable<K, V>::remove(const K & k)
{
    auto & whichList = theLists[myhash(k)];

for (auto it = whichList.begin(); it != whichList.end(); ++it) {
        if (it->first == k) {
            whichList.erase(it); // Remove the key-value pair
            --currentSize; // Reduce currentSize
            return true; // Key deleted successfully
        }
    }

    return false;


}


template <typename K, typename V>
void HashTable<K, V>::clear()
{
    makeEmpty();
}


template <typename K, typename V>
void HashTable<K, V>::rehash()
{
    vector<list<pair<K, V>>> oldLists = theLists;
    
    theLists.resize(prime_below(2 * theLists.size()));
    for (auto & thisList: theLists)
        thisList.clear();

    currentSize = 0;
    for(auto & thisList : oldLists)
        for (auto & x : thisList)
            insert(std::move(x));
}


template <typename K, typename V>
size_t HashTable<K, V>::myhash(const K &k) const
{
    static hash<K> hf;
    cout << "hashed position of " << k << " is " << hf(k) << endl;

    return hf(k) % theLists.size();
}


template <typename K, typename V>
void HashTable<K, V>::makeEmpty()
{
    for (auto & thisList : theLists) {
        thisList.clear();
    }
}


template <typename K, typename V>
size_t HashTable<K, V>::size() const
{
    return currentSize;
}


template <typename K, typename V>
bool HashTable<K, V>::insert(const std::pair<K, V> & kv)
{
    if(match(kv)){
       return false; 
    }

    auto & whichList = theLists[myhash(kv.first)];
    if(contains(kv.first)){
    for(auto & pair : whichList) {
        if(pair.first == kv.first) {
            pair.second = kv.second;
            return true;
        }
    }
    }

    whichList.push_back(kv);
    ++currentSize;

    if (currentSize > theLists.size()) {
        rehash();
    }

    return true;
}


template <typename K, typename V>
bool HashTable<K, V>::insert(std::pair<K, V> && kv)
{
    if(match(kv)){
        return false;
    }

    size_t index = myhash(kv.first);
    auto & whichList = theLists[index];

    if(contains(kv.first)){
    for(auto & pair : whichList){
        if(pair.first == kv.first) {
            pair.second = std::move(kv.second);

            return true;
        }
    }
    }

    whichList.push_back(std::move(kv));
    ++currentSize;


    if(currentSize > theLists.size()) {
        rehash();
    }
    
    return true;

}



template<typename K, typename V>
void HashTable<K, V>::dump() const {
    cout << "In dump, in total there are " << theLists.size() << " rows in the vector." << endl;
    for (size_t i = 0; i < theLists.size(); ++i) {
        cout << "v[" << i << "]: ";
        
        const auto& currentList = theLists[i];
        if (!currentList.empty()) {
            auto it = currentList.begin();
            cout << it->first << " " << it->second;
            ++it;
            for (; it != currentList.end(); ++it) {
                cout << ":" << it->first << " " << it->second;
            }
        }
        
        std::cout << std::endl;
    }
}

template <typename K, typename V>
bool HashTable<K, V>::load(const char *filename)
{
    ifstream fh(filename);

    if (!fh.is_open()) {
        return false;
    }

    K k;
    V v;
    string line;

    while(fh >> k >> v){
        if (!insert(make_pair(k, v))) {
            cout << "Error: Key " << k << " already exists in the hash table." << endl;
        }
    }

    fh.close();
    return true;
}


template <typename K, typename V>
bool HashTable<K, V>::write_to_file(const char *filename) const
{
    ofstream fh(filename);
    if(fh.is_open()){
        for( auto & list : theLists){
            for (auto & x : list){
                fh << x.first << " " << x.second << endl;
            }
        }
        fh.close();
        return true;
    }
    else {
        return false;
    }
}
