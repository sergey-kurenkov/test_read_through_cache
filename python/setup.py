try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

config = {
    'description': 'read-through cache',
    'author': 'Sergei Kurenkov',
    'url': 'https://github.com/skwllsp/read_through_cache',
    'download_url': 'https://github.com/skwllsp/read_through_cache',
    'author_email': 'sergei.a.kurenkov@gmail.com',
    'version': '1.0',
    'install_requires': ['nose'],
    'packages': ['read_through_cache'],
    'scripts': [],
    'name': 'read_through_cache'
}

setup(**config)