#include "lfu.h"
#include <sstream>

static void lfu_parse (string entry, string& value, uint64_t& freq, uint64_t& stamp) {
    istringstream ss(entry);
    ss >> value >> freq >> stamp;
}

static bool lfu_read (Lfu* c, string key, string& value, uint64_t& freq, uint64_t& stamp) {
    string entry;
    if (!ct_get(c->data, key, entry)) return false;
    lfu_parse(entry, value, freq, stamp);
    return true;
}

void lfu_put (Lfu* c, string key, string value, uint64_t freq, uint64_t stamp) {
    ct_put(c->data, key, value + " " + to_string(freq) + " " + to_string(stamp));
}

bool lfu_get (Lfu* c, string key, string& value) {
    uint64_t freq, stamp;
    if (!lfu_read(c, key, value, freq, stamp)) return false;
    c->clock++;
    lfu_put(c, key, value, freq + 1, c->clock);
    return true;
}

static void lfu_evict (Lfu* c) {
    string worst;
    uint64_t worst_freq = 0;
    uint64_t worst_stamp = 0;
    bool found = false;
    for (int i = 0; i < c->data->cap; i++) {
        for (KVNode* cur = c->data->buckets[i]; cur != nullptr; cur = cur->next) {
            string value;
            uint64_t freq, stamp;
            lfu_parse(cur->value, value, freq, stamp);
            if (!found || freq < worst_freq || (freq == worst_freq && stamp < worst_stamp)) {
                worst = cur->key;
                worst_freq = freq;
                worst_stamp = stamp;
                found = true;
            }
        }
    }
    if (found) ct_remove(c->data, worst);
}

void lfu_set (Lfu* c, string key, string value) {
    if (c->cap == 0) return;
    c->clock++;
    string old;
    uint64_t freq, stamp;
    if (lfu_read(c, key, old, freq, stamp)) {
        lfu_put(c, key, value, freq + 1, c->clock);
        return;
    }
    if (c->data->sz >= c->cap) lfu_evict(c);
    lfu_put(c, key, value, 1, c->clock);
}

void lfu_print (Lfu* c) {
    if (c->data->sz == 0) {
        cout << "Кэш пуст\n";
        return;
    }
    cout << "Кэш (емкость " << c->cap << "):\n";
    for (int i = 0; i < c->data->cap; i++) {
        for (KVNode* cur = c->data->buckets[i]; cur != nullptr; cur = cur->next) {
            string value;
            uint64_t freq, stamp;
            lfu_parse(cur->value, value, freq, stamp);
            cout << cur->key << "=" << value << " (использований: " << freq << ")" << endl;
        }
    }
}
