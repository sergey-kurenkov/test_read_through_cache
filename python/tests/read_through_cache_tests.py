""" unittests for read through cache """

import mock
import read_through_cache

def setup():
    """ setup """
    pass

def teardown():
    """ teardown """
    pass

def test_get_user_by_id():
    """  simple getUserByID checks """
    assert read_through_cache.get_user_by_id.getUserByID(1) == "UserName #1"
    assert read_through_cache.get_user_by_id.getUserByID(2) == "UserName #2"

def test_injected_get_user_by_id():
    """  simple injected function checks """
    with mock.patch('read_through_cache.get_user_by_id.getUserByID',
                    return_value='test_name') as injected_func:
        assert not injected_func.called
        assert read_through_cache.get_user_by_id.getUserByID(1) == "test_name"
        assert injected_func.call_count == 1

def test_get_names():
    """ get entries from a default cache """
    cache = read_through_cache.cache.Cache()
    assert cache.get_user_name(1) == read_through_cache.get_user_by_id.getUserByID(1)
    assert cache.get_user_name(1) == read_through_cache.get_user_by_id.getUserByID(1)
    assert cache.get_user_name(2) == read_through_cache.get_user_by_id.getUserByID(2)
    assert cache.get_user_name(2) == read_through_cache.get_user_by_id.getUserByID(2)

def test_get_names_with_limit():
    """ get entries from a default cache with a limit on the number of entries """
    cache = read_through_cache.cache.Cache(1)
    assert cache.get_user_name(1) == read_through_cache.get_user_by_id.getUserByID(1)
    assert cache.get_user_name(2) == read_through_cache.get_user_by_id.getUserByID(2)
    assert cache.get_user_name(1) == read_through_cache.get_user_by_id.getUserByID(1)
    assert cache.get_user_name(2) == read_through_cache.get_user_by_id.getUserByID(2)

def test_with_injected_func():
    """ get entries from a test cache """
    with mock.patch('read_through_cache.get_user_by_id.getUserByID',
                    return_value='test_name') as injected_func:
        assert not injected_func.called
        cache = read_through_cache.cache.Cache()
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
        cache = read_through_cache.cache.Cache(1)
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

