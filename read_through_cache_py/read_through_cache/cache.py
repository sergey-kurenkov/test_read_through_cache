"""
Implement a simple read-through cache for Users which looks up Users by ID and
returns their name by callinggetUserByID() for users not in the cache.
It is an error to look up a user with a bad ID.
Demonstrate that the cache does not call the getUserByID() function a second
time when the same User ID is requested.
"""


class Cache(object):
    """ a simple read-through cache for Users which looks up Users by ID and
        returns their name """

    def __init__(self, cache_limit):
        self.cache_limit = cache_limit
        self.cached_names = {}
        self.sorted_by_mru_index = []
        self.mru_index = 0

    def __insert_entry(self, user_name, user_id):
        self.__evict_lru_entries()
        self.mru_index += 1
        self.cached_names[user_id] = {"user_name":  user_name,
                                      "mru_index": self.mru_index}
        self.sorted_by_mru_index.append((self.mru_index, user_id))
        self.sorted_by_mru_index.sort(key=lambda t: t[0])

    def __update_entry(self, cached_entry):
        pass

    def __evict_lru_entries(self):
        pass

    def get_user_name(self, user_id):
        """ looks up Users by ID and returns their name"""
        if user_id in self.cached_names:
            # cache hit
            cached_entry = self.cached_names[user_id]
            self.__update_entry(cached_entry)
            return cached_entry["user_name"]
