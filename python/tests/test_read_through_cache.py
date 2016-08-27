#!/usr/bin/python
# coding=utf-8
""" unittests for read through cache """

import mock
import nose

import read_through_cache.get_user_by_id
from read_through_cache.cache import Cache, CacheException
from read_through_cache.get_user_by_id import getUserByID

def setup():
    """ setup """
    pass

def teardown():
    """ teardown """
    pass

def test_get_user_by_id():
    """  simple getUserByID checks """
    assert getUserByID(1) == "UserName #1"
    assert getUserByID(2) == "UserName #2"

def test_injected_get_user_by_id():
    """  simple injected function checks """
    with mock.patch('read_through_cache.get_user_by_id.getUserByID',
                    return_value='test_name') as injected_func:
        assert not injected_func.called
        assert read_through_cache.get_user_by_id.getUserByID(1) == "test_name"
        assert injected_func.call_count == 1

def test_get_names():
    """ get entries from a default cache """
    cache = Cache()
    assert cache.get_user_name(1) == getUserByID(1)
    assert cache.get_user_name(1) == getUserByID(1)
    assert cache.get_user_name(2) == getUserByID(2)
    assert cache.get_user_name(2) == getUserByID(2)

@nose.tools.raises(CacheException)
def test_bad_id():
    """ get entries from a default cache """
    cache = Cache()
    cache.get_user_name(-1)

def test_get_names_with_limit():
    """ get entries from a default cache with a limit on the number of entries """
    cache = Cache(1)
    assert cache.get_user_name(1) == getUserByID(1)
    assert cache.get_user_name(2) == getUserByID(2)
    assert cache.get_user_name(1) == getUserByID(1)
    assert cache.get_user_name(2) == getUserByID(2)

def test_with_injected_func():
    """ get entries from a test cache """
    with mock.patch('read_through_cache.get_user_by_id.getUserByID',
                    return_value='test_name') as injected_func:
        assert not injected_func.called
        cache = Cache()
        assert cache.get_user_name(1) == "test_name"
        assert injected_func.call_count == 1
        assert cache.get_user_name(1) == "test_name"
        assert injected_func.call_count == 1
        assert cache.get_user_name(1) == "test_name"
        assert injected_func.call_count == 1
        assert cache.get_user_name(2) == "test_name"
        assert injected_func.call_count == 2
        assert cache.get_user_name(2) == "test_name"
        assert injected_func.call_count == 2
        assert cache.get_user_name(2) == "test_name"
        assert injected_func.call_count == 2

def test_with_func_and_limit():
    """ get entries from a test cache """
    with mock.patch('read_through_cache.get_user_by_id.getUserByID',
                    return_value='test_name') as injected_func:
        assert not injected_func.called
        cache = Cache(1)
        assert cache.get_user_name(1) == "test_name"
        assert injected_func.call_count == 1
        assert cache.get_user_name(1) == "test_name"
        assert injected_func.call_count == 1
        assert cache.get_user_name(2) == "test_name"
        assert injected_func.call_count == 2
        assert cache.get_user_name(2) == "test_name"
        assert injected_func.call_count == 2
        assert cache.get_user_name(1) == "test_name"
        assert injected_func.call_count == 3
