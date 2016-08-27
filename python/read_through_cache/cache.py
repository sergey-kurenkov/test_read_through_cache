#!/usr/bin/python
# coding=utf-8
"""
Implement a simple read-through cache for Users which looks up Users by ID and
returns their name by callinggetUserByID() for users not in the cache.
It is an error to look up a user with a bad ID.
Demonstrate that the cache does not call the getUserByID() function a second
time when the same User ID is requested.
"""
from read_through_cache import get_user_by_id
from sortedcontainers import SortedDict
import itertools

class CacheException(Exception):
    """ base exception for the library """
    pass

class Cache(object):
    """ a simple read-through cache for Users which looks up Users by ID and
        returns their name """

    def __init__(self, cache_limit=None):
        self.cache_limit = cache_limit
        self.cached_names = {}
        self.cached_mru_index = SortedDict()
        self.mru_index = 0

    def get_user_name(self, user_id):
        """ looks up Users by ID and returns their name"""
        if user_id < 0:
            raise CacheException("bad id: {}".format(user_id))

        if user_id in self.cached_names:
            # cache hit
            cached_entry = self.cached_names[user_id]
            self.__update_entry(cached_entry)
            return cached_entry[0]
        else:
            # cache miss
            user_name = get_user_by_id.getUserByID(user_id)
            self.__insert_entry(user_name, user_id)
            return user_name


    def __insert_entry(self, user_name, user_id):
        """ insert entry in cache """
        self.mru_index += 1
        self.cached_names[user_id] = [user_name, self.mru_index, user_id]
        self.cached_mru_index[self.mru_index]=user_id
        self.__evict_lru_entries()

    def __update_entry(self, cached_entry):
        """ update mru_index of the entry in cache """
        del self.cached_mru_index[cached_entry[1]]
        self.mru_index += 1
        cached_entry[1] = self.mru_index
        self.cached_mru_index[self.mru_index]=cached_entry[2]

    def __evict_lru_entries(self):
        """ remove from cache least recent entries """
        if self.cache_limit is None:
            return
        if len(self.cached_names) <= self.cache_limit:
            return
        if len(self.cached_names) == 0:
            return

        min_mru_index =  next(iter(self.cached_mru_index))
        mru_user_id = self.cached_mru_index[min_mru_index]
        del self.cached_names[mru_user_id]
        del self.cached_mru_index[min_mru_index]
