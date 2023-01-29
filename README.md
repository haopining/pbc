# pbc docker template

## Pre Requirement Installation

- gmp library
- m4
- libtool
- automake
- autoconf

> expect aclocal, autoheader are dependencies of automake

### macOS (osX)
```bash
# reinstall libtool
brew install libtool

# install gmp library
brew install gmp


# create symbolic link for gmp header file & binary files
gmp_header=$(find /usr /opt -name "gmp.h" -type f 2>/dev/null | head -n 1)
ln -s $gmp_header /usr/local/include/gmp.h
gmp_bin=$(find /usr /opt -name "libgmp.a" -type f 2>/dev/null)
ln -s $gmp_bin /usr/local/lib/libgmp.a

# install pbc library
# run autoconfig, automake to generate configure and makefile
./setup
./configure
make
make install

# check dependencies working good
ls /usr/local/include | grep -e gmp.h -e pbc # suppose to see gmp.h & pbc both
ls /usr/local/lib | grep -e '^lib.*\.a$' # suppose to see libgmp.a and libpbc.a
```



[pbc library](https://crypto.stanford.edu/pbc/manual/)