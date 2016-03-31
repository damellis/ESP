# Smart Sensors

This project aims to help novices make sophisticated use of sensors in
interactive projects through the application of machine learning.

# Dependencies

All dependencies are under `third-party` and managed using `git
submodule`. After you checkout the repository, do the following

```
$ git submodule init
$ git submodule update
```

- [openFrameworks](http://openframeworks.cc/), a C++ toolkit for creative
  coding.

- [GRT](http://www.nickgillian.com/software/grt), Gesture Recognition Toolkit, a
  cross-platform, open-source, C++ machine learning library that has been
  specifically designed for real-time gesture recognition. See:
  [our fork of the GRT repository](https://github.com/damellis/grt).

- [ofxGrt](https://github.com/nickgillian/ofxGrt), an openFrameworks extension
  for the Gesture Recognition Toolkit (GRT). See:
  [our fork of the ofxGrt repository](https://github.com/nebgnahz/ofxGrt/tree/snapshot-for-sensors).

Because `ofxGrt` is needed as an extension to openFrameworks, it has to reside
inside `addons` folder inside `openFrameworks` project. But doing that would
make it impossible to version control `ofxGrt` (as
[submodule inside submodule](http://i1.kym-cdn.com/photos/images/newsfeed/000/531/557/a88.jpg)).
Instead, we create a symlink to make openFrameworks happy.

```
$ ln -s $(pwd)/third-party/ofxGrt/ third-party/openFrameworks/addons/ofxGrt
$ ln -s $(pwd)/third-party/ofxDatGui/ third-party/openFrameworks/addons/ofxDatGui
$ ln -s $(pwd)/third-party/ofxParagraph/ third-party/openFrameworks/addons/ofxParagraph
```

# GRT Installation

Once you've checked out the submodules, you'll need to compile and install the
GRT to /usr/local:

1. install cmake
2. cd third-party/grt/build
3. mkdir tmp
4. cd tmp
5. cmake ..
6. make
7. sudo make install

For details, see [README.md in GRT/build](https://github.com/damellis/grt/tree/master/build).

# Tips

Since openFrameworks's repo size is too
[large](https://github.com/openframeworks/openFrameworks/wiki/Moving-binaries-out-of-the-repo),
a nice hack is to control the depth by: `--depth 1` after `init`:

```
$ git submodule init
$ git submodule update --depth 1 -- third-party/openFrameworks
```
