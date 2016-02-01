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
  specifically designed for real-time gesture recognition.

- [ofxGrt](https://github.com/nickgillian/ofxGrt), an openFrameworks extension
  for the Gesture Recognition Toolkit (GRT).

Because `ofxGrt` is needed as an extension to openFrameworks, it has to reside
inside `addons` folder inside `openFrameworks` project. But doing that would
make it impossible to version control `ofxGrt` (as
[submodule inside submodule](http://i1.kym-cdn.com/photos/images/newsfeed/000/531/557/a88.jpg)).
Instead, we create a symlink to make openFrameworks happy.

```
$ ln -s third-party/ofxGrt third-party/openFrameworks/addons/ofxGrt
```

We are currently using stable versions (master branch) of all of them.

# Tips

Since openFrameworks's repo size is too
[large](https://github.com/openframeworks/openFrameworks/wiki/Moving-binaries-out-of-the-repo),
a nice hack is to control the depth by: `--depth 1` after `init`:

```
$ git submodule init
$ git submodule update --depth 1 -- third-party/openFrameworks
```
