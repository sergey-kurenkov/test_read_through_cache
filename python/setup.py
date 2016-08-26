from setuptools import setup, find_packages

config = {
    'description': 'read-through cache',
    'author': 'Sergei Kurenkov',
    'author_email': 'sergei.a.kurenkov@gmail.com',
    'version': '1.0',
    'install_requires': ['nose'],
    'packages': ['read_through_cache'],
    'scripts': [],
    'name': 'read_through_cache'
}
setup(**config)